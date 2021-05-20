#include <iomanip>
#include <sstream>
#include <string>

#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) {
  int hours = seconds / 3600;
  int remaining_seconds = seconds % 3600;
  int minutes = remaining_seconds / 60;
  remaining_seconds = remaining_seconds % 60;

  std::ostringstream oss;
  oss << std::setw(2) << std::setfill('0') << hours << ':'
      << std::setw(2) << std::setfill('0') << minutes << ':'
      << std::setw(2) << std::setfill('0') << remaining_seconds;

  return oss.str();
}