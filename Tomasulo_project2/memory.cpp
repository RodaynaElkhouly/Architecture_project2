#include "memory.h"
#include <iostream>

Memory::Memory() {
    // Example initial memory setup
    mem[10] = 42;  // For testing LOAD
    mem[12] = 99;  // Optional additional data
}

int Memory::read(int addr) {
    if (addr < 0 || addr >= 65536) {
        std::cerr << "Memory read out of bounds at address: " << addr << std::endl;
        return 0;
    }
    return mem[addr];
}

void Memory::write(int addr, int value) {
    if (addr < 0 || addr >= 65536) {
        std::cerr << "Memory write out of bounds at address: " << addr << std::endl;
        return;
    }
    mem[addr] = value;
}

void Memory::loadData(int addr, int value) {
    write(addr, value);
}