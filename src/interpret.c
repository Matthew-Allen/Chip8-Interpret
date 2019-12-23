#include "interpret.h"

#define DEBUG_ENABLED true

void printDebug(char* debugString)
{
  if(DEBUG_ENABLED)
  {
    printf("%s",debugString);
  }
}

typedef void (*jumpTable)(uint8_t* instruction, Chip8State cpu);

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

  switch(nibbles[0])
  {
    case 0x00:
      if(instruction[1] == 0xE0)
      {
        return CLS;
      } else if (instruction[1] == 0xEE)
      {
        return RETURN;
      } else
      {
        return EXEC_ASM;
      }
   case 0x01:
      return JMP;
      break;
   case 0x02:
      return EXEC_SUB;
      break;
   case 0x03:
      return SKIP_EQ_IMM;
      break;
   case 0x04:
      return SKIP_NEQ_IMM;
      break;
   case 0x05:
      return SKIP_EQ;
      break;
   case 0x06:
      return STORE_IMM;
      break;
   case 0x07:
      return ADD_IMM;
      break;
   case 0x08:
      switch(nibbles[3])
      {
        case 0x00:
          return STORE;
          break;
        case 0x01:
          return OR;
          break;
        case 0x02:
          return AND;
          break;
        case 0x03:
          return XOR;
          break;
        case 0x04:
          return ADD;
          break;
        case 0x05:
          return SUBXY;
          break;
        case 0x06:
          return SHIFT_RIGHT;
          break;
        case 0x07:
          return SUBYX;
          break;
        case 0x0E:
          return SHIFT_LEFT;
          break;
      }
   case 0x09:
      return SKIP_NEQ;
      break;
   case 0x0A:
      return LOAD_I_IMM;
      break;
   case 0x0B:
      return JMPI_OFFSET;
      break;
   case 0x0C:
      return RAND;
      break;
   case 0x0D:
      return DRAW_SPRITE;
      break;
   case 0x0E:

      switch(instruction[1])
      {
        case 0x9E:
          return SKIP_IFKEY;
          break;
      }
   case 0x0F:
      
      switch(instruction[1])
      {
        case 0xA1:
          return SKIP_NOTKEY;
          break;
        case 0x07:
          return STORE_DELAY;
          break;
        case 0x0A:
          return WAIT_FOR_KEY;
          break;
        case 0x15:
          return SET_DELAY;
          break;
        case 0x18:
          return SET_SOUND_TIMER;
          break;
        case 0x1E:
          return ADD_VX_I;
          break;
        case 0x29:
          return SET_I_SPRITE;
          break;
        case 0x33:
          return STORE_BCD;
          break;
        case 0x55:
          return STORE_REGISTER_RANGE;
          break;
        case 0x65:
          return LOAD_REGISTER_RANGE;
          break;
      }
  }

   return INVALID_OP;
}

