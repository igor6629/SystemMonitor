#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>
#include "ProcessParser.h"

using namespace std;

class Process
{
private:
    string PID;
    string User;
    string CMD;
    string CPU;
    string Memory;
    string UpTime;

public:
    Process(string PID)
    {
        this->PID = PID;
        this->User = ProcessParser::getProcUser(PID);
        this->CMD = ProcessParser::getCmd(PID);
        this->CPU = ProcessParser::getCpuPercent(PID);
        this->Memory = ProcessParser::getVmSize(PID);
        this->UpTime = ProcessParser::getProcUpTime(PID);
    }

    void setPID(string PID);
    
    string getPID() const;
    string getUser() const;
    string getCMD() const;
    int getCPU() const;
    int getMemory() const;
    string getUpTime() const;
    string getProcess();
};

void Process::setPID(string PID)
{
    this->PID = PID;
}

string Process::getPID() const
{
    return this->PID;
}

string Process::getUser() const
{
    return this->User;
}

string Process::getCMD() const
{
    return this->CMD;
}

int Process::getCPU() const
{
    return stoi(this->CPU);
}

int Process::getMemory() const
{
    return stoi(this->Memory);
}

string Process::getUpTime() const
{
    return this->UpTime;
}

// getProcess returns all information about process
string Process::getProcess()
{
    this->Memory = ProcessParser::getVmSize(this->PID);
    this->UpTime = ProcessParser::getProcUpTime(this->PID);
    this->CPU = ProcessParser::getCpuPercent(this->PID);

    string currentCMD = this->CMD;

    if (currentCMD.size() >= 25)
        currentCMD = currentCMD.substr(0, 25) + "...";

    return (this->PID + "   "
                    + this->User.substr(0,4)
                    + "     "
                    + this->Memory.substr(0,5)
                    + "     "
                    + this->CPU.substr(0,5)
                    + "     "
                    + this->UpTime.substr(0,5)
                    + "    "
                    + currentCMD);
}

#endif // PROCESS_H