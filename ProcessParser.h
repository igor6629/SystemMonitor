#ifndef PROCESSPARSER_H
#define PROCESSPARSER_H

#include <algorithm>
#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>
#include <iterator>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <filesystem>
#include "constants.h"
#include "Util.h"

using namespace std;

class ProcessParser 
{
public:
    static string getCmd(string pid);
    static vector<string> getPidList();
    static string getVmSize(string pid);
    static string getCpuPercent(string pid);
    static long int getSysUpTime();
    static string getProcUpTime(string pid);
    static string getProcUser(string pid);
    static vector<string> getSysCpuPercent(string coreNumber);
    static float getSysActiveCpuTime(vector<string> time);
    static float getSysIdleCpuTime(vector<string> time);
    static float getSysRamPercent();
    static string getSysKernelVersion();
    static int getTotalThreads();
    static int getTotalNumberOfProcesses();
    static int getNumberOfRunningProcesses();
    static string getOsName();
    static int getNumberOfCores();
    static string printCpuStats(vector<string> previus, vector<string>current);
};

// getCmd returns the command that executed the PID
string ProcessParser::getCmd(string pid)
{
    string line;
    ifstream stream = Util::getStream((Path::basePath() + pid + Path::cmdPath()));
    getline(stream, line);
    
    return line;
}

// getPidList returns list of all PIDs 
vector<string> ProcessParser::getPidList()
{
    vector<string> pidList; 

    for (const auto & entry : filesystem::directory_iterator(Path::basePath())) 
    {
        string current = entry.path();
        current = current.substr(6, current.size());
        
        if (current[0] >= '0' && current[0] <= '9')
            pidList.push_back(current);
    }

    return pidList;
}

// getVmSize returns VM size of current PID
string ProcessParser::getVmSize(string pid)
{
    string name = "VmData";
    string line;
    float size = 0;

    ifstream stream = Util::getStream(Path::basePath() + pid + Path::statusPath());

    while (getline(stream, line))
    {
        if (line.compare(0, name.size(), name) == 0)
        {
            vector<string> values = Util::splitString(line);

            size = (stof(values[1]) / float(1024));
            break;
        }
    }

    return to_string(size);
}

// getCpuPercent returns the CPU load value in percent for the current PID
string ProcessParser::getCpuPercent(string pid)
{
    string line;
    float result;
   
    ifstream stream = Util::getStream(Path::basePath() + pid + "/" + Path::statPath());
    getline(stream, line);
    
    vector<string> values = Util::splitString(line);

    float utime = stof(ProcessParser::getProcUpTime(pid));
    float stime = stof(values[14]);
    float cutime = stof(values[15]);
    float cstime = stof(values[16]);
    float starttime = stof(values[21]);
    float uptime = ProcessParser::getSysUpTime();
    
    float freq = sysconf(_SC_CLK_TCK);
    
    float totalTime = utime + stime + cutime + cstime;
    float seconds = uptime - (starttime / freq);
    
    result = 100.0 * ((totalTime / freq) / seconds);
    
    return to_string(result);
}

// getProcUpTime returns Process Up Time
string ProcessParser::getProcUpTime(string pid)
{
    string line;

    ifstream stream = Util::getStream(Path::basePath() + pid + "/" + Path::statPath());
    getline(stream, line);
    
    vector<string> values = Util::splitString(line);

    return to_string(float(stof(values[13]) / sysconf(_SC_CLK_TCK)));
}

// getSysUpTime returns System Up Time
long int ProcessParser::getSysUpTime()
{
    string line;

    ifstream stream = Util::getStream(Path::basePath() + Path::upTimePath());
    getline(stream, line);
    
    vector<string> values = Util::splitString(line);

    return stoi(values[0]);
}

// getProcUser returns the name of the user who started this PID
string ProcessParser::getProcUser(string pid)
{
    string name = "Uid";
    string userId;
    string line;

    ifstream stream = Util::getStream(Path::basePath() + pid + Path::statusPath());

    while (getline(stream, line))
    {
        if (line.compare(0, name.size(), name) == 0)
        {
            vector<string> values = Util::splitString(line);

            userId = values[1];
            break;
        }
    }

    stream = Util::getStream("/etc/passwd");

    while (getline(stream, line))
    {
        vector<string> result;
        stringstream ss(line);
        string token;
        
        while (getline(ss, token, ':')) 
        {
            result.push_back(token);
        }

        if (result[2] == userId)
            return result[0];
    }

    return "";
}

// getNumberOfCores returns the number of CPU cores on the host
int ProcessParser::getNumberOfCores()
{
    string name = "cpu cores";
    string line;

    ifstream stream = Util::getStream(Path::basePath() + "cpuinfo");

    while (getline(stream, line))
    {
        if (line.compare(0, name.size(), name) == 0)
        {
            vector<string> values = Util::splitString(line);

            return stoi(values[3]);
        }
    }

    return 0;
}

