
#include <gtest/gtest.h>
#include <math/vec3.h>


using namespace uncanny;


#define EXPECT_EQ_VEC3(v, ex, ey, ez)   EXPECT_EQ((v).x, ex); \
                                        EXPECT_EQ((v).y, ey); \
                                        EXPECT_EQ((v).z, ez)


TEST(Vec3TestSet, DefaultInitialization) {
  vec3 a{};
  EXPECT_EQ_VEC3(a, 0.f, 0.f, 0.f);

  const vec3 b;
  EXPECT_EQ_VEC3(b, 0.f, 0.f, 0.f);

  vec3 c = a;
  EXPECT_EQ_VEC3(c, 0.f, 0.f, 0.f);
}


TEST(Vec3TestSet, SpecificInitialization) {
  vec3 a{ 1.f, 2.f, 3.f };
  EXPECT_EQ_VEC3(a, 1.f, 2.f, 3.f);

  const vec3 b{ 4.f, 5.f, 6.f };
  EXPECT_EQ_VEC3(b, 4.f, 5.f, 6.f);

  vec3 c = a;
  EXPECT_EQ_VEC3(c, 1.f, 2.f, 3.f);
}


TEST(Vec3TestSet, AdditionOperation) {
  vec3 a{ 1.f, 2.f, 3.f };
  vec3 b{ 1.f, 2.f, 3.f };
  vec3 c;
  vec3 f;
  vec3 i;
  vec3 l;

  c = a.add(b);
  vec3 d = a.add(b);
  vec3 e{ a.add(b) };

  EXPECT_EQ_VEC3(c, 2.f, 4.f, 6.f);
  EXPECT_EQ_VEC3(d, 2.f, 4.f, 6.f);
  EXPECT_EQ_VEC3(e, 2.f, 4.f, 6.f);

  f = b.add(a);
  vec3 g = b.add(a);
  vec3 h{ b.add(a) };

  EXPECT_EQ_VEC3(f, 2.f, 4.f, 6.f);
  EXPECT_EQ_VEC3(g, 2.f, 4.f, 6.f);
  EXPECT_EQ_VEC3(h, 2.f, 4.f, 6.f);

  i = a + b;
  vec3 j = a + b;
  vec3 k{ a + b };

  EXPECT_EQ_VEC3(i, 2.f, 4.f, 6.f);
  EXPECT_EQ_VEC3(j, 2.f, 4.f, 6.f);
  EXPECT_EQ_VEC3(k, 2.f, 4.f, 6.f);

  l = b + a;
  vec3 m = b + a;
  vec3 n{ b + a };

  EXPECT_EQ_VEC3(l, 2.f, 4.f, 6.f);
  EXPECT_EQ_VEC3(m, 2.f, 4.f, 6.f);
  EXPECT_EQ_VEC3(n, 2.f, 4.f, 6.f);

  f32 o{ 0.f };
  f32 p{ -2.f };

  c = a.add(o);
  d = a.add(p);

  EXPECT_EQ_VEC3(c, 1.f, 2.f, 3.f);
  EXPECT_EQ_VEC3(d, -1.f, 0.f, 1.f);

  c = a + o;
  d = a + p;

  EXPECT_EQ_VEC3(c, 1.f, 2.f, 3.f);
  EXPECT_EQ_VEC3(d, -1.f, 0.f, 1.f);

  c = o + a;
  d = p + a;

  EXPECT_EQ_VEC3(c, 1.f, 2.f, 3.f);
  EXPECT_EQ_VEC3(d, -1.f, 0.f, 1.f);
}


