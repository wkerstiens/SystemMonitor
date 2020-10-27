#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid, std::string user, std::string command,
                 float cpu_utilization, std::string ram, long uptime) {
  this->pid = pid;
  this->user = user;
  this->command = command;
  this->cpu_utilization = cpu_utilization;
  this->ram = ram;
  this->uptime = uptime;
}

// TODO: Return this process's ID
int Process::Pid() const { return pid; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const { return cpu_utilization; }

// TODO: Return the command that generated this process
string Process::Command() const { return command; }
// TODO: Return this process's memory utilization
string Process::Ram() const { return ram; }

// TODO: Return the user (name) that generated this process
string Process::User() const { return user; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() const { return uptime; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
  return this->CpuUtilization() < a.CpuUtilization();
}

bool Process::operator>(Process const& a) const {
  return this->CpuUtilization() > a.CpuUtilization();
}