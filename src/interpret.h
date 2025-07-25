#ifndef INTERPRET_H
#define INTERPRET_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

#define MILLION 1000000
#define BILLION 1000000000

#define MAX_PROGRAM_SIZE ( PROGRAM_MEMORY_END - PROGRAM_MEMORY_START )
#define PROGRAM_MEMORY_START 0x200
#define PROGRAM_MEMORY_END 0x1000
#define FONT_DATA_START 0x000
#define FONT_DATA_END 0x050

typedef struct cpuState 
{
    uint8_t memory[4096];
    uint8_t registers[16];
    uint16_t stack[16];
    uint8_t screen[64][32];
    uint8_t stackPointer;
    uint16_t PC;
    uint8_t DT;
    uint8_t ST;
    uint16_t VI;
    unsigned int frequency;
    struct timespec prevTime;
    bool paused;
    int shiftMethod;
    char* currentFilePath;
    int error;
} Chip8State;

enum iType{EXEC_ASM, CLS, RETURN, JMP,
  EXEC_SUB, SKIP_EQ_IMM, SKIP_NEQ_IMM, SKIP_EQ,
  STORE_IMM, ADD_IMM, STORE, OR,
  AND, XOR, ADD, SUBXY,
  SHIFT_RIGHT, SUBYX, SHIFT_LEFT, SKIP_NEQ,
  LOAD_I_IMM, JMPI_OFFSET, RAND, DRAW_SPRITE,
  SKIP_IFKEY, SKIP_NOTKEY, STORE_DELAY, WAIT_FOR_KEY,
  SET_DELAY, SET_SOUND_TIMER, ADD_VX_I, SET_I_SPRITE,
  STORE_BCD, STORE_REGISTER_RANGE, LOAD_REGISTER_RANGE,
  INVALID_OP=-1
};

enum errorType{
    OK,
    BOUND_ERROR,
    ASM_NOT_SUPPORTED,
    INVALID_REG
};


uint8_t getUpperNibble(uint8_t inputChar);

uint8_t getLowerNibble(int8_t inputChar);

int decodeInstruction(uint8_t* instruction);

Chip8State* createDefaultState();

void initialize(Chip8State* cpu);

int run(Chip8State *cpu);

void initialize(Chip8State* cpu);

void setDebug(bool value);

void clearNumpad();

int loadProgram(Chip8State* cpu, char* path);

void reload(Chip8State* cpu);

void setNumpadKey(int keyIndex);

int timediff(struct timespec *result, const struct timespec *x, const struct timespec *y);
#endif