TEST(Vec3TestSet, SubtractionOperation) {
  vec3 a{ 1.f, 2.f, 3.f };
  vec3 b{ 1.f, 3.f, 1.f };
  vec3 c;
  vec3 f;
  vec3 i;
  vec3 l;

  c = a.subtract(b);
  vec3 d = a.subtract(b);
  vec3 e{ a.subtract(b) };

  EXPECT_EQ_VEC3(c, 0.f, -1.f, 2.f);
  EXPECT_EQ_VEC3(d, 0.f, -1.f, 2.f);
  EXPECT_EQ_VEC3(e, 0.f, -1.f, 2.f);

  f = b.subtract(a);
  vec3 g = b.subtract(a);
  vec3 h{ b.subtract(a) };

  EXPECT_EQ_VEC3(f, 0.f, 1.f, -2.f);
  EXPECT_EQ_VEC3(g, 0.f, 1.f, -2.f);
  EXPECT_EQ_VEC3(h, 0.f, 1.f, -2.f);

  i = a - b;
  vec3 j = a - b;
  vec3 k{ a - b };

  EXPECT_EQ_VEC3(i, 0.f, -1.f, 2.f);
  EXPECT_EQ_VEC3(j, 0.f, -1.f, 2.f);
  EXPECT_EQ_VEC3(k, 0.f, -1.f, 2.f);

  l = b - a;
  vec3 m = b - a;
  vec3 n{ b - a };

  EXPECT_EQ_VEC3(l, 0.f, 1.f, -2.f);
  EXPECT_EQ_VEC3(m, 0.f, 1.f, -2.f);
  EXPECT_EQ_VEC3(n, 0.f, 1.f, -2.f);

  f32 o{ 0.f };
  f32 p{ -2.f };

  c = a.subtract(o);
  d = a.subtract(p);

  EXPECT_EQ_VEC3(c, 1.f, 2.f, 3.f);
  EXPECT_EQ_VEC3(d, 3.f, 4.f, 5.f);

  c = a - o;
  d = a - p;

  EXPECT_EQ_VEC3(c, 1.f, 2.f, 3.f);
  EXPECT_EQ_VEC3(d, 3.f, 4.f, 5.f);

  c = o - a;
  d = p - a;

  EXPECT_EQ_VEC3(c, -1.f, -2.f, -3.f);
  EXPECT_EQ_VEC3(d, -3.f, -4.f, -5.f);
}


TEST(Vec3TestSet, MultiplicationOperation) {
  vec3 a{ 1.f, 2.f, 3.f };
  vec3 b{ 0.f, 2.f, 1.5f };
  vec3 c;
  vec3 f;
  vec3 i;
  vec3 l;

  c = a.multiply(b);
  vec3 d = a.multiply(b);
  vec3 e{ a.multiply(b) };

  EXPECT_EQ_VEC3(c, 0.f, 4.f, 4.5f);
  EXPECT_EQ_VEC3(d, 0.f, 4.f, 4.5f);
  EXPECT_EQ_VEC3(e, 0.f, 4.f, 4.5f);

  f = b.multiply(a);
  vec3 g = b.multiply(a);
  vec3 h{ b.multiply(a) };

  EXPECT_EQ_VEC3(f, 0.f, 4.f, 4.5f);
  EXPECT_EQ_VEC3(g, 0.f, 4.f, 4.5f);
  EXPECT_EQ_VEC3(h, 0.f, 4.f, 4.5f);

  i = a * b;
  vec3 j = a * b;
  vec3 k{ a * b };

  EXPECT_EQ_VEC3(i, 0.f, 4.f, 4.5f);
  EXPECT_EQ_VEC3(j, 0.f, 4.f, 4.5f);
  EXPECT_EQ_VEC3(k, 0.f, 4.f, 4.5f);

  l = b * a;
  vec3 m = b * a;
  vec3 n{ b * a };

  EXPECT_EQ_VEC3(l, 0.f, 4.f, 4.5f);
  EXPECT_EQ_VEC3(m, 0.f, 4.f, 4.5f);
  EXPECT_EQ_VEC3(n, 0.f, 4.f, 4.5f);

  f32 o{ 0.f };
  f32 p{ -2.f };

  c = a.multiply(o);
  d = a.multiply(p);

  EXPECT_EQ_VEC3(c, 0.f, 0.f, 0.f);
  EXPECT_EQ_VEC3(d, -2.f, -4.f, -6.f);

  c = a * o;
  d = a * p;

  EXPECT_EQ_VEC3(c, 0.f, 0.f, 0.f);
  EXPECT_EQ_VEC3(d, -2.f, -4.f, -6.f);

  c = o * a;
  d = p * a;

  EXPECT_EQ_VEC3(c, 0.f, 0.f, 0.f);
  EXPECT_EQ_VEC3(d, -2.f, -4.f, -6.f);
}


