#include <unistd.h>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::vector;

string LinuxParser::OperatingSystem() {
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    string line, key, value;
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
    return value;
  }

  throw std::runtime_error("Unable to read operating system information from " + kOSPath);
}

string LinuxParser::Kernel() {
  std::ifstream filestream(kProcDirectory + kVersionFilename);
  if (filestream.is_open()) {
    string os, kernel, version, line;
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
    return kernel;
  }
  throw std::runtime_error("Unable to get Kernel info from " + kProcDirectory + kVersionFilename);
}


vector<int> LinuxParser::Pids() {
  vector<int> pids;
  for (const auto &file : std::filesystem::directory_iterator(kProcDirectory)) {
    if (file.is_directory()) {
      string filename = file.path().filename();
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = std::stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  return pids;
}


float LinuxParser::MemoryUtilization() {
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    string line, key, value, metric;
    long total_memory{0}, memory_free{0};
    for (int i = 0; i < 2; i++) {
      std::getline(filestream, line);
      std::istringstream iss(line);
      iss >> key >> value >> metric;
      if (key == "MemTotal:") {
        total_memory = std::stol(value);
      } else if (key == "MemFree:") {
        memory_free = std::stol(value);
      }
    }
    return static_cast<float>(total_memory - memory_free) / total_memory;
  }

  throw std::runtime_error("Unable to read system memory utilization in " + kProcDirectory + kMeminfoFilename);
}


long LinuxParser::SystemUpTime() {
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    string line;
    std::getline(filestream, line);
    std::istringstream linestream(line);
    long uptime_in_seconds;
    linestream >> uptime_in_seconds;
    return uptime_in_seconds;
  }

  throw std::runtime_error("Unable to read system uptime in " + kProcDirectory + kUptimeFilename);
}


std::unordered_map<string, long> LinuxParser::CpuUtilizationValues() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    string line, name;
    std::unordered_map<string, long> values = {
      {"user", 0},
      {"nice", 0},
      {"system", 0},
      {"idle", 0},
      {"iowait", 0},
      {"irq", 0},
      {"softirq", 0},
      {"steal", 0},
      {"guest", 0},
      {"guest_nice", 0},
    };
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> name >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
    values["user"] =  user;
    values["nice"] =  nice;
    values["system"] =  system;
    values["idle"] =  idle;
    values["iowait"] =  iowait;
    values["irq"] =  irq;
    values["softirq"] =  softirq;
    values["steal"] =  steal;
    values["guest"] =  guest;
    values["guest_nice"] =  guest_nice;

    return values;

  }

  throw std::runtime_error("Unable to read system cpu utilization values from " + kProcDirectory + kStatFilename);
}


float LinuxParser::CpuUtilization(int pid) {
  string file_path = kProcDirectory + "/" + std::to_string(pid) + kStatFilename;
  std::ifstream filestream(file_path);
  if (filestream.is_open()) {
    string line, value;
    std::getline(filestream, line);
    std::istringstream linestream(line);
    long utime{0}, stime{0}, cutime{0}, cstime{0}, starttime{0}, total_time{0}, seconds{0};
    for (int i = 1; i < 23; i++) {
      linestream >> value;

      switch (i) {
        case 14:
          utime = std::stol(value);
          break;
        case 15:
          stime = std::stol(value);
          break;
        case 16:
          cutime = std::stol(value);
          break;
        case 17:
          cstime = std::stol(value);
          break;
        case 22:
          starttime = std::stol(value);
          break;
      }
    }

    total_time = utime + stime + cutime + cstime;
    seconds = SystemUpTime() - (starttime / sysconf(_SC_CLK_TCK));

    if (total_time == 0) { return 0; }

    return static_cast<float>(total_time / sysconf(_SC_CLK_TCK)) / seconds;
  }

  throw std::runtime_error("Unable to calculate process cpu utilization from " + file_path);
}


int LinuxParser::TotalProcesses() { return Pids().size(); }


int LinuxParser::RunningProcesses() {
  int running_processes = 0;
  string file_path, line, key, short_hand, value;

  for (int pid: Pids()) {
    file_path = kProcDirectory + "/" + std::to_string(pid) + kStatFilename;
    std::ifstream filestream(file_path);
    if (filestream.is_open()) {

      while (std::getline(filestream, line)) {
        std::istringstream linestream(line);
        linestream >> key;
        if (key == "State:") {
          linestream >> key >> short_hand >> value;
          if (value == "(running)") {
            running_processes++;
          }
          break;
        }
      }
    } else {
      throw std::runtime_error("Unable to read process status in " + file_path);
    }
  }

  return running_processes;
}


string LinuxParser::Command(int pid) {
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    string command, line;
    while (std::getline(filestream, line)){
      command += " " + line;
    }
    return command;
  }

  throw std::runtime_error("Unable to find the command to run process " + std::to_string(pid));
}


string LinuxParser::Ram(int pid) {
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    string line, key, value;
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmData:") {
        // convert from kb to mb
        float ram = std::stof(value);
        ram *= 0.001;
        std::ostringstream outputstream;
        outputstream << std::fixed << std::setprecision(2) << ram;
        return outputstream.str();
      }
    }
  }

  throw std::runtime_error("Unable to find VmSize for process " + std::to_string(pid));
}


string LinuxParser::Uid(int pid) {
  string file_path = kProcDirectory + "/" + std::to_string(pid) + kStatusFilename;
  std::ifstream filestream(file_path);
  if (filestream.is_open()) {
    string line, key, value;
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") {
        return value;
      }
    }
  }

  throw std::runtime_error("Unable to read process UID in " + file_path);
}


string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    string line, value;
    vector<string> values{3};
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      for (int i = 0; i < 3; i++) {
        std::getline(linestream, value, ':');
        values[i] = value;
        if (value == uid) {
          return values[0];
        }
      }
    }
  }

  throw std::runtime_error("Unable to find user with uid " + uid + " in " + kPasswordPath);
}


long LinuxParser::UpTime(int pid) {
  string file_path = kProcDirectory + "/" + std::to_string(pid) + kStatFilename;
  std::ifstream filestream(file_path);
  if (filestream.is_open()) {
    string line, value;
    std::getline(filestream, line);
    std::istringstream linestream(line);
    long starttime{0};
    for (int i = 1; i < 23; i++) {
      linestream >> value;
      if (i == 22) {
        starttime = std::stol(value);
        break;
      }
    }
    return SystemUpTime() - (starttime / sysconf(_SC_CLK_TCK));;
  }

  throw std::runtime_error("Unable to calculate uptime for process " + std::to_string(pid) + " in " + file_path);
}
