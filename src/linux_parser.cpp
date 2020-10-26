#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>
#include <iostream>
// include <filesystem> - can only be used on machines with up-to-date libraries

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem - completed BONUS on my machine but
// would never work inside my udacity workspace
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  // This code here would work if Udacity systems were up-to-date have to use
  // the old code below until then,
  // for (auto &p : std::filesystem::directory_iterator(kProcDirectory)) {
  //   const auto file_name = p.path().filename().string();
  //   if(p.is_directory()) {
  //     if(std::all_of(file_name.begin(), file_name.end(), isdigit)) {
  //       int pid = stoi(file_name);
  //       pids.push_back(pid);
  //     }
  //   }
  // }
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string start_of_line;
  float memory_total = 0.0;
  float memory_free = 0.0;
  string line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line) &&
           (memory_total == 0 || memory_free == 0)) {
      std::istringstream linestream(line);
      linestream >> start_of_line;
      if (start_of_line == "MemTotal:") {
        linestream >> memory_total;
      } else if (start_of_line == "MemFree:") {
        linestream >> memory_free;
      }
    }
    return (memory_total - memory_free) / memory_total;
  }
  return 0;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  long up_time, idle_time;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> up_time >> idle_time;
  }
  return up_time;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  string cpu;
  long jiffy = 0;
  long jiffies = 0;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    while (linestream >> jiffy) {
      jiffies += jiffy;
    }
  }
  return jiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int _pid) {
  string line, pid, comm, state;
  long ppid, pgrp, session, tty_nr, tpgid, flags, minflt, cminflt, majflt,
      cmajflt, utime, stime, cutime, cstime;
  std::ifstream filestream(kProcDirectory + std::to_string(_pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr >>
          tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime >>
          stime >> cutime >> cstime;
    }
  }
  return utime + stime + cutime + cstime;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  string cpu;
  long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >>
        softirq >> steal >> guest >> guest_nice;
  }
  return user + nice + system + iowait + irq + softirq + steal + guest +
         guest_nice;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  string cpu;
  long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  long jiffies = 0;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >>
        softirq >> steal >> guest >> guest_nice;
  }
  return idle;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string data;
  vector<string> cpu_stats{};
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> data) {
      cpu_stats.push_back(data);
    }
    cpu_stats.erase(cpu_stats.begin());
  }
  return cpu_stats;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string start_of_line;
  int number_of_processes;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> start_of_line;
      if (start_of_line == "processes") {
        linestream >> number_of_processes;
        return number_of_processes;
      }
    }
  }
  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string start_of_line;
  int procs_running;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> start_of_line;
      if (start_of_line == "procs_running") {
        linestream >> procs_running;
        return procs_running;
      }
    }
  }
  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kCmdlineFilename);
  if (filestream.is_open()) {
    if (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> value;
    }
  }
  return value;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  std::string line, key;
  int ram;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> ram;
      if (key == "VmRSS:") {
        ram = ram / 1000;
        return std::to_string(ram);
      }
    }
  }
  return "0"; 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line;
  long start_time;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int index = 0; index < 22; index++) {
      std::istringstream linestream(line);
      linestream >> start_time;
    }
  }
  return start_time;
}

float LinuxParser::CpuUtilization(int pid) {
  long total_proc_time = ActiveJiffies(pid);
  long total_system_uptime = UpTime();
  long proc_start_time = UpTime(pid);
  long total_seconds =
      total_system_uptime - (proc_start_time / sysconf(_SC_CLK_TCK));

  return  (float(total_proc_time) / sysconf(_SC_CLK_TCK)) / float(total_seconds);
}