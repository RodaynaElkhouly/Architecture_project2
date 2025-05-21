#include "simulator.h"
#include <iostream>
#include <fstream>
#include <functional>
using namespace std;

void runTest(const string& filename, function<void(TomasuloSimulator&)> memInit) {
    TomasuloSimulator sim;
    sim.loadProgram(filename);
    memInit(sim); // setup memory
    cout << "\n== Running " << filename << " ==\n";
    sim.simulate();

    cout << "\nFinal Register File:\n";
    for (int i = 0; i < 8; ++i)
        cout << "R" << i << " = " << sim.getRegisters().read(i) << "\n";

    cout << "\nFinal Memory Snapshot:\n";
    for (int addr : {10, 20, 50, 60, 100, 104, 108, 200})
        cout << "MEM[" << addr << "] = " << sim.getMemory().read(addr) << "\n";
}

int main() {
    // === Test Case 6: Two BEQs (one taken, one not taken) ===
    {
        ofstream f("tc6.txt");
        f << "LOAD R1, 100(R0)\n";       // R1 = 5
        f << "ADD R2, R1, R0\n";         // R2 = 5
        f << "BEQ R1, R2, 2\n";          // TAKEN → skip next 2
        f << "ADD R3, R1, R1\n";         // SKIPPED
        f << "MUL R4, R1, R1\n";         // SKIPPED
        f << "SUB R5, R2, R1\n";         // R5 = 0
        f << "BEQ R5, R1, 2\n";          // NOT TAKEN
        f << "NOR R6, R5, R5\n";         // R6 = -1
        f << "ADD R7, R1, R2\n";         // R7 = 10
        f.close();

        runTest("tc6.txt", [](TomasuloSimulator& s) {
            s.getMemory().loadData(100, 5);
        });
    }

    // // === Test Case 5: BEQ condition met ===
    // {
    //     ofstream f("tc5.txt");
    //     f << "LOAD R1, 100(R0)\n";       // R1 = 5
    //     f << "ADD R2, R1, R0\n";         // R2 = 5
    //     f << "BEQ R1, R2, 2\n";          // TAKEN → skip next 2
    //     f << "ADD R3, R1, R1\n";         // SKIPPED
    //     f << "MUL R4, R1, R1\n";         // SKIPPED
    //     f << "SUB R5, R2, R1\n";         // R5 = 0
    //     f << "NOR R6, R5, R5\n";         // R6 = -1
    //     f.close();
    //     runTest("tc5.txt", [](TomasuloSimulator& s) {
    //         s.getMemory().loadData(100, 5);
    //     });
    // }

    // // === Test Case 1: Basic Arithmetic + BEQ (not taken) ===
    // {
    //     ofstream f("tc1.txt");
    //     f << "LOAD R1, 100(R0)\n";
    //     f << "LOAD R2, 104(R0)\n";
    //     f << "ADD R3, R1, R2\n";
    //     f << "SUB R4, R2, R1\n";
    //     f << "STORE R3, 108(R0)\n";
    //     f << "BEQ R4, R0, 2\n";
    //     f << "MUL R5, R3, R1\n";
    //     f << "NOR R6, R2, R1\n";
    //     f.close();
    //     runTest("tc1.txt", [](TomasuloSimulator& s) {
    //         s.getMemory().loadData(100, 5);
    //         s.getMemory().loadData(104, 10);
    //     });
    // }
    // // === Test Case 2: BEQ (taken) skipping instructions ===
    //  {
    // ofstream f("tc2.txt");
    //   f << "LOAD R1, 200(R0)\n";
    //  f << "SUB R2, R1, R1\n";
    //   f << "BEQ R2, R0, 2\n";
    //    f << "ADD R3, R1, R1\n"; // skipped
    //     f << "MUL R4, R1, R1\n"; // skipped
    //    f << "NOR R5, R1, R2\n";
    //    f << "CALL 0\n";
    //   f << "RET\n";
    //   f << "ADD R6, R0, R0\n";
    //     f.close();
    //     runTest("tc2.txt", [](TomasuloSimulator& s) {
    //      s.getMemory().loadData(200, 8);
    //  });
    // }
    // //
    // // === Test Case 3: STORE and RET round trip ===
    // {
    //     ofstream f("tc3.txt");
    //     f << "LOAD R1, 10(R0)\n";       // R1 = 42
    //     f << "ADD R2, R1, R1\n";        // R2 = 84
    //     f << "STORE R2, 20(R0)\n";      // MEM[20] = 84
    //     f << "CALL 0\n";                // R1 = PC+1
    //     f << "RET\n";                   // jump to R1 (instr 4)
    //     f << "NOR R3, R1, R2\n";        // R3 = ~(PC | 84)
    //     f.close();
    //     runTest("tc3.txt", [](TomasuloSimulator& s) {
    //         s.getMemory().loadData(10, 42);
    //     });
    // }
    //
    // // === Test Case 4: BEQ not taken, back-to-back logic ===
    // {
    //     ofstream f("tc4.txt");
    //     f << "LOAD R1, 50(R0)\n";      // R1 = 7
    //     f << "LOAD R2, 54(R0)\n";      // R2 = 4
    //     f << "SUB R3, R1, R2\n";       // R3 = 3
    //     f << "ADD R4, R3, R1\n";       // R4 = 10
    //     f << "STORE R4, 60(R0)\n";     // MEM[60] = 10
    //     f << "MUL R5, R2, R3\n";       // R5 = 12
    //     f << "BEQ R5, R3, 2\n";        // not taken
    //     f << "NOR R6, R1, R4\n";       // R6 = -16
    //     f << "ADD R7, R6, R5\n";       // R7 = -4
    //     f.close();
    //     runTest("tc4.txt", [](TomasuloSimulator& s) {
    //         s.getMemory().loadData(50, 7);
    //         s.getMemory().loadData(54, 4);
    //     });
    // }

    return 0;
}
