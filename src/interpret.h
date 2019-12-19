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

#endif
