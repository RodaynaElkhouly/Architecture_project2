#include "simulator.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

TomasuloSimulator::TomasuloSimulator()
    : cycle(0), completed(0), branches(0), mispredictions(0), programCounter(0), branchPending(false), pendingBEQIndex(-1) {
    for (int i = 0; i < 8; ++i) {
        ReservationStation rs;
        rs.name = "RS" + to_string(i);
        RS.push_back(rs);
    }
}

void TomasuloSimulator::loadProgram(const string& filename) {
    ifstream file(filename);
    string line;
    programCounter = 0;

    while (getline(file, line)) {
        stringstream ss(line);
        string op;
        ss >> op;

        Instruction inst;
        inst.rd = -1;
        inst.rs1 = -1;
        inst.rs2 = -1;
        inst.imm = 0;

        if (op == "ADD") inst.opcode = OpCode::ADD;
        else if (op == "SUB") inst.opcode = OpCode::SUB;
        else if (op == "MUL") inst.opcode = OpCode::MUL;
        else if (op == "NOR") inst.opcode = OpCode::NOR;
        else if (op == "LOAD") inst.opcode = OpCode::LOAD;
        else if (op == "STORE") inst.opcode = OpCode::STORE;
        else if (op == "BEQ") inst.opcode = OpCode::BEQ;
        else if (op == "CALL") inst.opcode = OpCode::CALL;
        else if (op == "RET") inst.opcode = OpCode::RET;
        else inst.opcode = OpCode::NOP;

        if (inst.opcode == OpCode::LOAD) {
            string rA, offsetR;
            ss >> rA >> offsetR;
            size_t start = offsetR.find("(");
            size_t end = offsetR.find(")");
            inst.rd = stoi(rA.substr(1));
            inst.rs1 = stoi(offsetR.substr(start + 2, end - start - 2));
            inst.imm = stoi(offsetR.substr(0, start));
        } else if (inst.opcode == OpCode::STORE) {
            string rA, offsetR;
            ss >> rA >> offsetR;
            size_t start = offsetR.find("(");
            size_t end = offsetR.find(")");
            inst.rs2 = stoi(rA.substr(1));
            inst.rs1 = stoi(offsetR.substr(start + 2, end - start - 2));
            inst.imm = stoi(offsetR.substr(0, start));
        } else if (inst.opcode == OpCode::BEQ) {
            string rA, rB;
            int offset;
            ss >> rA >> rB >> offset;
            inst.rs1 = stoi(rA.substr(1));
            inst.rs2 = stoi(rB.substr(1));
            inst.imm = offset;
        } else if (inst.opcode == OpCode::CALL) {
            int label;
            ss >> label;
            inst.imm = label;
        } else if (inst.opcode == OpCode::RET) {
            inst.rs1 = 1;
        } else if (inst.opcode != OpCode::NOP) {
            string rA, rB, rC;
            ss >> rA >> rB >> rC;
            inst.rd = stoi(rA.substr(1));
            inst.rs1 = stoi(rB.substr(1));
            inst.rs2 = stoi(rC.substr(1));
        }

        program.push_back(inst);
    }
}

void TomasuloSimulator::simulate() {
    int safetyCounter = 0;

    // Continue simulation until all issued instructions are completed or we reach the cap
    while (safetyCounter < 1000) {
        writeBack();
        execute();
        issue();
        cycle++;
        safetyCounter++;

        // Stop when all issued instructions have been completed
        int issuedCount = 0;
        for (const auto& inst : program) {
            if (inst.issue != -1) issuedCount++;
        }
        if (completed >= issuedCount) {
            break;
        }
    }

    if (safetyCounter >= 1000) {
        cout << "ERROR: Simulation stuck!" << endl;
        cout << "Instructions completed: " << completed << " / " << program.size() << endl;
    }

    printStats();
}

void TomasuloSimulator::writeBack() {
    completedWrites.clear();

    for (auto& rs : RS) {
        if (rs.busy && rs.remainingCycles == 0) {
            Instruction& inst = program[rs.instrIndex];
            int result = executeOp(inst.opcode, rs.Vj, rs.Vk, inst.imm, rs.instrIndex);

            if (inst.opcode != OpCode::STORE &&
                inst.opcode != OpCode::BEQ &&
                inst.opcode != OpCode::RET &&
                inst.rd >= 0) {
                if (RF.status[inst.rd] == rs.name)
                    RF.status[inst.rd] = "";
                RF.write(inst.rd, result);

                for (auto& otherRS : RS) {
                    if (!otherRS.busy) continue;
                    if (otherRS.Qj == rs.name) {
                        otherRS.Qj = "";
                        otherRS.Vj = result;
                    }
                    if (otherRS.Qk == rs.name) {
                        otherRS.Qk = "";
                        otherRS.Vk = result;
                    }
                }
            }

            if (inst.opcode == OpCode::BEQ) {
                branches++;
                bool condition = (rs.Vj == rs.Vk);
                bool prediction = false;
                if (condition != prediction) mispredictions++;

                if (condition) {
                    programCounter = rs.instrIndex + 1 + inst.imm;
                } else {
                    programCounter = rs.instrIndex + 1;
                }
                branchPending = false;
                pendingBEQIndex = -1;
            }

            inst.write = cycle;
            completed++;
            rs.busy = false;

            cout << "WRITEBACK: Instr " << rs.instrIndex
                 << " result = " << result << "\n";
        }
    }
}