TEST(Vec3TestSet, DivisionOperation) {
  vec3 a{ 1.f, 2.f, 3.f };
  vec3 b{ 0.f, 2.f, 1.5f };
  vec3 c;
  vec3 f;
  vec3 i;
  vec3 l;

  c = a.divide(b);
  vec3 d = a.divide(b);
  vec3 e{ a.divide(b) };

  EXPECT_EQ_VEC3(c, FLT_MAX, 1.f, 2.f);
  EXPECT_EQ_VEC3(d, FLT_MAX, 1.f, 2.f);
  EXPECT_EQ_VEC3(e, FLT_MAX, 1.f, 2.f);

  f = b.divide(a);
  vec3 g = b.divide(a);
  vec3 h{ b.divide(a) };

  EXPECT_EQ_VEC3(f, 0.f, 1.f, 0.5f);
  EXPECT_EQ_VEC3(g, 0.f, 1.f, 0.5f);
  EXPECT_EQ_VEC3(h, 0.f, 1.f, 0.5f);

  i = a / b;
  vec3 j = a / b;
  vec3 k{ a / b };

  EXPECT_EQ_VEC3(i, FLT_MAX, 1.f, 2.f);
  EXPECT_EQ_VEC3(j, FLT_MAX, 1.f, 2.f);
  EXPECT_EQ_VEC3(k, FLT_MAX, 1.f, 2.f);

  l = b / a;
  vec3 m = b / a;
  vec3 n{ b / a };

  EXPECT_EQ_VEC3(l, 0.f, 1.f, 0.5f);
  EXPECT_EQ_VEC3(m, 0.f, 1.f, 0.5f);
  EXPECT_EQ_VEC3(n, 0.f, 1.f, 0.5f);

  f32 o{ 0.f };
  f32 p{ -2.f };

  c = a.divide(o);
  d = a.divide(p);

  EXPECT_EQ_VEC3(c, FLT_MAX, FLT_MAX, FLT_MAX);
  EXPECT_EQ_VEC3(d, -0.5f, -1.f, -1.5f);

  c = a / o;
  d = a / p;

  EXPECT_EQ_VEC3(c, FLT_MAX, FLT_MAX, FLT_MAX);
  EXPECT_EQ_VEC3(d, -0.5f, -1.f, -1.5f);

  c = o / a;
  d = p / a;

  EXPECT_EQ_VEC3(c, 0.f, 0.f, 0.f);
  EXPECT_EQ_VEC3(d, -2.f, -1.f, -2.f / 3.f);
}


TEST(Vec3TestSet, ComparisonOperation) {
  vec3 a{ 1.f, 2.f, 3.f };
  vec3 b{ 1.f, 2.f, 3.f };
  vec3 c{ 1.f, 2.f, 3.0001f };
  vec3 d{ 0.f, 2.f, 3.f };

  EXPECT_EQ(vec3::compare(a, b), UTRUE);
  EXPECT_EQ(vec3::compare(b, a), UTRUE);

  EXPECT_EQ(vec3::compare(a, c), UFALSE);
  EXPECT_EQ(vec3::compare(a, d), UFALSE);
  EXPECT_EQ(vec3::compare(c, a), UFALSE);
  EXPECT_EQ(vec3::compare(d, a), UFALSE);

  EXPECT_EQ(vec3::compare(a, b, 0.f), UTRUE);
  EXPECT_EQ(vec3::compare(b, a, 0.f), UTRUE);

  EXPECT_EQ(vec3::compare(a, b, 1.f), UTRUE);
  EXPECT_EQ(vec3::compare(b, a, 1.f), UTRUE);

  EXPECT_EQ(vec3::compare(a, c, 0.f), UFALSE);
  EXPECT_EQ(vec3::compare(a, c, 0.0001f), UTRUE);
  EXPECT_EQ(vec3::compare(a, c, 0.00001f), UFALSE);

  EXPECT_EQ(vec3::compare(c, a, 0.f), UFALSE);
  EXPECT_EQ(vec3::compare(c, a, 0.0001f), UTRUE);
  EXPECT_EQ(vec3::compare(c, a, 0.00001f), UFALSE);

  EXPECT_EQ(a == b, UTRUE);
  EXPECT_EQ(b == a, UTRUE);

  EXPECT_EQ(a != c, UTRUE);
  EXPECT_EQ(c != a, UTRUE);
}


GTEST_API_ int main(int argc, char **argv) {
  printf("Running main() from %s\n", __FILE__);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
