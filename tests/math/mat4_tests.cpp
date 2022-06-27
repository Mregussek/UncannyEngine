
#include <gtest/gtest.h>
#include <math/mat4.h>


using namespace uncanny;


TEST(Vec3TestSet, DefaultInitialization) {
  mat4 m{};

  for (const f32 i : m.elements) {
    EXPECT_EQ(i, 0.f);
  }
}


GTEST_API_ int main(int argc, char **argv) {
  printf("Running main() from %s\n", __FILE__);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
