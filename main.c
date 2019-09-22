#include <stdio.h>
#include <stdint.h>

typedef struct Chip8State
{
	uint8_t memory[4096];
	uint8_t registers[16];
	uint16_t VI;
	uint16_t stack[16];
	uint16_t PC;
	uint8_t stackPointer;
} state;

typedef void (*jumpTable)(uint8_t* instruction, state cpu);

uint8_t getUpperNibble(uint8_t inputChar)
{
	inputChar >>= 4;

	return inputChar;
}

uint8_t getLowerNibble(int8_t inputChar)
{
  inputChar &= 0x0F;

	return inputChar;
}


int decodeInstruction(uint8_t* instruction)
{
	uint8_t nibbles[4];
	nibbles[0] = getUpperNibble(instruction[0]);
	nibbles[1] = getLowerNibble(instruction[0]);
	nibbles[2] = getUpperNibble(instruction[1]);
	nibbles[3] = getLowerNibble(instruction[1]);

//	printf("For instruction %X %X:\n", instruction[0],instruction[1]);
//	printf("Nibbles[0] = %X.\n", nibbles[0]);

	if(instruction[0] == 0x00 &&  instruction[1] == 0xE0)
	{
		return 1;

	} else if (instruction[0] == 0x00 && instruction[1] == 0xEE)
	{
		return 2;

	} else if (nibbles[0] < 0x08)
	{
		return (2 + nibbles[0]); // 2-9

	} else if (nibbles[0] == 0x08)
	{
    printf("Nibbles 3 = %X\n",nibbles[3]);

		if(nibbles[3] == 0x0E)
		{
			return 18;
		} else
		{ 
			return (10 + nibbles[3]); // 10-18
		}

	} else if (nibbles[0] < 0x0E)
	{
		return (19 + nibbles[0] - 0x09); // 19-23

	} else if (nibbles[0] < 0x0F) // 24-25
	{
		if(nibbles[2] == 0x09 && nibbles[3] == 0x0E)
		{
			return 24;

		} else if(nibbles[2] == 0x0A && nibbles[3] == 0x01)
		{
			return 25;

		}
	} else if (nibbles[0] == 0x0F) // 26-33
	{
		if(instruction[1] == 0x07)
		{
			return 26;
		} else if (instruction[1] == 0x0A)
		{
			return 27;
		} else if (instruction[1] == 0x15)
		{
			return 28;
		} else if (instruction[1] == 0x18)
		{
			return 29;
		} else if (instruction[1] == 0x1E)
		{
			return 30;
		} else if (instruction[1] == 0x29)
		{
			return 31;
		} else if (instruction[1] == 0x33)
		{
			return 32;
		} else if (instruction[1] == 0x55)
		{
			return 33;
		} else if (instruction[1] == 0x65)
		{
			return 34;
		}
	}

	return 0;


}

void testDecoder()
{
	uint8_t testArray[68] = 
	{0x00,0xE0,
	0x00,0xEE,
	0x10,0x00,
	0x20,0x00,
	0x30,0x00,
	0x40,0x00,
	0x50,0x00,
	0x60,0x00,
	0x70,0x00,
	0x80,0x00,
	0x80,0x01,
	0x80,0x02,
	0x80,0x03,
	0x80,0x04,
	0x80,0x05,
	0x80,0x06,
	0x80,0x07,
	0x80,0x0E,
	0x90,0x00,
	0xA0,0x00,
	0xB0,0x00,
	0xC0,0x00,
	0xD0,0x00,
	0xE0,0x9E,
	0xE0,0xA1,
	0xF0,0x07,
	0xF0,0x0A,
	0xF0,0x15,
	0xF0,0x18,
	0xF0,0x1E,
	0xF0,0x29,
	0xF0,0x33,
	0xF0,0x55,
	0xF0,0x65
	};

	int decodedValue = 0;

	for(int i = 0; i < 68; i += 2)
	{
		decodedValue = decodeInstruction(&testArray[i]);
		printf("%d\n",decodedValue);
	}

	return;
}

uint16_t decode12bitAddr(uint8_t* instruction)
{
  uint16_t address = *instruction; 
  address += (instruction[0] & 0x0F) << 8;
  address += instruction[1];
  return address;
}

int detectOverflow(uint8_t a, uint8_t b)
{
  if(a > 255 - b)
  {
    return 1;
  } else
  {
    return 0;
  }
}

int detectUnderflow(uint8_t a, uint8_t b)
{
  if(a < b)
  {
    return 0;
  } else
  {
    return 1;
  }
}
void clearScreen(uint8_t* instruction, state cpu)
{}

void returnFromSub(uint8_t* instruction, state cpu) 
{
	cpu.PC = cpu.stack[cpu.stackPointer-1];
	cpu.stackPointer --;
}

void jumpToAddress(uint8_t* instruction, state cpu)
{
  uint16_t address = decode12bitAddr(instruction); 
  cpu.PC = address;
}

