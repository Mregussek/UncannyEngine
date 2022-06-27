
#include <gtest/gtest.h>
#include <math/vec3.h>


using namespace uncanny;


TEST(Vec3TestSet, DefaultInitialization) {
  vec3 a{};

  EXPECT_EQ(a.x, 0.f);
  EXPECT_EQ(a.y, 0.f);
  EXPECT_EQ(a.z, 0.f);
}


GTEST_API_ int main(int argc, char **argv) {
  printf("Running main() from %s\n", __FILE__);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
