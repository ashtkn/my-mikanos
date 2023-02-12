#include <cstdint>

extern "C" void KernelMain(std::uintptr_t frame_buffer_base,
                           std::uintptr_t frame_buffer_size) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  auto frame_buffer = reinterpret_cast<uint8_t*>(frame_buffer_base);
  for (auto i = 0; i < frame_buffer_size; ++i) {
    frame_buffer[i] = i % 256;
  }
  while (true) {
    __asm__("hlt");  // NOLINT(hicpp-no-assembler)
  }
}
