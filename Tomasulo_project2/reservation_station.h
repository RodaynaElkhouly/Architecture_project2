#ifndef RESERVATION_STATION_H
#define RESERVATION_STATION_H

#include "instruction.h"
#include <string>
using namespace std;

// Each reservation station holds one instruction that's waiting to execute
struct ReservationStation {
    string name;             // Unique name for this RS (e.g., "RS0", "RS1")
    bool busy = false;       // Is this RS holding an active instruction?
    OpCode op;               // The operation (ADD, SUB, etc.)
    int Vj = 0, Vk = 0;      // Actual operand values if available
    string Qj = "", Qk = ""; // Tags for who will produce the operands (empty = ready)
    int dest = -1;           // Destination register index
    int instrIndex = -1;     // Index in the instruction list
    int remainingCycles = 0; // Countdown to finish execution
};

#endif
