#ifndef MEMORY_H
#define MEMORY_H

class Memory {
public:
    Memory();
    int read(int addr);
    void write(int addr, int value);
    void loadData(int addr, int value);

private:
    int mem[65536] = {0}; // 16-bit addressable memory (128KB)
};

#endif