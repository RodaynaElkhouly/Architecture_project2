#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "instruction.h"
#include "reservation_station.h"
#include "register_file.h"
#include "memory.h"
#include <vector>
#include <string>
using namespace std;

class TomasuloSimulator {
public:
    TomasuloSimulator();

    void loadProgram(const string& filename);
    void simulate();
    void printStats();

    Memory& getMemory();
    RegisterFile& getRegisters();

private:
    int cycle;
    int completed;
    int branches;
    int mispredictions;
    int programCounter;

    bool branchPending = false;
    int pendingBEQIndex = -1;

    vector<Instruction> program;
    vector<ReservationStation> RS;
    vector<pair<int, int>> completedWrites;

    RegisterFile RF;
    Memory MEM;

    void issue();
    void execute();
    void writeBack();

    int executeOp(OpCode op, int a, int b, int imm, int instrIndex);
    int getLatency(OpCode op);
};

#endif // SIMULATOR_H
