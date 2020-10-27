#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <vector>
// include <filesystem> - can only be used on machines with up-to-date libraries

#include "linux_parser.h"
// using std::stof;
// using std::string;
// using std::to_string;
// using std::vector;
// using std::ifstream;
// using std::istringstream;

using namespace std;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  ifstream stream(kOSPath);
  if (stream.is_open()) {
    while (getline(stream, line)) {
      replace(line.begin(), line.end(), ' ', '_');
      replace(line.begin(), line.end(), '=', ' ');
      replace(line.begin(), line.end(), '"', ' ');
      istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          replace(value.begin(), value.end(), '_', ' ');
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
  ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    getline(stream, line);
    istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use filesystem - completed BONUS on my machine but
// would never work inside my udacity workspace
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  // This code here would work if Udacity systems were up-to-date have to use
  // the old code below until then,
  // for (auto &p : filesystem::directory_iterator(kProcDirectory)) {
  //   const auto file_name = p.path().filename().string();
  //   if(p.is_directory()) {
  //     if(all_of(file_name.begin(), file_name.end(), isdigit)) {
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
      if (std::all_of(filename.begin(), filename.end(), ::isdigit)) {
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
  string line, key, value;
  float memory_total, memory_free, memory_usage;
  ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (getline(stream, line)) {
      replace(line.begin(), line.end(), ':', ' ');  // From review
      istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal") {
          memory_total = stof(value);
        }
        if (key == "MemFree") {
          memory_free = stof(value);
        }
      }
    }
  }
  memory_usage =
      memory_total == 0 ? 0 : (memory_total - memory_free) / memory_total;
  return memory_usage;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  long up_time, idle_time;
  string line;
  ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    getline(stream, line);
    istringstream linestream(line);
    linestream >> up_time >> idle_time;
  }
  return up_time;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return LinuxParser::IdleJiffies() +
         LinuxParser::ActiveJiffies();  // from review
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int _pid) {
  string line, pid, comm, state;
  long ppid, pgrp, session, tty_nr, tpgid, flags, minflt, cminflt, majflt,
      cmajflt, utime, stime, cutime, cstime;
  ifstream stream(kProcDirectory + to_string(_pid) + kStatFilename);
  if (stream.is_open()) {
    while (getline(stream, line)) {
      istringstream linestream(line);
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
  ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    getline(stream, line);
    istringstream linestream(line);
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
  string line;
  ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    getline(stream, line);
    istringstream linestream(line);
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
  ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    getline(stream, line);
    istringstream linestream(line);
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
  ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (getline(stream, line)) {
      istringstream linestream(line);
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
  ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (getline(stream, line)) {
      istringstream linestream(line);
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
  ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    if (getline(stream, line)) {
      istringstream linestream(line);
      linestream >> value;
    }
  }
  return value;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line, key, value;
  int ram;
  ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (getline(stream, line)) {
      istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:") {
        ram = stoi(value) / 1024;
      }
    }
  }
  return to_string(ram);
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line, key, uid;
  ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (getline(stream, line)) {
      istringstream linestream(line);
      linestream >> key >> uid;
      if (key == "Uid:") {
        return uid;
      }
    }
  }
  return "0";
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  int uid = stoi(Uid(pid));
  string line, user, password;
  int file_uid;

  ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (getline(stream, line)) {
      replace(line.begin(), line.end(), ':', ' ');
      istringstream linestream(line);
      linestream >> user >> password >> file_uid;
      if (file_uid == uid) {
        return user;
      }
    }
  }
  return "__USER_NOT_FOUND__";
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line, value;
  vector<string> all_data = {};
  ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    getline(stream, line);
    istringstream linestream(line);
    while (linestream >> value) {
      all_data.push_back(value);
    }
  }
  return LinuxParser::UpTime() - stol(all_data[21]) / sysconf(_SC_CLK_TCK);
}

float LinuxParser::CpuUtilization(int pid) {
  long total_proc_time = ActiveJiffies(pid);
  long total_system_uptime = UpTime();
  long proc_start_time = UpTime(pid);
  long total_seconds =
      total_system_uptime - (proc_start_time / sysconf(_SC_CLK_TCK));

  return (float(total_proc_time) / sysconf(_SC_CLK_TCK)) / float(total_seconds);
}