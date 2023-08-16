# System Monitor for Linux

The system monitor shows you what’s happening on your computer: processes, id, CPU and memory usage, and much more. Object-oriented C++ project for Linux similar to the widely used htop application

![](/img/system-monitor.png)

## Instruction

1. Clone the project repository: 
```
git clone https://github.com/igor6629/SystemMonitor.git
```

2. Install `ncurses` package:
```
sudo apt-get install libncurses5-dev libncursesw5-dev
```

3. Compile and run:
```
g++ -std="c++17" main.cpp -lncurses
./a.out
```

4. In case of error that looks like the following:
```
nechepurenko@VB:~/source/SystemMonitor$ ./a.out
*** %n in writable segment detected ***
                                Aborted (core dumped)
```

just keep trying `./a.out` and it should work eventually!

### ncurses

[ncurses](https://invisible-island.net/ncurses/announce.html) is a library that facilitates text-based graphical output in the terminal. This project relies on ncurses for display output.

Install **ncurses** within your own Linux environment: `sudo apt install libncurses5-dev libncursesw5-dev`