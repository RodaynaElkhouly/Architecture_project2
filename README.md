# Tomasulo Simulator (Non-Speculative Out-of-Order Execution)

## Project Overview
This simulator models the backend execution of a simplified 16-bit RISC processor using **Tomasulo’s algorithm without speculation**. It supports dynamic scheduling and register renaming via reservation stations and simulates only the **issue**, **execution**, and **write-back** stages.

Designed for educational purposes, the simulator processes a program written in a custom assembly-like instruction set derived from the **RiSC-16 architecture**.

---

## Developed By
- **Rodayna Elkhouly**
- **Karim Fady**

---

## Features Implemented
- **16-bit word-addressable memory** (64K locations)
- **8 General Purpose Registers (R0–R7)** with `R0 = 0`
- Instruction Support:
  - `LOAD`, `STORE`
  - `ADD`, `SUB`, `MUL`, `NOR`
  - `BEQ` (Branch if Equal) with static **not-taken predictor**
  - `CALL` and `RET`
- **Tomasulo’s Algorithm** (non-speculative):
  - Issue only based on prediction
  - Full RAW hazard tracking (`Qj`, `Qk`)
  - Reservation stations mapped 1-to-1 with functional units
- **Performance Metrics:**
  - Instruction timing table (issue, exec, writeback)
  - Total cycle count
  - IPC (Instructions per cycle)
  - Branch misprediction rate

---

## 🛠️ How It Works

### 1. **Input**
Assembly-like `.txt` program file including:
- Instructions in `LOAD R1, offset(R0)` or `ADD R3, R1, R2` format
- Memory initialized via `sim.getMemory().loadData(addr, value)`

### 2. **Execution**
The simulator:
- Loads all instructions upfront (no fetch/decode simulation)
- Runs cycle by cycle through `simulate()`
- Displays full execution timeline and final register/memory state

### 3. **Output**
On each run, the simulator prints:
- Instruction-by-instruction cycle timing
- Register file after execution
- Memory snapshot
- Total execution stats

---

## 🧪 Test Cases Included

Four comprehensive test programs are auto-generated in `main.cpp`:

| Test Case | Purpose |
|-----------|---------|
| `tc1.txt` | Arithmetic, load/store, and BEQ (not taken) |
| `tc2.txt` | BEQ (taken), control-flow correctness |
| `tc3.txt` | CALL/RET and memory write |
| `tc4.txt` | Combined flow with all operations |

Each program validates one or more aspects of pipeline behavior, hazard resolution, and functional unit timing.

---

## Functional Unit Timing

| Operation | Latency |
|-----------|---------|
| LOAD, STORE | 6 cycles (2 addr + 4 mem) |
| ADD, SUB    | 2 cycles |
| MUL         | 10 cycles |
| NOR         | 1 cycle |
| BEQ, CALL, RET | 1 cycle |

---

## File Structure
├── main.cpp # Entry point and test harness
├── simulator.cpp/h # Tomasulo algorithm core
├── instruction.h # Instruction representation
├── memory.cpp/h # Memory abstraction
├── register_file.cpp/h # Register file with renaming
├── reservation_station.h # Reservation station struct
