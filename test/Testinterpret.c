#include "unity.h"
#include "interpret.h"

void setUp(void) {}

void tearDown(void) {}

void test_do_nothing(void)
{
  TEST_ASSERT_TRUE(true);
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(test_do_nothing);
  return UNITY_END();
}
