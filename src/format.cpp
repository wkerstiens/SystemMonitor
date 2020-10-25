#include <sstream>
#include <iomanip>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    long hours = seconds / 3600;
    long minutes = seconds % 3600 / 60;
    long real_seconds = seconds % 3600  % 60;

    std::ostringstream output;
    output << std::setfill('0') << std::setw(2) << hours << ":" 
        << std::setfill('0') << std::setw(2) << minutes << ":" 
        << std::setfill('0') << std::setw(2) << real_seconds;
    return  output.str();
}