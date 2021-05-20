#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int p)
  : pid_(p) {
    cpu_utilization_ = LinuxParser::CpuUtilization(pid_);
    command_ = LinuxParser::Command(pid_);
    ram_ = LinuxParser::Ram(pid_);
    user_ = LinuxParser::User(pid_);
    uptime_ = LinuxParser::UpTime(pid_);
  }

int Process::Pid() { return pid_; }

float Process::CpuUtilization() { return cpu_utilization_; }

string Process::Command() { return command_; }

string Process::Ram() { return ram_; }

string Process::User() { return user_; }

long int Process::UpTime() { return uptime_; }
