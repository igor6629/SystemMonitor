#ifndef SYSINFO_H
#define SYSINFO_H

#include <string>
#include <iostream>
#include <vector>
#include "ProcessParser.h"

using namespace std;

class SysInfo 
{
private:
    vector<string> lastCpuStats;
    vector<string> currentCpuStats;
    vector<string> coresStats;
    
    vector<vector<string>> lastCpuCoresStats;
    vector<vector<string>> currentCpuCoresStats;
    
    string cpuPercent;
    string OSname;
    string kernelVer;

    float memPercent;
    long upTime;
    int totalProc;
    int runningProc;
    int threads;

public:
    SysInfo() 
    {
      this->getOtherCores(ProcessParser::getNumberOfCores());
      this->setLastCpuMeasures();
      this->setAttributes();
      this->OSname = ProcessParser::getOsName();
      this->kernelVer = ProcessParser::getSysKernelVersion();
    }

  void setAttributes();
  void setLastCpuMeasures();
  void setCpuCoresStats();

  long getUpTime() const;
  string getMemPercent() const;
  string getThreads() const;
  string getTotalProc() const;
  string getRunningProc() const;
  string getKernelVersion() const;
  string getOSName() const;
  string getCpuPercent() const;
  void getOtherCores(int size);
  vector<string> getCoresStats() const;
};

// getOtherCores initializes attributes of the class
void SysInfo::getOtherCores(int size) 
{
  this->coresStats = vector<string>(size);
  this->lastCpuCoresStats = vector<vector<string>>(size);
  this->currentCpuCoresStats = vector<vector<string>>(size);

  for (int i = 0; i < size; ++i)
    this->lastCpuCoresStats[i] = ProcessParser::getSysCpuPercent(to_string(i));
}

void SysInfo::setLastCpuMeasures() 
{
  this->lastCpuStats = ProcessParser::getSysCpuPercent();
}

// setCpuCoresStats updates and creates new datasets for CPU calculation
// every core is updated and previous data becomes the current data of calculated measures
void SysInfo::setCpuCoresStats() 
{
  for (int i = 0; i < this->currentCpuCoresStats.size(); ++i)
    this->currentCpuCoresStats[i] = ProcessParser::getSysCpuPercent(to_string(i));

  for (int i = 0; i < this->currentCpuCoresStats.size(); ++i)
    this->coresStats[i] = ProcessParser::printCpuStats(this->lastCpuCoresStats[i], this->currentCpuCoresStats[i]);

  this->lastCpuCoresStats = this->currentCpuCoresStats;
}

// setAttributes initializes or refreshes an object
void SysInfo::setAttributes() 
{
  this->memPercent = ProcessParser::getSysRamPercent();
  this->upTime = ProcessParser::getSysUpTime();
  this->totalProc = ProcessParser::getTotalNumberOfProcesses();
  this->runningProc = ProcessParser::getNumberOfRunningProcesses();
  this->threads = ProcessParser::getTotalThreads();
  this->currentCpuStats = ProcessParser::getSysCpuPercent();
  this->cpuPercent = ProcessParser::printCpuStats(this->lastCpuStats, this->currentCpuStats);
  this->lastCpuStats = this->currentCpuStats;
  this->setCpuCoresStats();
}


// getCoresStats creates a string that represents a progress bar
// shows the current status of aggregate CPU utilization, or the utilization of a selected core.
vector<string> SysInfo::getCoresStats() const 
{
  vector<string> result;

  for (int i = 0; i < this->coresStats.size(); ++i) 
  {
    string temp = ("cpu" + to_string(i) + ":   ");
    float check = 0;

    if (!this->coresStats[i].empty())
      check = stof(this->coresStats[i]);
    
    if (!check || this->coresStats[i] == "nan")
      return {};

    temp += Util::getProgressBar(this->coresStats[i]);
    result.push_back(temp);
  }
  
  return result;
}

string SysInfo::getCpuPercent() const 
{
  return this->cpuPercent;
}

string SysInfo::getMemPercent() const 
{
  return to_string(this->memPercent);
}

long SysInfo::getUpTime() const 
{
  return this->upTime;
}

string SysInfo::getKernelVersion() const 
{
  return this->kernelVer;
}

string SysInfo::getTotalProc() const 
{
  return to_string(this->totalProc);
}

string SysInfo::getRunningProc() const 
{
  return to_string(this->runningProc);
}

string SysInfo::getThreads() const 
{
  return to_string(this->threads);
}

string SysInfo::getOSName() const 
{ 
  return this->OSname; 
}

#endif // SYSINFO_H