void TomasuloSimulator::execute() {
    for (auto& rs : RS) {
        if (rs.busy && rs.remainingCycles > 0) {
            if (rs.Qj.empty() && rs.Qk.empty()) {
                Instruction& inst = program[rs.instrIndex];
                if (inst.start_exec == -1) inst.start_exec = cycle;
                rs.remainingCycles--;
                if (rs.remainingCycles == 0) inst.end_exec = cycle;
            }
        }
    }
}

void TomasuloSimulator::issue() {
    if (programCounter >= program.size()) return;
    if (branchPending) return;

    Instruction& inst = program[programCounter];
    if (inst.issue != -1) return;

    for (auto& rs : RS) {
        if (!rs.busy) {
            rs.busy = true;
            rs.op = inst.opcode;
            rs.dest = inst.rd;
            rs.instrIndex = programCounter;

            if (inst.opcode == OpCode::STORE) {
                rs.Vj = RF.status[inst.rs1].empty() ? RF.read(inst.rs1) : 0;
                rs.Qj = RF.status[inst.rs1];
                rs.Vk = RF.status[inst.rs2].empty() ? RF.read(inst.rs2) : 0;
                rs.Qk = RF.status[inst.rs2];
            } else {
                if (inst.rs1 >= 0) {
                    rs.Vj = RF.status[inst.rs1].empty() ? RF.read(inst.rs1) : 0;
                    rs.Qj = RF.status[inst.rs1];
                }
                if (inst.rs2 >= 0) {
                    rs.Vk = RF.status[inst.rs2].empty() ? RF.read(inst.rs2) : 0;
                    rs.Qk = RF.status[inst.rs2];
                }
            }

            rs.remainingCycles = getLatency(inst.opcode);
            inst.issue = cycle;

            if (inst.opcode != OpCode::STORE &&
                inst.opcode != OpCode::BEQ &&
                inst.opcode != OpCode::RET &&
                inst.rd >= 0) {
                RF.status[inst.rd] = rs.name;
            }

            if (inst.opcode == OpCode::BEQ) {
                branchPending = true;
                pendingBEQIndex = programCounter;
            }

            programCounter++;
            return;
        }
    }
}

int TomasuloSimulator::executeOp(OpCode op, int a, int b, int imm, int instrIndex) {
    switch (op) {
        case OpCode::ADD: return a + b;
        case OpCode::SUB: return a - b;
        case OpCode::MUL: return a * b;
        case OpCode::NOR: return ~(a | b);
        case OpCode::LOAD: return MEM.read(a + imm);
        case OpCode::STORE: MEM.write(a + imm, b); return b;
        case OpCode::CALL: RF.write(1, instrIndex + 1); return imm;
        case OpCode::RET: return RF.read(1);
        default: return 0;
    }
}

int TomasuloSimulator::getLatency(OpCode op) {
    switch (op) {
        case OpCode::ADD:
        case OpCode::SUB: return 2;
        case OpCode::MUL: return 10;
        case OpCode::NOR: return 1;
        case OpCode::LOAD:
        case OpCode::STORE: return 6;
        case OpCode::BEQ:
        case OpCode::CALL:
        case OpCode::RET: return 1;
        default: return 1;
    }
}

void TomasuloSimulator::printStats() {
    cout << "\nInstruction Stats:\n";
    for (int i = 0; i < program.size(); ++i) {
        Instruction& inst = program[i];
        cout << "Instr " << i
             << ": Issue=" << inst.issue
             << ", Exec=" << inst.start_exec << "-" << inst.end_exec
             << ", WB=" << inst.write << "\n";
    }
    cout << "Total Cycles: " << cycle << "\n";
    cout << "IPC: " << fixed << setprecision(2)
         << static_cast<float>(program.size()) / cycle << "\n";
    cout << "Branch Misprediction %: "
         << (branches ? (100.0 * mispredictions / branches) : 0.0) << "%\n";
}

Memory& TomasuloSimulator::getMemory() {
    return MEM;
}

RegisterFile& TomasuloSimulator::getRegisters() {
    return RF;
}
