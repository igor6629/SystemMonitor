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
#include <constants.h>
#include <util.h>

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
    static vector<string> getSysCpuPercent(string coreNumber = "");
    static float getSysRamPercent();
    static string getSysKernelVersion();
    static int getTotalThreads();
    static int getTotalNumberOfProcesses();
    static int getNumberOfRunningProcesses();
    static string getOsName();
    static string printCpuStats(vector<string> values1, vector<string>values2);
};

// getCmd returns Cmd info of current PID
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
    float size;

    ifstream stream = Util::getStream(Path::basePath() + pid + Path::statusPath());

    while (getline(stream, line))
    {
        if (line.compare(0, name.size(), name) == 0)
        {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);

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
    string str = line;
    
    istringstream buf(str);
    istream_iterator<string> beg(buf), end;
    vector<string> values(beg, end);

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
    string str = line;
    
    istringstream buf(str);
    istream_iterator<string> beg(buf), end;
    vector<string> values(beg, end);

    return to_string(float(stof(values[13]) / sysconf(_SC_CLK_TCK)));
}

// getSysUpTime returns System Up Time
long int ProcessParser::getSysUpTime()
{
    string line;

    ifstream stream = Util::getStream(Path::basePath() + Path::upTimePath());
    getline(stream, line);
    
    istringstream buf(line);
    istream_iterator<string> beg(buf), end;
    vector<string> values(beg, end);

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
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);

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