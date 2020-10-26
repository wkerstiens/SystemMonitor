
#include "processor.h"

#include <string>

#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  std::vector<std::string> stats = LinuxParser::CpuUtilization();
  float total_jiffies = 0;
  float active_jiffies = 0;
  for (int index = 0; index < stats.size(); index++) {
    float converted_value = std::stof(stats[index]);
    switch (index) {
      case 0:  // user
      case 1:  // nice
      case 2:  // system
        active_jiffies += converted_value;
      default:
        total_jiffies += converted_value;
        break;
    }
  }
  if (previous_total_jiffies != 0) {
    float return_value = (active_jiffies - previous_active_jiffies) /
                         (total_jiffies - previous_total_jiffies);
    previous_total_jiffies = total_jiffies;
    previous_active_jiffies = active_jiffies;
    return return_value;
  }
  previous_total_jiffies = total_jiffies;
  previous_active_jiffies = active_jiffies;
  return total_jiffies != 0 ? active_jiffies / total_jiffies : 0;
}