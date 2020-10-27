#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using namespace std;

System::System() {
  // Going to set variables that shoud not change
  // over the course of the execution
  this->operating_system = LinuxParser::OperatingSystem();
  this->kernel = LinuxParser::Kernel();
}

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  vector<int> pids = LinuxParser::Pids();
  processes_.clear();
  for (auto pid : pids) {
    Process process(pid, LinuxParser::User(pid), LinuxParser::Command(pid),
                    LinuxParser::CpuUtilization(pid), LinuxParser::Ram(pid),
                    LinuxParser::UpTime(pid));
    processes_.push_back(process);
  }
  sort(processes_.begin(), processes_.end(),
       [](const Process& a, const Process& b) { return a > b; });
  return processes_;
}

// TODO: Return the system's kernel identifier (string)
string System::Kernel() const { return kernel; }

// TODO: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// TODO: Return the operating system name
string System::OperatingSystem() const { return operating_system; }

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// TODO: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }