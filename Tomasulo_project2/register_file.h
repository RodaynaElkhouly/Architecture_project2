#ifndef REGISTER_FILE_H
#define REGISTER_FILE_H

#include <string>
using namespace std;

class RegisterFile {
public:
    RegisterFile();
    int read(int reg);
    void write(int reg, int value);

private:
    int regs[8];           // 8 general-purpose registers (R0–R7)
public:
    string status[8];      // Reservation status for each register
};

#endif