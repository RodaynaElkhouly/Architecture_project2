#include "register_file.h"
#include <iostream>

RegisterFile::RegisterFile() {
    for (int i = 0; i < 8; ++i) {
        regs[i] = 0;
        status[i] = "";
    }
}

int RegisterFile::read(int reg) {
    if (reg < 0 || reg >= 8) {
        std::cerr << "Register read out of bounds: R" << reg << std::endl;
        return 0;
    }
    return regs[reg];
}

void RegisterFile::write(int reg, int value) {
    if (reg == 0) return; // R0 is hardwired to 0
    if (reg < 0 || reg >= 8) {
        std::cerr << "Register write out of bounds: R" << reg << std::endl;
        return;
    }
    regs[reg] = value;
}