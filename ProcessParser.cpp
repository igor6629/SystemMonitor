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

// getCmd returns Cmd info of current pid
string ProcessParser::getCmd(string pid)
{
    string line;
    ifstream stream = Util::getStream((Path::basePath() + pid + Path::cmdPath()));
    getline(stream, line);
    
    return line;
}

// getPidList returns list of all pids 
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

// getVmSize returns VM size of current pid
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