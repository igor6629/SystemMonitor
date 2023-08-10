class Process
{
private:
    string PID;
    string User;
    string CMD;
    float CPU;
    uint_t Memory;
    Time time;

public:
    string GetPID() { return PID; }
    string GetUser() { return User; }
    string GetCMD() { return CMD; }
    float GetCPU() { return CPU; }
    uint_t GetPID() { return Memory; }
    Time GetTime() { return time; }

    void SetPID(string PID);
    void SetUser(string user);
    void SetCMD(string CMD);
    void SetCPU(float CPU);
    void SetMemory(uint_t memory);
    void SetTime(Time time);
};

void Process::SetPID(string PID)
{
    this->PID = PID;
}

int main()
{

    return 0;
}