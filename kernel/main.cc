#include <cstdint>

extern "C" void KernelMain(uint64_t frame_buffer_base,
                           uint64_t frame_buffer_size) {
  auto frame_buffer = reinterpret_cast<uint8_t*>(
      frame_buffer_base);  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
  for (uint64_t i = 0; i < frame_buffer_size; ++i) {
    frame_buffer[i] = i % 256;
  }
  while (true) {
    __asm__("hlt");  // NOLINT(hicpp-no-assembler)
  }
}
