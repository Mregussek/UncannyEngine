
#include <gtest/gtest.h>
#include <utilities/Variables.h>


using namespace uncanny;


TEST(SampleIntegerTestSet, Calculation_i32) {
  i32 a{ 0 };
  i32 b{ 1 };

  const i32 sum{ a + b };
  const i32 subtract{ a - b };
  const i32 multiply{ a * b };
  const i32 divide{ a / b };

  EXPECT_EQ(sum, 1);
  EXPECT_EQ(subtract, -1);
  EXPECT_EQ(multiply, 0);
  EXPECT_EQ(divide, 0);
}


GTEST_API_ int main(int argc, char **argv) {
  printf("Running main() from %s\n", __FILE__);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