// getSysCpuPercent returns information on overall CPU usage, as well stats for individual cores
vector<string> ProcessParser::getSysCpuPercent(string coreNumber = "")
{
    string name = "cpu" + coreNumber;
    string line;

    ifstream stream = Util::getStream(Path::basePath() + Path::statPath());

    while (getline(stream, line))
    {
        if (line.compare(0, name.size(), name) == 0)
            return Util::splitString(line);
    }

    return {};
}

// printCpuStats returns calculates CPU usage, either overall or for a selected core, has two parameters: previous and current time
string ProcessParser::printCpuStats(vector<string> previus, vector<string>current)
{
    float activeTime = getSysActiveCpuTime(current) - getSysActiveCpuTime(previus);
    float idleTime = getSysIdleCpuTime(current) - getSysIdleCpuTime(previus);
    float totalTime = activeTime + idleTime;
    float result = 100.0 * (activeTime / totalTime);
    
    return to_string(result);
}

// getSysActiveCpuTime returns system active CPU time
float ProcessParser::getSysActiveCpuTime(vector<string> time)
{
    return stof(time[S_USER]) +
          stof(time[S_NICE]) +
          stof(time[S_SYSTEM]) +
          stof(time[S_IRQ]) +
          stof(time[S_SOFTIRQ]) +
          stof(time[S_STEAL]) +
          stof(time[S_GUEST]) +
          stof(time[S_GUEST_NICE]);
}

// getSysIdleCpuTime returns system idle CPU time
float ProcessParser::getSysIdleCpuTime(vector<string> time)
{
    return stof(time[S_IDLE]) + stof(time[S_IOWAIT]);
}

// getSysRamPercent returns the RAM utilization in percentages
float ProcessParser::getSysRamPercent()
{
    string memAvailable = "MemAvailable:";
    string memFree = "MemFree:";
    string buffers = "Buffers:";
    string line;

    ifstream stream = Util::getStream(Path::basePath() + Path::memInfoPath());

    bool memAvailableB = false;
    bool memFreeB = false;
    bool buffersB = false;

    float memAvailableF = 0;
    float memFreeF = 0;
    float buffersF = 0;

    while (getline(stream, line))
    {
        if (memAvailableB && memFreeB && buffersB)
            break;

        if (line.compare(0, memAvailable.size(), memAvailable) == 0)
        {
            vector<string> values = Util::splitString(line);

            memAvailableF = stof(values[1]);
            memAvailableB = true;
        }

        if (line.compare(0, memFree.size(), memFree) == 0)
        {
            vector<string> values = Util::splitString(line);

            memFreeF = stof(values[1]);
            memFreeB = true;
        }

        if (line.compare(0, buffers.size(), buffers) == 0)
        {
            vector<string> values = Util::splitString(line);

            buffersF = stof(values[1]);
            buffersB = true;
        }
    }

    return float(100.0 * (1 - (memFreeF / (memAvailableF - buffersF))));
}

// getSysKernelVersion returns data about the kernel version
string ProcessParser::getSysKernelVersion()
{
    string line;
    ifstream stream = Util::getStream(Path::basePath() + Path::versionPath());

    getline(stream, line);

    vector<string> values = Util::splitString(line);

    return values[2];
}

// getOsName returns the name of the operating system
string ProcessParser::getOsName()
{
    string line;
    string name = "PRETTY_NAME";

    ifstream stream = Util::getStream("/etc/os-release");

    while (getline(stream, line))
    {
        if (line.compare(0, name.size(), name) == 0)
        {
            string result = line.substr(13, line.size() - 13 - 1);
            return result;
        }
    }

    return "";
}

// getTotalThreads returns the total number of threads
int ProcessParser::getTotalThreads()
{
    vector<string> pids = ProcessParser::getPidList();
    string name = "Threads"; 
    string line;
    int threads = 0;

    for (const auto& pid : pids)
    {
        ifstream stream = Util::getStream(Path::basePath() + pid + Path::statusPath());

        while (getline(stream, line))
        {
            if (line.compare(0, name.size(), name) == 0)
            {
                vector<string> values = Util::splitString(line);

                threads += stoi(values[1]);
                break;
            }
        }
    }

    return threads;
}

// getTotalNumberOfProcesses returns total number of processes
int ProcessParser::getTotalNumberOfProcesses()
{
    string name = "processes"; 
    string line;

    ifstream stream = Util::getStream(Path::basePath() + Path::statPath());

    while (getline(stream, line))
    {
        if (line.compare(0, name.size(), name) == 0)
        {
            vector<string> values = Util::splitString(line);

            return stoi(values[1]);
        }
    }

    return 0;
}

// getNumberOfRunningProcesses returns number of running processes
int ProcessParser::getNumberOfRunningProcesses()
{
    string name = "procs_running"; 
    string line;

    ifstream stream = Util::getStream(Path::basePath() + Path::statPath());

    while (getline(stream, line))
    {
        if (line.compare(0, name.size(), name) == 0)
        {
            vector<string> values = Util::splitString(line);
            
            return stoi(values[1]);
        }
    }

    return 0;
}

#endif // PROCCESSPARSER_H