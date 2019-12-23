#ifndef INTERPRET_H
#define INTERPRET_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct cpuState 
{
	uint8_t memory[4096];
	uint8_t registers[16];
	uint16_t stack[16];
	uint8_t stackPointer;
	uint16_t PC;
  uint8_t DT;
  uint8_t ST;
	uint16_t VI;
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

uint8_t getUpperNibble(uint8_t inputChar);

uint8_t getLowerNibble(int8_t inputChar);

int decodeInstruction(uint8_t* instruction);
#endif
