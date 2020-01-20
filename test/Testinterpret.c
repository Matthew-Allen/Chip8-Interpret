#include "unity.h"
#include <stdint.h>
#include "interpret.h"

void setUp(void) {}

void tearDown(void) {}

void test_util_functions(void)
{
  TEST_ASSERT_EQUAL_UINT8(0x05, getUpperNibble(0x53));
  TEST_ASSERT_EQUAL_UINT8(0x08, getLowerNibble(0x38));
}

void lower_three_nibbles(char *instruction, int expectedResult) // Sweeps through lower-three nibble range of input instruction
{
  for(int i = 0; i < 16; i++)
  {
    for(int j = 0; j < 256; j++)
    {
      TEST_ASSERT_EQUAL_INT(expectedResult, decodeInstruction(instruction));
      instruction[1]++;
    }
    instruction[0]++;
  }
}

void middle_two_nibbles(char * instruction, int expectedResult) // Sweeps through middle-two nibble range of input instruction
{
  for(int i = 0; i < 16; i++)
  {
    for(int j = 0; j < 16; j++)
    {
      TEST_ASSERT_EQUAL_INT(expectedResult, decodeInstruction(instruction));
      instruction[1] += 16;
    }
    instruction[0]++;
  }
}

void test_decoder(void)
{
  uint8_t instruction[2] = {0,0}; 
  TEST_ASSERT_EQUAL_INT(EXEC_ASM, decodeInstruction(instruction)); 

  instruction[0] = 0x00;
  instruction[1] = 0xE0;
  TEST_ASSERT_EQUAL_INT(CLS, decodeInstruction(instruction)); // Clear Screen

  instruction[0] = 0x00;
  instruction[1] = 0xEE;
  TEST_ASSERT_EQUAL_INT(RETURN, decodeInstruction(instruction)); // Return from subroutine

  //Jump to address NNN
  instruction[0] = 0x10;
  instruction[1] = 0x00;
  lower_three_nibbles(instruction, JMP); 

  //Execute subroutine at address NNN
  instruction[0] = 0x20;
  instruction[1] = 0x00;
  lower_three_nibbles(instruction, EXEC_SUB);

  instruction[0] = 0x30;
  instruction[1] = 0x00;
  lower_three_nibbles(instruction, SKIP_EQ_IMM);

  instruction[0] = 0x40;
  instruction[1] = 0x00;
  lower_three_nibbles(instruction, SKIP_NEQ_IMM);

  instruction[0] = 0x50;
  instruction[1] = 0x00;
  lower_three_nibbles(instruction, SKIP_EQ);

  instruction[0] = 0x60;
  instruction[1] = 0x00;
  lower_three_nibbles(instruction, STORE_IMM);

  instruction[0] = 0x70;
  instruction[1] = 0x00;
  lower_three_nibbles(instruction, ADD_IMM);

  instruction[0] = 0x80;
  instruction[1] = 0x00;
  middle_two_nibbles(instruction, STORE); 
  
  instruction[0] = 0x80;
  instruction[1] = 0x01;
  middle_two_nibbles(instruction, OR);
  
  instruction[0] = 0x80;
  instruction[1] = 0x02;
  middle_two_nibbles(instruction, AND);

  instruction[0] = 0x80;
  instruction[1] = 0x03;
  middle_two_nibbles(instruction, XOR);

  instruction[0] = 0x80;
  instruction[1] = 0x04;
  middle_two_nibbles(instruction, ADD);

  instruction[0] = 0x80;
  instruction[1] = 0x05;
  middle_two_nibbles(instruction, SUBXY);

  instruction[0] = 0x80;
  instruction[1] = 0x06;
  middle_two_nibbles(instruction, SHIFT_RIGHT);

  instruction[0] = 0x80;
  instruction[1] = 0x07;
  middle_two_nibbles(instruction, SUBYX);

  instruction[0] = 0x80;
  instruction[1] = 0x0E;
  middle_two_nibbles(instruction, SHIFT_LEFT);

  instruction[0] = 0x90;
  instruction[1] = 0x00;
  lower_three_nibbles(instruction, SKIP_NEQ);

  instruction[0] = 0xA0;
  instruction[1] = 0x00;
  lower_three_nibbles(instruction, LOAD_I_IMM);

  instruction[0] = 0xB0;
  instruction[1] = 0x00;
  lower_three_nibbles(instruction, JMPI_OFFSET);

  instruction[0] = 0xC0;
  instruction[1] = 0x00;
  lower_three_nibbles(instruction, RAND);

  instruction[0] = 0xD0;
  instruction[1] = 0x00;
  lower_three_nibbles(instruction, DRAW_SPRITE);

  instruction[0] = 0xE0;
  instruction[1] = 0x9E;
  TEST_ASSERT_EQUAL_INT(SKIP_IFKEY, decodeInstruction(instruction));

  instruction[0] = 0xE0;
  instruction[1] = 0xA1;
  TEST_ASSERT_EQUAL_INT(SKIP_NOTKEY, decodeInstruction(instruction));

  instruction[0] = 0xF0;
  instruction[1] = 0x07;
  TEST_ASSERT_EQUAL_INT(STORE_DELAY, decodeInstruction(instruction));

  instruction[0] = 0xF0;
  instruction[1] = 0x0A;
  TEST_ASSERT_EQUAL_INT(WAIT_FOR_KEY, decodeInstruction(instruction));

  instruction[0] = 0xF0;
  instruction[1] = 0x15;
  TEST_ASSERT_EQUAL_INT(SET_DELAY, decodeInstruction(instruction));

  instruction[0] = 0xF0;
  instruction[1] = 0x18;
  TEST_ASSERT_EQUAL_INT(SET_SOUND_TIMER, decodeInstruction(instruction));

  instruction[0] = 0xF0;
  instruction[1] = 0x1E;
  TEST_ASSERT_EQUAL_INT(ADD_VX_I, decodeInstruction(instruction));

  instruction[0] = 0xF0;
  instruction[1] = 0x29;
  TEST_ASSERT_EQUAL_INT(SET_I_SPRITE, decodeInstruction(instruction));

  instruction[0] = 0xF0;
  instruction[1] = 0x33;
  TEST_ASSERT_EQUAL_INT(STORE_BCD, decodeInstruction(instruction));

  instruction[0] = 0xF0;
  instruction[1] = 0x55;
  TEST_ASSERT_EQUAL_INT(STORE_REGISTER_RANGE, decodeInstruction(instruction));

  instruction[0] = 0xF0;
  instruction[1] = 0x65;
  TEST_ASSERT_EQUAL_INT(LOAD_REGISTER_RANGE, decodeInstruction(instruction));

}

void test_return_function(void)
{
  
  Chip8State testState;
  uint8_t testInstruction[2];
  testInstruction[0] = 0x00;
  testInstruction[1] = 0xEE;
  testState.stackPointer = 1;
  testState.stack[0] = 1024;
  testState.PC = 2048;
  returnFromSub(testInstruction, &testState);
  TEST_ASSERT_EQUAL_UINT16(testState.PC, 1024);
  TEST_ASSERT_EQUAL_UINT8(testState.stackPointer, 0);
}

void test_jump_function(void)
{

}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(test_util_functions);
  RUN_TEST(test_decoder);
  RUN_TEST(test_return_function);
  return UNITY_END();
}