void testDecoder()
{

	int decodedValue = 0;
  uint8_t instruction[2]; 
  
  for(int i = 0; i < (256*256); i++)
  {
    instruction[0] = ((i & 0xFF00) >> 4);
    instruction[1] = (i & 0x00FF);
    decodedValue = decodeInstruction(instruction);
    
    printf("%d",decodedValue);
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

void executeASM() // This function intentionally not implemented.
{}

void clearScreen(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Clear screen function called.\n");
}

void returnFromSub(uint8_t* instruction, Chip8State cpu) 
{
//  printDebug("Return from subroutine function called.\n");
	cpu.PC = cpu.stack[cpu.stackPointer-1];
	cpu.stackPointer --;
}

void jumpToAddress(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Jump to address function called.\n");
  uint16_t address = decode12bitAddr(instruction); 
  cpu.PC = address;
}

void executeSubroutine(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Execute subroutine function called.\n");
  uint16_t address = decode12bitAddr(instruction);
  cpu.stack[cpu.stackPointer] = address;
  cpu.stackPointer++;
}

void skipEq(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Skip if equal function called.\n");
  uint8_t value = instruction[1];
  uint8_t reg = (instruction[0] & 0x0F);
  if(value == cpu.registers[reg])
  {
    cpu.PC++;
  }
}

void skipNeq(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Skip if not equal function called.\n");
  uint8_t value = instruction[1];
  uint8_t reg = (instruction[0] * 0x0F);
  if(value != cpu.registers[reg])
  {
    cpu.PC++;
  }
}

void skipCmp(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Skip if register comparison is equal function called.\n");
  uint8_t reg1 = getLowerNibble(instruction[0]);
  uint8_t reg2 = getUpperNibble(instruction[1]);
  if(cpu.registers[reg1] == cpu.registers[reg2])
  {
    cpu.PC++;
  }
}

void storeImmediate(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Store immediate function called.\n");
  int reg = getLowerNibble(instruction[0]);
  cpu.registers[reg] = instruction[1];
}

void addImmediate(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Add immediate function called.\n");
  int reg = getLowerNibble(instruction[0]);
  cpu.registers[reg] = instruction[1];
}

void storeReg(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Store Register function called.\n");
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getUpperNibble(instruction[1]);
  cpu.registers[reg1] = cpu.registers[reg2];
}

void regOR(uint8_t* instruction, Chip8State cpu)
{ 
  printDebug("Register OR function called.\n");
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getUpperNibble(instruction[1]);
  cpu.registers[reg1] |= cpu.registers[reg2];
}

void regAND(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Register AND function called.\n");
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getUpperNibble(instruction[1]);
  cpu.registers[reg1] &= cpu.registers[reg2];
}

void regXOR(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Register XOR function called.\n");
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getUpperNibble(instruction[1]);
}

void addReg(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Add register function called.\n");
  int overflowFlag;
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getUpperNibble(instruction[1]);

  overflowFlag = detectOverflow(cpu.registers[reg1], cpu.registers[reg2]);
  cpu.registers[reg1] += cpu.registers[reg2];
  cpu.registers[15] = overflowFlag;
}

void subReg(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Subtract register function called.\n");
  int underflowFlag;
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getUpperNibble(instruction[1]);

  underflowFlag = detectUnderflow(cpu.registers[reg1], cpu.registers[reg2]);
  cpu.registers[reg1] -= cpu.registers[reg2];
  cpu.registers[15] = underflowFlag;
}

void shiftRight(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Shift register right function called.\n");
  int lsb;
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getUpperNibble(instruction[1]);
  lsb = cpu.registers[reg2] & 0x01; 
  cpu.registers[reg1] = cpu.registers[reg2] >> 1;
}

void subRegRev(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Sub registers backwards function called.\n");
  int underflowFlag;
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getLowerNibble(instruction[1]);
  cpu.registers[15] = detectUnderflow(cpu.registers[reg2], cpu.registers[reg1]);
  cpu.registers[reg1] = cpu.registers[reg2] - cpu.registers[reg1];
}
void skipRegEq(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Print if registers are equal function called.\n");
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getLowerNibble(instruction[1]);
  if(cpu.registers[reg1] == cpu.registers[reg2])
  {
    cpu.PC++;
  }
}

void shiftLeft(uint8_t* instruction, Chip8State cpu)
{ 
  printDebug("Shift left function called.\n");
  int lsb;
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getUpperNibble(instruction[1]);
  lsb = cpu.registers[reg2] & 0x80; 
  cpu.registers[reg1] = cpu.registers[reg2] << 1;
}

void storeAddr(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Store address function called.\n");
  uint16_t address = decode12bitAddr(instruction);
  cpu.VI = address;
}

void jmpOffset(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Jump offset function called.\n");
  uint16_t offset = decode12bitAddr(instruction);
  cpu.PC = cpu.registers[0] + offset;
}

void storeRand(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Store rand function called.\n");
  uint8_t rand8Bit = (rand() % 256) & instruction[1];
  int reg = getLowerNibble(instruction[0]);
  cpu.registers[reg] = rand8Bit;
}

void drawSprite(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Draw sprite function called.\n");
}

void skipIfKey(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Skip if key function called.\n");
}

void skipIfNotKey(uint8_t* instruction, Chip8State cpu) // Not yet complete.
{
  printDebug("Skip if not key function called.\n");
  cpu.PC++;
}

void storeDT(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Store DT function called.\n");
  int reg = getLowerNibble(instruction[0]);
  cpu.registers[reg] = cpu.DT;
}

void setDT(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Set DT function called.\n");
  int reg = getLowerNibble(instruction[0]);
  cpu.DT = cpu.registers[reg];
}

void setST(uint8_t* instrucstion, Chip8State cpu)
{
  printDebug("Set ST function called.\n");
}

void addRegI(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Add register I function called.\n");
  int reg = getLowerNibble(instruction[0]);
  cpu.VI = cpu.VI + cpu.registers[reg];
}


void setISpriteAddr(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Set I Sprite Addr function called.\n");
}

void storeBCD(uint8_t* instrucstion, Chip8State cpu)
{
  printDebug("Store BCD function called.\n");
}

void storeRegs(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Store registers function called.\n");
  int VX = getLowerNibble(instruction[0]); 
  for(int i = 0; i < VX; i++)
  {
    cpu.memory[cpu.VI + i] = cpu.registers[i];
  }
}

void loadRegs(uint8_t* instruction, Chip8State cpu)
{
  printDebug("Load registers function called.\n");
  int VX = getLowerNibble(instruction[0]);
  for(int i = 0; i < VX; i++)
  {
    cpu.registers[i] = cpu.memory[cpu.VI + i];
  }
}

