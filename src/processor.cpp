#include <string>

#include "processor.h"
#include "linux_parser.h"

using std::string;

float Processor::Utilization() {
  std::unordered_map<std::string, long> values = LinuxParser::CpuUtilizationValues();

  long new_idle = values["idle"] + values["iowait"];
  long new_non_idle = values["user"] + values["nice"] + values["system"] + values["irq"] + values["softirq"] + values["steal"];

  long new_total = new_idle + new_non_idle;
  long total_difference = new_total - total_;
  long idle_difference = new_idle - idle_;

  float cpu_utilization = static_cast<float>(total_difference - idle_difference) / total_difference;

  //update member variables
  idle_ = new_idle;
  total_ = new_total;

  return cpu_utilization;
}