void executeSubroutine(uint8_t* instruction, state cpu)
{
  uint16_t address = decode12bitAddr(instruction);
  cpu.stack[cpu.stackPointer] = address;
  cpu.stackPointer++;
}

void skipEq(uint8_t* instruction, state cpu)
{
  uint8_t value = instruction[1];
  uint8_t reg = (instruction[0] & 0x0F);
  if(value == cpu.registers[reg])
  {
    cpu.PC++;
  }
}

void skipNeq(uint8_t* instruction, state cpu)
{
  uint8_t value = instruction[1];
  uint8_t reg = (instruction[0] * 0x0F);
  if(value != cpu.registers[reg])
  {
    cpu.PC++;
  }
}

void skipCmp(uint8_t* instruction, state cpu)
{
  uint8_t reg1 = getLowerNibble(instruction[0]);
  uint8_t reg2 = getUpperNibble(instruction[1]);
  if(cpu.registers[reg1] == cpu.registers[reg2])
  {
    cpu.PC++;
  }
}

void storeImmediate(uint8_t* instruction, state cpu)
{
  int reg = getLowerNibble(instruction[0]);
  cpu.registers[reg] = instruction[1];
}

void addImmediate(uint8_t* instruction, state cpu)
{
  int reg = getLowerNibble(instruction[0]);
  cpu.registers[reg] = instruction[1];
}

void storeReg(uint8_t* instruction, state cpu)
{
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getUpperNibble(instruction[1]);
  cpu.registers[reg1] = cpu.registers[reg2];
}

void regOR(uint8_t* instruction, state cpu)
{ 
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getUpperNibble(instruction[1]);
  cpu.registers[reg1] |= cpu.registers[reg2];
}

void regAND(uint8_t* instruction, state cpu)
{
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getUpperNibble(instruction[1]);
  cpu.registers[reg1] &= cpu.registers[reg2];
}

void regXOR(uint8_t* instruction, state cpu)
{
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getUpperNibble(instruction[1]);
}

void addReg(uint8_t* instruction, state cpu)
{
  int overflowFlag;
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getUpperNibble(instruction[1]);

  overflowFlag = detectOverflow(cpu.registers[reg1], cpu.registers[reg2]);
  cpu.registers[reg1] += cpu.registers[reg2];
  cpu.registers[15] = overflowFlag;
}

void subReg(uint8_t* instruction, state cpu)
{
  int underflowFlag;
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getUpperNibble(instruction[1]);

  underflowFlag = detectUnderflow(cpu.registers[reg1], cpu.registers[reg2]);
  cpu.registers[reg1] -= cpu.registers[reg2];
  cpu.registers[15] = underflowFlag;
}

void shiftRight(uint8_t* instruction, state cpu)
{
  int lsb;
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getUpperNibble(instruction[1]);
  lsb = cpu.registers[reg2] & 0x01; 
  cpu.registers[reg1] = cpu.registers[reg2] >> 1;
}

void subRegRev(uint8_t* instruction, state cpu)
{
  int underflowFlag;
  reg1 = getLowerNibble(instruction[0]);
  reg2 = getLowerNibble(instruction[1]);
  cpu.registers[15] detectUnderflow(cpu.registers[reg2], cpu.registers[reg1]);
  cpu.registers[reg1] = cpu.registers[reg2] - cpu.registers[reg1];
}
void skipRegEq(uint8_t* instruction, state cpu)
{
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getLowerNibble(instruction[1]);
  if(cpu.registers[reg1] == cpu.registers[reg2])
  {
    cpu.PC++;
  }
}

void shiftLeft(uint8_t* instruction, state cpu)
{ 
  int lsb;
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getUpperNibble(instruction[1]);
  lsb = cpu.registers[reg2] & 0x80; 
  cpu.registers[reg1] = cpu.registers[reg2] << 1;
}

void storeAddr(uint8_t* instruction, state cpu)
{
  uint16_t address = decode12bitAddr(instruction);
  cpu.VI = address;
}

void jmpOffset(uint8_t* instruction, state cpu)
{
  uint16_t offset = decode12bitAddr(instruction);
  cpu.PC = cpu.registers[0] + offset;
}

void rand(uint8_t* instruction, state cpu)
{
  uint8_t rand8Bit = (rand() % 256) & instruction[1];
  int reg = getLowerNibble(instruction[0]);
  cpu.registers[reg] = rand8Bit;
}
int main()
{
	state CPUState;
  srand(time(NULL));
  uint8_t testInstruction[2] = {0x12,0xFF};
	jumpTable instructionHandler[34] = {clearScreen, returnFromSub, jumpToAddress, executeSubroutine, skipEq, skipNeq, skipCmp, storeImmediate, addImmediate, storeReg, regOR, regAND, regXOR, addReg, subReg, shiftRight, subRegRev, shiftLeft, skipRegEq, storeAddr, jmpOffset, rand};
  instructionHandler[2](testInstruction,CPUState);

	//testDecoder();
	return 0;
}
