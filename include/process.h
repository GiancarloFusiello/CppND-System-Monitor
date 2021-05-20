#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for static Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int p);
  int Pid();
  float CpuUtilization();
  std::string Command();
  std::string Ram();
  std::string User();
  long int UpTime();

 private:
  int pid_;
  float cpu_utilization_{0};
  std::string command_;
  std::string ram_;
  std::string user_;
  long int uptime_{0};
};

#endif