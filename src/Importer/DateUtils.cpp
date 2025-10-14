#include "ImageLibrary/Importer/DateUtils.hpp"
#include <stdexcept>

namespace ImageLibrary {
  std::tm ParseTime(const std::string& _timeBuffer) {
    if (_timeBuffer.size() != 20)
    {
      throw std::runtime_error("Invalid time buffer");
    }

    std::tm output {0};

    std::string yearStr(_timeBuffer.begin(), _timeBuffer.begin() + 4);
    std::string monthStr(_timeBuffer.begin() + 5, _timeBuffer.begin() + 7);
    std::string dayStr(_timeBuffer.begin() + 8, _timeBuffer.begin() + 10);
    std::string hourStr(_timeBuffer.begin() + 11, _timeBuffer.begin() + 13);
    std::string minuteStr(_timeBuffer.begin() + 14, _timeBuffer.begin() + 16);
    std::string secondStr(_timeBuffer.begin() + 17, _timeBuffer.begin() + 19);

    output.tm_year = std::stoi(yearStr) - 1900;
    output.tm_mon = std::stoi(monthStr) - 1;
    output.tm_mday = std::stoi(dayStr);

    output.tm_hour = std::stoi(hourStr) - 1;
    output.tm_min = std::stoi(minuteStr) - 1;
    output.tm_sec = std::stoi(secondStr) - 1;

    return output;
  }
}