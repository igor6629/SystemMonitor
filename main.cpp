#include <iostream>
#include <string>
#include <vector>
#include <ncurses.h>
#include <time.h>
#include "Util.h"
#include "SysInfo.h"
#include "ProcessContainer.h"

using namespace std;

char *getCString(string str) 
{
  char *cstr = new char[str.length() + 1];
  strcpy(cstr, str.c_str());
  return cstr;
}

void writeSysInfoToConsole(SysInfo sys, WINDOW* sys_win)
{
    sys.setAttributes();

    mvwprintw(sys_win,2,2,getCString(( "OS: " + sys.getOSName())));
    mvwprintw(sys_win,3,2,getCString(( "Kernel version: " + sys.getKernelVersion())));
    mvwprintw(sys_win,4,2,getCString( "CPU: "));
    wattron(sys_win,COLOR_PAIR(1));
    wprintw(sys_win,getCString(Util::getProgressBar(sys.getCpuPercent())));
    wattroff(sys_win,COLOR_PAIR(1));
    mvwprintw(sys_win,5,2,getCString(( "Other cores: ")));
    wattron(sys_win,COLOR_PAIR(1));
    vector<string> val = sys.getCoresStats();
    
    for (int i = 0; i < val.size(); ++i)
        mvwprintw(sys_win,(6+i),2,getCString(val[i]));
    
    wattroff(sys_win,COLOR_PAIR(1));
    mvwprintw(sys_win,10,2,getCString(( "Memory: ")));
    wattron(sys_win,COLOR_PAIR(1));
    wprintw(sys_win,getCString(Util::getProgressBar(sys.getMemPercent())));
    wattroff(sys_win,COLOR_PAIR(1));
    mvwprintw(sys_win,11,2,getCString(( "Total Processes: " + sys.getTotalProc())));
    mvwprintw(sys_win,12,2,getCString(( "Running Processes: " + sys.getRunningProc())));
    mvwprintw(sys_win,13,2,getCString(( "Up Time: " + Util::convertToTime(sys.getUpTime()))));
}

void getProcessListToConsole(ProcessContainer procs, WINDOW* win)
{
    procs.refreshList();
    wattron(win,COLOR_PAIR(2));
    
    mvwprintw(win,1,2,"PID:");
    mvwprintw(win,1,9,"User:");
    mvwprintw(win,1,18,"CPU[%]:");
    mvwprintw(win,1,28,"RAM[MB]:");
    mvwprintw(win,1,37,"Uptime:");
    mvwprintw(win,1,46,"CMD:");
    wattroff(win, COLOR_PAIR(2));
    
    vector<string> processes = procs.getList();
    
    for(int i = 0; i < 10; ++i)
        mvwprintw(win, 2 + i, 2, getCString(processes[i]));
}

void printMain(SysInfo sys,ProcessContainer procs)
{
    initscr();
    noecho();
    cbreak();
    start_color();
    
    int yMax,xMax;
    getmaxyx(stdscr,yMax,xMax);
    WINDOW *sys_win = newwin(17,xMax-1,0,0);
    WINDOW *proc_win = newwin(15,xMax-1,18,0);

    init_pair(1,COLOR_BLUE,COLOR_BLACK);
    init_pair(2,COLOR_GREEN,COLOR_BLACK);

    while (true) 
    {
        box(sys_win,0,0);
        box (proc_win,0,0);
        writeSysInfoToConsole(sys,sys_win);
        getProcessListToConsole(procs,proc_win);
        wrefresh(sys_win);
        wrefresh(proc_win);
        refresh();
        sleep(1);
    }
    
    endwin();
}

int main( int   argc, char *argv[] )
{
    ProcessContainer procs;
    SysInfo sys;

    printMain(sys,procs);
    
    return 0;
}