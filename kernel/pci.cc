#include "pci.h"

#include "asmfunc.h"

namespace {

/** @brief CONFIG_ADDRESS 用の 32 ビット整数を生成する */
uint32_t MakeAddress(uint8_t bus, uint8_t device, uint8_t function,
                     uint8_t reg_addr) {
  auto shl = [](uint32_t x, uint32_t bits) { return x << bits; };
  return shl(1, 31) | shl(bus, 16) | shl(device, 11) | shl(function, 8) |
         (reg_addr & 0xfcu);
}

/** @brief devices[num_device] に情報を書き込み num_device
 * をインクリメントする． */
Error AddDevice(uint8_t bus, uint8_t device, uint8_t function,
                uint8_t header_type) {
  if (pci::num_device == pci::devices.size()) {
    return Error{Error::Code::kFull};
  }
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
  pci::devices[pci::num_device] =
      pci::Device{bus, device, function, header_type};
  ++pci::num_device;
  return Error{Error::Code::kSuccess};
}

Error ScanBus(uint8_t bus);

/** @brief 指定のファンクションを devices に追加する．
 * もし PCI-PCI ブリッジなら，セカンダリバスに対し ScanBus を実行する
 */
Error ScanFunction(uint8_t bus, uint8_t device, uint8_t function) {
  const auto header_type = pci::ReadHeaderType(bus, device, function);
  if (const auto err = AddDevice(bus, device, function, header_type); err) {
    return err;
  }
  const auto class_code = pci::ReadClassCode(bus, device, function);
  const uint8_t base = (class_code >> 24u) & 0xffu;
  const uint8_t sub = (class_code >> 16u) & 0xffu;
  if (base == 0x06u && sub == 0x04u) {
    // standard PCI-PCI bridge
    const auto bus_numbers = pci::ReadBusNumbers(bus, device, function);
    const uint8_t secondary_bus = (bus_numbers >> 8u) & 0xffu;
    return ScanBus(secondary_bus);
  }
  return Error{Error::Code::kSuccess};
}

/** @brief 指定のデバイス番号の各ファンクションをスキャンする．
 * 有効なファンクションを見つけたら ScanFunction を実行する．
 */
Error ScanDevice(uint8_t bus, uint8_t device) {
  if (const auto err = ScanFunction(bus, device, 0u); err) {
    return err;
  }
  if (pci::IsSingleFunctionDevice(pci::ReadHeaderType(bus, device, 0u))) {
    return Error{Error::Code::kSuccess};
  }
  for (uint8_t function = 1; function < 8u; ++function) {
    if (pci::ReadVendorId(bus, device, function) == 0xffffu) {
      continue;
    }
    if (const auto err = ScanFunction(bus, device, function); err) {
      return err;
    }
  }
  return Error{Error::Code::kSuccess};
}

/** @brief 指定のバス番号の各デバイスをスキャンする．
 * 有効なデバイスを見つけたら ScanDevice を実行する．
 */
Error ScanBus(uint8_t bus) {
  for (uint8_t device = 0; device < 32; ++device) {
    if (pci::ReadVendorId(bus, device, 0) == 0xffffu) {
      continue;
    }
    if (const auto err = ScanDevice(bus, device); err) {
      return err;
    }
  }
  return Error{Error::Code::kSuccess};
}

}  // unnamed namespace

namespace pci {

void WriteAddress(uint32_t address) { IoOut32(kConfigAddress, address); }

void WriteData(uint32_t value) { IoOut32(kConfigData, value); }

uint32_t ReadData() { return IoIn32(kConfigData); }

uint16_t ReadVendorId(uint8_t bus, uint8_t device, uint8_t function) {
  WriteAddress(MakeAddress(bus, device, function, 0x00u));
  return ReadData() & 0xffffu;
}

uint16_t ReadDeviceId(uint8_t bus, uint8_t device, uint8_t function) {
  WriteAddress(MakeAddress(bus, device, function, 0x00u));
  return (ReadData() >> 16u) & 0xffu;
}

uint8_t ReadHeaderType(uint8_t bus, uint8_t device, uint8_t function) {
  WriteAddress(MakeAddress(bus, device, function, 0x0cu));
  return (ReadData() >> 16u) & 0xffu;
}

uint32_t ReadClassCode(uint8_t bus, uint8_t device, uint8_t function) {
  WriteAddress(MakeAddress(bus, device, function, 0x08u));
  return ReadData();
}

uint32_t ReadBusNumbers(uint8_t bus, uint8_t device, uint8_t function) {
  WriteAddress(MakeAddress(bus, device, function, 0x18u));
  return ReadData();
}

bool IsSingleFunctionDevice(uint8_t header_type) {
  return (header_type & 0x80u) == 0;
}

Error ScanAllBus() {
  num_device = 0;

  const auto header_type = ReadHeaderType(0u, 0u, 0u);
  if (IsSingleFunctionDevice(header_type)) {
    return ScanBus(0u);
  }

  for (uint8_t function = 1u; function < 8u; ++function) {
    if (ReadVendorId(0u, 0u, function) == 0xffffu) {
      continue;
    }
    if (const auto err = ScanBus(function); err) {
      return err;
    }
  }
  return Error{Error::Code::kSuccess};
}

}  // namespace pci
