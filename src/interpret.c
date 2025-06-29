#include "interpret.h"

typedef int (*jumpTable)(uint8_t* instruction, Chip8State *cpu);

static bool numpad[16];

int timediff(struct timespec *result, const struct timespec *x, const struct timespec *y)
{
    if(x->tv_nsec < y->tv_nsec)
    {
        result->tv_nsec = (x->tv_nsec + BILLION) - y->tv_nsec;
        result->tv_sec = (x->tv_sec - 1) - y->tv_sec;
    } else
    {
        result->tv_nsec = x->tv_nsec - y->tv_nsec;
        result->tv_sec = x->tv_sec - y->tv_sec;
    }

    if(x->tv_sec < y->tv_sec)
    {
        return 1;
    } else
    {
        return 0;
    }
}

void clearNumpad()
{
    for(int i = 0; i < 16; i++)
    {
        numpad[i] = false;
    }
}

void setNumpadKey(int keyIndex)
{
    numpad[keyIndex] = true;
}

Chip8State* createDefaultState()
{
    Chip8State* state = malloc(sizeof(Chip8State));
    initialize(state);
    return state;
}

void initialize(Chip8State* cpu)
{
    //Set default metadata values
    cpu->shiftMethod = 0; // Default to store in VX
    cpu->paused = false;
    //Load reserved-memory data

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

    for(int i = 0; i < 64; i++) // Zero out screen;
    {
        for(int j = 0; j < 32; j++)
        {
            cpu->screen[i][j] = 0;
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

    cpu->currentFilePath = NULL;

    return;
}

int loadProgram(Chip8State* cpu, char* path)
{
    FILE* programFile = fopen(path, "r");
    if(cpu->currentFilePath != NULL && strcmp(path, cpu->currentFilePath) != 0)
    {
        free(cpu->currentFilePath);
    }

    cpu->currentFilePath = (char*)malloc(strlen(path)+1);
    strcpy(cpu->currentFilePath, path);

    if(programFile == NULL)
    {
        printf("Error opening file %s\n", path);
        return -1;
    }

    int bytesRead = fread((cpu->memory + 0x200), 1, MAX_PROGRAM_SIZE + 1, programFile);
    printf("Finished reading file \"%s\"\n%d bytes loaded.\n", path, bytesRead);

    if(bytesRead > MAX_PROGRAM_SIZE)
    {
        printf("Warning: Read stopped before reaching EOF (File size too large).\n");
        return -1;
    }
    return 0;
}

void reload(Chip8State* cpu)
{
    char* tempString = (char*)malloc(strlen(cpu->currentFilePath)+1);
    strcpy(tempString, cpu->currentFilePath);
    initialize(cpu);
    loadProgram(cpu, tempString);
    free(tempString);
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
    uint16_t address = 0;
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

int executeASM(uint8_t* instruction, Chip8State *cpu) // This function intentionally not implemented.
{
    return ASM_NOT_SUPPORTED;
}

int clearScreen(uint8_t* instruction, Chip8State *cpu)
{
    for(int i = 0; i < 64; i++)
    {
        for(int j = 0; j < 32; j++)
        {
            cpu->screen[i][j] = 0;
        }
    }
    return OK;
}

int returnFromSub(uint8_t* instruction, Chip8State *cpu) 
{
    cpu->PC = cpu->stack[cpu->stackPointer-1];
    cpu->stackPointer --;
    return OK;
}

int jumpToAddress(uint8_t* instruction, Chip8State *cpu)
{
    uint16_t address = decode12bitAddr(instruction); 
    cpu->PC = address-2;
    return OK;
}

int executeSubroutine(uint8_t* instruction, Chip8State *cpu)
{
    uint16_t address = decode12bitAddr(instruction);
    cpu->stack[cpu->stackPointer] = cpu->PC;
    cpu->stackPointer++;
    cpu->PC = address - 2;
    return OK;
}

int skipEqImm(uint8_t* instruction, Chip8State *cpu)
{
    uint8_t value = instruction[1];
    uint8_t reg = getLowerNibble(instruction[0]);
    if(value == cpu->registers[reg])
    {
        cpu->PC += 2;
    }
    return OK;
}

int skipNeqImm(uint8_t* instruction, Chip8State *cpu)
{
    uint8_t value = instruction[1];
    uint8_t reg = (instruction[0] & 0x0F);
    if(value != cpu->registers[reg])
    {
        cpu->PC += 2;
    }
    return OK;
}

int skipEq(uint8_t* instruction, Chip8State *cpu)
{
    uint8_t reg1 = getLowerNibble(instruction[0]);
    uint8_t reg2 = getUpperNibble(instruction[1]);
    if(cpu->registers[reg1] == cpu->registers[reg2])
    {
        cpu->PC += 2;
    }
    return OK;
}

int storeImmediate(uint8_t* instruction, Chip8State *cpu)
{
    int reg = getLowerNibble(instruction[0]);
    cpu->registers[reg] = instruction[1];
    return OK;
}

int addImmediate(uint8_t* instruction, Chip8State *cpu)
{
    int reg = getLowerNibble(instruction[0]);
    cpu->registers[reg] += instruction[1];
    return OK;
}

int store(uint8_t* instruction, Chip8State *cpu)
{
    int reg1 = getLowerNibble(instruction[0]);
    int reg2 = getUpperNibble(instruction[1]);
    cpu->registers[reg1] = cpu->registers[reg2];
    return OK;
}

int or(uint8_t* instruction, Chip8State *cpu)
{ 
    int reg1 = getLowerNibble(instruction[0]);
    int reg2 = getUpperNibble(instruction[1]);
    cpu->registers[reg1] |= cpu->registers[reg2];
    return OK;
}

int and(uint8_t* instruction, Chip8State *cpu)
{
    int reg1 = getLowerNibble(instruction[0]);
    int reg2 = getUpperNibble(instruction[1]);
    cpu->registers[reg1] &= cpu->registers[reg2];
    return OK;
}

int xor(uint8_t* instruction, Chip8State *cpu)
{
    int reg1 = getLowerNibble(instruction[0]);
    int reg2 = getUpperNibble(instruction[1]);
    cpu->registers[reg1] ^= cpu->registers[reg2];
    return OK;
}

int add(uint8_t* instruction, Chip8State *cpu)
{
    int overflowFlag;
    int reg1 = getLowerNibble(instruction[0]);
    int reg2 = getUpperNibble(instruction[1]);

    overflowFlag = detectOverflow(cpu->registers[reg1], cpu->registers[reg2]);
    cpu->registers[reg1] += cpu->registers[reg2];
    cpu->registers[15] = overflowFlag;
    return OK;
}

int subXY(uint8_t* instruction, Chip8State *cpu)
{
    int underflowFlag;
    int reg1 = getLowerNibble(instruction[0]);
    int reg2 = getUpperNibble(instruction[1]);

    underflowFlag = detectUnderflow(cpu->registers[reg1], cpu->registers[reg2]);
    cpu->registers[reg1] -= cpu->registers[reg2];
    cpu->registers[15] = underflowFlag;
    return OK;
}

int shiftRight(uint8_t* instruction, Chip8State *cpu)
{
    uint8_t lsb;
    int reg1 = getLowerNibble(instruction[0]);
    int reg2 = getUpperNibble(instruction[1]);
    lsb = cpu->registers[reg1] & 0x01; 
    cpu->registers[15] = lsb;
    if(cpu->shiftMethod == 0)
    {
        cpu->registers[reg1] = cpu->registers[reg1] >> 1;
    } else if(cpu->shiftMethod == 1)
    {
        cpu->registers[reg2] = cpu->registers[reg1] >> 1;
    }
    return OK;
}

int subYX(uint8_t* instruction, Chip8State *cpu)
{
    int underflowFlag;
    int reg1 = getLowerNibble(instruction[0]);
    int reg2 = getUpperNibble(instruction[1]);
    cpu->registers[15] = detectUnderflow(cpu->registers[reg2], cpu->registers[reg1]);
    cpu->registers[reg1] = cpu->registers[reg2] - cpu->registers[reg1];
    return OK;
}
int skipNeq(uint8_t* instruction, Chip8State *cpu)
{
    int reg1 = getLowerNibble(instruction[0]);
    int reg2 = getUpperNibble(instruction[1]);
    if(cpu->registers[reg1] != cpu->registers[reg2])
    {
        cpu->PC += 2;
    }
    return OK;
}

int shiftLeft(uint8_t* instruction, Chip8State *cpu)
{ 
    uint8_t msb;
    int reg1 = getLowerNibble(instruction[0]);
    int reg2 = getUpperNibble(instruction[1]);
    msb = (cpu->registers[reg1] & 0x80) >> 7;
    cpu->registers[15] = msb;
    if(cpu->shiftMethod == 0)
    {
        cpu->registers[reg1] = cpu->registers[reg1] << 1;
    } else if(cpu->shiftMethod == 1)
    {
        cpu->registers[reg2] = cpu->registers[reg1] << 1;
    }
    return OK;
}

int loadI_Imm(uint8_t* instruction, Chip8State *cpu)
{
    uint16_t address = decode12bitAddr(instruction);
    cpu->VI = address;
    return OK;
}

int jmpIOffset(uint8_t* instruction, Chip8State *cpu)
{
    uint16_t offset = decode12bitAddr(instruction);
    cpu->PC = cpu->registers[0] + offset;
    return OK;
}

int setRand(uint8_t* instruction, Chip8State *cpu)
{
    uint8_t rand8Bit = (rand() % 256) & instruction[1];
    int reg = getLowerNibble(instruction[0]);
    cpu->registers[reg] = rand8Bit;
    return OK;
}

int drawSprite(uint8_t* instruction, Chip8State *cpu)
{
    uint16_t spriteAddr = cpu->VI;
    uint8_t xCoord = cpu->registers[getLowerNibble(instruction[0])];
    uint8_t yCoord = cpu->registers[getUpperNibble(instruction[1])];
    int numBytes = getLowerNibble(instruction[1]);
    uint8_t spritePixels[8];


    cpu->registers[15] = 0x00;

    for(int i = 0; i < numBytes; i++)
    {
        uint8_t spriteByte = cpu->memory[spriteAddr + i];
        uint8_t currentBit;
        for(int j = 7; j >= 0; j--)
        {
            currentBit = spriteByte & 1;
            if(currentBit == 1)
            {
                spritePixels[j] = 1;
            } else
            {
                spritePixels[j] = 0;
            }
            spriteByte >>= 1;
        }

        for(int j = 0; j < 8; j++)
        {
            int x = (xCoord + j)%64;
            int y = yCoord + i;

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
    return OK;
}

int skipIfKey(uint8_t* instruction, Chip8State *cpu)
{
    int reg = getLowerNibble(instruction[0]);
    if(numpad[cpu->registers[reg]])
    {
        cpu->PC += 2;
    }
    return OK;
}

int skipIfNotKey(uint8_t* instruction, Chip8State *cpu) 
{
    int reg = getLowerNibble(instruction[0]);
    if(!numpad[cpu->registers[reg]])
    {
        cpu->PC += 2; 
    }
    return OK;
}

int storeDT(uint8_t* instruction, Chip8State *cpu)
{
    int reg = getLowerNibble(instruction[0]);
    cpu->registers[reg] = cpu->DT;
    return OK;
}

int waitKey(uint8_t* instruction, Chip8State *cpu)
{
    int reg = getLowerNibble(instruction[0]);
    for(int i = 0; i < 16; i++)
    {
        if(numpad[i] != 0)
        {
            cpu->registers[reg] = i;
            return OK;
        }
    }
    // If no key is pressed, rewind program counter by 2 bytes so that this instruction will be repeated on the next pass.
    cpu->PC -= 2;  
    return OK;
}

int setDT(uint8_t* instruction, Chip8State *cpu)
{
    int reg = getLowerNibble(instruction[0]);
    cpu->DT = cpu->registers[reg];
    return OK;
}

int setST(uint8_t* instruction, Chip8State *cpu)
{
    int targetRegister = getLowerNibble(instruction[0]);
    cpu->ST = cpu->registers[targetRegister];
    return OK;
}

int addI(uint8_t* instruction, Chip8State *cpu)
{
    int reg = getLowerNibble(instruction[0]);
    cpu->VI = cpu->VI + cpu->registers[reg];
    return OK;
}


int setISpriteAddr(uint8_t* instruction, Chip8State *cpu)
{
    int targetRegister = getLowerNibble(instruction[0]);
    cpu->VI = cpu->registers[targetRegister]*5;
    return OK;
}

int storeBCD(uint8_t* instruction, Chip8State *cpu)
{
    int targetRegister = getLowerNibble(instruction[0]);
    uint8_t hundreds = 0;
    uint8_t tens = 0;
    uint8_t ones = 0;
    uint8_t value = cpu->registers[targetRegister];
    hundreds = value/100;
    value %= 100; 
    tens = value/10;
    value %= 10; 
    ones = value;

    cpu->memory[cpu->VI] = hundreds;
    cpu->memory[cpu->VI+1] = tens;
    cpu->memory[cpu->VI+2] = ones;
    return OK;
}

int storeRegs(uint8_t* instruction, Chip8State *cpu)
{
    int VX = getLowerNibble(instruction[0]); 
    for(int i = 0; i <= VX; i++)
    {
        cpu->memory[cpu->VI + i] = cpu->registers[i];
    }
    //cpu->VI += VX + 1;
    return OK;
}

int loadRegs(uint8_t* instruction, Chip8State *cpu)
{
    int VX = getLowerNibble(instruction[0]);
    for(int i = 0; i <= VX; i++)
    {
        cpu->registers[i] = cpu->memory[cpu->VI + i];
    }
    //cpu->VI += VX + 1;
    return OK;
}

int run(Chip8State *cpu)
{
    static struct timespec currentTime, resultTime;
    int opResult = 0;
    if(cpu->paused)
    {
        return OK;
    }
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

    uint8_t* nextInstruction = &cpu->memory[cpu->PC];
    opResult = opFuncs[decodeInstruction(nextInstruction)](nextInstruction, cpu);
    if(opResult != OK)
    {
        cpu->paused = true;
        cpu->error = opResult;
        return opResult;
    }
    cpu->PC += 2;

    clock_gettime(CLOCK_MONOTONIC, &currentTime);
    timediff(&resultTime, &currentTime, &cpu->prevTime);
    if((resultTime.tv_nsec / 1000000) > 16)
    {
        if(cpu->DT > 0)
        {
            cpu->DT--;
        }
        if(cpu->ST > 0)
        {
            cpu->ST--;
        }
        clock_gettime(CLOCK_MONOTONIC, &cpu->prevTime);
    }

    return opResult;

}
