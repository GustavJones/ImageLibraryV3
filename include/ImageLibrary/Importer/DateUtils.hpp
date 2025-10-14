#pragma once
#include <cstdint>
#include <chrono>
#include <vector>
#include <string>

namespace ImageLibrary {
  std::tm ParseTime(const std::string &_timeBuffer);

  template<typename CharT>
  std::tm ParseTime(const std::vector<CharT>& _timeBuffer) {
    std::string timeBuffer(_timeBuffer.begin(), _timeBuffer.end());
    return ParseTime(timeBuffer);
  }

}
