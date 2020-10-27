#include "format.h"

#include <iomanip>
#include <sstream>

using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  long hours = seconds / 3600;
  long minutes = (seconds / 60) % 60;
  long real_seconds = seconds % 60;

  return string(Format::PadLeft(to_string(hours), '0', 2) + ":" +
                Format::PadLeft(to_string(minutes), '0', 2) + ":" +
                Format::PadLeft(to_string(real_seconds), '0', 2));
}

string Format::CleanNumber(int number) {
  return string(Format::PadRight(to_string(number), ' ', 10));
}

string Format::PadLeft(string s, char c, unsigned long l) {
  if (s.size() >= l) return s;
  s.insert(s.begin(), l - s.size(), c);
  return s;
}

string Format::PadRight(string s, char c, unsigned long l) {
  if (s.size() >= l) return s;
  s.insert(s.end(), l - s.size(), c);
  return s;
}