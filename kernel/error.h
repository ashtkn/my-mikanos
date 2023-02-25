#pragma once

#include <array>
#include <string_view>

class Error {
 public:
  enum class Code {
    kSuccess,
    kFull,
    kEmpty,
  };
  explicit Error(Code code) : code_{code} {};
  explicit operator bool() const noexcept { return code_ != Code::kSuccess; }
  std::string_view Name() const { return code_names_[static_cast<int>(code_)]; }

 private:
  static constexpr std::array<std::string_view, 3> code_names_{
      "kSuccess",
      "kFull",
      "kEmpty",
  };

  Code code_;
};
