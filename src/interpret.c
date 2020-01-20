#include "interpret.h"


typedef void (*jumpTable)(uint8_t* instruction, Chip8State *cpu);


void initialize(Chip8State* cpu)
{

  //Load reserved data
  //Array containing sprite data for built-in hexadecimal font 
  uint8_t digits[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80,}; // F

  for(int i = 0; i < 80; i++) // Load font sprite data to system memory
  {
    cpu->memory[i] = digits[i];
  }
  
  for(int i = 0; i < 8; i++)
  {
    for(int j = 0; j < 4; j++)
    {
      cpu->screen[i][j] = 0x00;
    }
  }
  
  for(int i = 0; i < 16; i++) // Zero out registers and stack
  {
    cpu->registers[i] = 0x00;
    cpu->stack[i] = 0x00;
  }
  
  cpu->stackPointer = 0x00;
  cpu->PC = 0x200;
  cpu->DT = 0x00;
  cpu->ST = 0x00;
  cpu->VI = 0x00;

  return;
}

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
      switch(instruction[1])
      {
        case 0xE0:
          return CLS;
          break;
        case 0xEE:
          return RETURN;
          break;
        default:
          return EXEC_ASM;
          break;
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
        case 0xA1:
          return SKIP_NOTKEY;
          break;
      }
   case 0x0F:
      
      switch(instruction[1])
      {
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

void clearScreen(uint8_t* instruction, Chip8State *cpu)
{
  for(int i = 0; i < 64; i++)
  {
    for(int j = 0; j < 32; j++)
    {
      cpu->screen[i][j] = 0;
    }
  }
}

void returnFromSub(uint8_t* instruction, Chip8State *cpu) 
{
	cpu->PC = cpu->stack[cpu->stackPointer-1];
	cpu->stackPointer --;
}

void jumpToAddress(uint8_t* instruction, Chip8State *cpu)
{
  uint16_t address = decode12bitAddr(instruction); 
  cpu->PC = address;
}

void executeSubroutine(uint8_t* instruction, Chip8State *cpu)
{
  uint16_t address = decode12bitAddr(instruction);
  cpu->stack[cpu->stackPointer] = address;
  cpu->stackPointer++;
}

void skipEqImm(uint8_t* instruction, Chip8State *cpu)
{
  uint8_t value = instruction[1];
  uint8_t reg = (instruction[0] & 0x0F);
  if(value == cpu->registers[reg])
  {
    cpu->PC++;
  }
}

void skipNeqImm(uint8_t* instruction, Chip8State *cpu)
{
  uint8_t value = instruction[1];
  uint8_t reg = (instruction[0] * 0x0F);
  if(value != cpu->registers[reg])
  {
    cpu->PC++;
  }
}

void skipEq(uint8_t* instruction, Chip8State *cpu)
{
  uint8_t reg1 = getLowerNibble(instruction[0]);
  uint8_t reg2 = getUpperNibble(instruction[1]);
  if(cpu->registers[reg1] == cpu->registers[reg2])
  {
    cpu->PC++;
  }
}

void storeImmediate(uint8_t* instruction, Chip8State *cpu)
{
  int reg = getLowerNibble(instruction[0]);
  cpu->registers[reg] = instruction[1];
}

void addImmediate(uint8_t* instruction, Chip8State *cpu)
{
  int reg = getLowerNibble(instruction[0]);
  cpu->registers[reg] = instruction[1];
}

void store(uint8_t* instruction, Chip8State *cpu)
{
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getUpperNibble(instruction[1]);
  cpu->registers[reg1] = cpu->registers[reg2];
}

void or(uint8_t* instruction, Chip8State *cpu)
{ 
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getUpperNibble(instruction[1]);
  cpu->registers[reg1] |= cpu->registers[reg2];
}

void and(uint8_t* instruction, Chip8State *cpu)
{
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getUpperNibble(instruction[1]);
  cpu->registers[reg1] &= cpu->registers[reg2];
}

void xor(uint8_t* instruction, Chip8State *cpu)
{
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getUpperNibble(instruction[1]);
}

void add(uint8_t* instruction, Chip8State *cpu)
{
  int overflowFlag;
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getUpperNibble(instruction[1]);

  overflowFlag = detectOverflow(cpu->registers[reg1], cpu->registers[reg2]);
  cpu->registers[reg1] += cpu->registers[reg2];
  cpu->registers[15] = overflowFlag;
}

void subXY(uint8_t* instruction, Chip8State *cpu)
{
  int underflowFlag;
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getUpperNibble(instruction[1]);

  underflowFlag = detectUnderflow(cpu->registers[reg1], cpu->registers[reg2]);
  cpu->registers[reg1] -= cpu->registers[reg2];
  cpu->registers[15] = underflowFlag;
}

void shiftRight(uint8_t* instruction, Chip8State *cpu)
{
  int lsb;
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getUpperNibble(instruction[1]);
  lsb = cpu->registers[reg2] & 0x01; 
  cpu->registers[reg1] = cpu->registers[reg2] >> 1;
}

void subYX(uint8_t* instruction, Chip8State *cpu)
{
  int underflowFlag;
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getLowerNibble(instruction[1]);
  cpu->registers[15] = detectUnderflow(cpu->registers[reg2], cpu->registers[reg1]);
  cpu->registers[reg1] = cpu->registers[reg2] - cpu->registers[reg1];
}
void skipNeq(uint8_t* instruction, Chip8State *cpu)
{
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getLowerNibble(instruction[1]);
  if(cpu->registers[reg1] != cpu->registers[reg2])
  {
    cpu->PC++;
  }
}

void shiftLeft(uint8_t* instruction, Chip8State *cpu)
{ 
  int lsb;
  int reg1 = getLowerNibble(instruction[0]);
  int reg2 = getUpperNibble(instruction[1]);
  lsb = cpu->registers[reg2] & 0x80; 
  cpu->registers[reg1] = cpu->registers[reg2] << 1;
}

void loadI_Imm(uint8_t* instruction, Chip8State *cpu)
{
  uint16_t address = decode12bitAddr(instruction);
  cpu->VI = address;
}

void jmpIOffset(uint8_t* instruction, Chip8State *cpu)
{
  uint16_t offset = decode12bitAddr(instruction);
  cpu->PC = cpu->registers[0] + offset;
}

void setRand(uint8_t* instruction, Chip8State *cpu)
{
  uint8_t rand8Bit = (rand() % 256) & instruction[1];
  int reg = getLowerNibble(instruction[0]);
  cpu->registers[reg] = rand8Bit;
}

void drawSprite(uint8_t* instruction, Chip8State *cpu)
{
  uint16_t spriteAddr = cpu->VI;
  uint8_t xCoord = getLowerNibble(instruction[0]) % 64;
  uint8_t yCoord = getUpperNibble(instruction[1]) % 32;
  int numBytes = getLowerNibble(instruction[1]);
  uint8_t spritePixels[8];

  cpu->registers[15] = 0x00;

  for(int i = 0; i < numBytes; i++)
  {
    uint8_t spriteByte = cpu->memory[spriteAddr + i];
    uint8_t currentBit;
    for(int j = 7; i >= 0; j--)
    {
      currentBit = spriteByte & 0x01;
      if(currentBit == 1)
      {
        spritePixels[j] = 1;
      } else
      {
        spritePixels[j] = 0;
      }
      currentBit >>= 1;
    }

    for(int j = 0; j < 8; j++)
    {
      int x = xCoord + j;
      int y = yCoord + i;
      
      if(x > 0x3F)
      {
        continue;
      }
      if(y > 0x1F)
      {
        continue;
      }

      if(spritePixels[j] == 1)
      {
        if(cpu->screen[x][y] == 1)
        {
          cpu->screen[x][y] = 0;
          cpu->registers[15] = 0x01;
        } else
        {
          cpu->screen[x][y] = 1;
        }
      }
    }
  }
}

void skipIfKey(uint8_t* instruction, Chip8State *cpu)
{
 // TODO: Implement
}

void skipIfNotKey(uint8_t* instruction, Chip8State *cpu) 
{
  cpu->PC++; // TODO: Finish implementing
}

void storeDT(uint8_t* instruction, Chip8State *cpu)
{
  int reg = getLowerNibble(instruction[0]);
  cpu->registers[reg] = cpu->DT;
}

void waitKey(uint8_t* instruction, Chip8State *cpu)
{
  // TODO: Implement
}

void setDT(uint8_t* instruction, Chip8State *cpu)
{
  int reg = getLowerNibble(instruction[0]);
  cpu->DT = cpu->registers[reg];
}

void setST(uint8_t* instrucstion, Chip8State *cpu)
{
  // TODO: Implement
}

void addI(uint8_t* instruction, Chip8State *cpu)
{
  int reg = getLowerNibble(instruction[0]);
  cpu->VI = cpu->VI + cpu->registers[reg];
}


void setISpriteAddr(uint8_t* instruction, Chip8State *cpu)
{
  // TODO: Implement
}

void storeBCD(uint8_t* instrucstion, Chip8State *cpu)
{
  // TODO: Implement
}

void storeRegs(uint8_t* instruction, Chip8State *cpu)
{
  int VX = getLowerNibble(instruction[0]); 
  for(int i = 0; i < VX; i++)
  {
    cpu->memory[cpu->VI + i] = cpu->registers[i];
  }
}

void loadRegs(uint8_t* instruction, Chip8State *cpu)
{
  int VX = getLowerNibble(instruction[0]);
  for(int i = 0; i < VX; i++)
  {
    cpu->registers[i] = cpu->memory[cpu->VI + i];
  }
}

int run(Chip8State *cpu)
{
  static const jumpTable opFuncs[35] = {
    executeASM, clearScreen, returnFromSub, jumpToAddress,
    executeSubroutine, skipEqImm, skipNeqImm, skipEq,
    storeImmediate, addImmediate, store, or,
    and, xor, add, subXY,
    shiftRight, subYX, shiftLeft, skipNeq,
    loadI_Imm, jmpIOffset, setRand, drawSprite,
    skipIfKey, skipIfNotKey, storeDT, waitKey,
    setDT, setST, addI, setISpriteAddr,
    storeBCD, storeRegs, loadRegs};


  initialize(cpu);
  bool running = true;

  while(running)
  {
    uint8_t* nextInstruction = &cpu->memory[cpu->PC];
    opFuncs[decodeInstruction(nextInstruction)](nextInstruction, cpu);
    cpu->PC += 2;
  }

}
