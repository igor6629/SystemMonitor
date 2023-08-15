#include "Process.h"
#include <string>
#include <vector>

using namespace std;

class ProcessContainer 
{
public:
    ProcessContainer()
    {
        this->refreshList();
    }
    
    void refreshList();
    string printList();
    vector<string> getList();

private:
    vector<Process> processes;
};

void ProcessContainer::refreshList()
{
    vector<string> pids = ProcessParser::getPidList();
    this->processes.clear();
    
    for (const auto& pid : pids) 
    {
        Process proc(pid);
        this->processes.push_back(proc);
    }
}

string ProcessContainer::printList()
{
    string result;
    
    for (auto& i : processes)
        result += i.getProcess();
    
    return result;
}

vector<string> ProcessContainer::getList() 
{
    vector<string> values;
    
    for (int i = this->processes.size() - 10; i < this->processes.size(); ++i)
        values.push_back(this->processes[i].getProcess());
    
    return values;
}