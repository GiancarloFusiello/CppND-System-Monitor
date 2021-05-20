#include <algorithm>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::string;
using std::vector;

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() {

  processes_.clear();

  for (int pid: LinuxParser::Pids()){
    try {
      processes_.emplace_back(pid);
    } catch (std::runtime_error &exception) {}
  }

  // sort by cpu usage descending
  sort(processes_.begin(), processes_.end(), [](Process &a, Process &b){
    return a.CpuUtilization() > b.CpuUtilization();
  });

  return processes_;
}

std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long System::UpTime() { return LinuxParser::SystemUpTime(); }
