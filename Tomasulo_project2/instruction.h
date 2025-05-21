#ifndef INSTRUCTION_H
#define INSTRUCTION_H

enum class OpCode {
    ADD, SUB, MUL, NOR,
    LOAD, STORE,
    BEQ, CALL, RET,
    NOP
};

struct Instruction {
    OpCode opcode;
    int rd = -1;
    int rs1 = -1;
    int rs2 = -1;
    int imm = 0;
    int issue = -1;
    int start_exec = -1;
    int end_exec = -1;
    int write = -1;
};

#endif
