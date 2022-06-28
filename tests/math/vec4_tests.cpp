
#include <gtest/gtest.h>
#include <math/vec4.h>


using namespace uncanny;


#define EXPECT_EQ_VEC4(v, ex, ey, ez, ew)   EXPECT_EQ((v).x, ex); \
                                            EXPECT_EQ((v).y, ey); \
                                            EXPECT_EQ((v).z, ez); \
                                            EXPECT_EQ((v).w, ew);


TEST(Vec4TestSet, DefaultInitialization) {
  vec4 a{};
  EXPECT_EQ_VEC4(a, 0.f, 0.f, 0.f, 0.f);

  const vec4 b;
  EXPECT_EQ_VEC4(b, 0.f, 0.f, 0.f, 0.f);

  vec4 c = a;
  EXPECT_EQ_VEC4(c, 0.f, 0.f, 0.f, 0.f);
}


TEST(Vec4TestSet, SpecificInitialization) {
  vec4 a{ 1.f, 2.f, 3.f, 0.f };
  EXPECT_EQ_VEC4(a, 1.f, 2.f, 3.f, 0.f);

  const vec4 b{ 4.f, 5.f, 6.f, 8.f };
  EXPECT_EQ_VEC4(b, 4.f, 5.f, 6.f, 8.f);

  vec4 c = a;
  EXPECT_EQ_VEC4(c, 1.f, 2.f, 3.f, 0.f);
}


TEST(Vec4TestSet, AdditionOperation) {
  vec4 a{ 1.f, 2.f, 3.f, 4.f };
  vec4 b{ 1.f, 2.f, 3.f, 4.f };
  vec4 c;
  vec4 f;
  vec4 i;
  vec4 l;

  c = a.add(b);
  vec4 d = a.add(b);
  vec4 e{ a.add(b) };

  EXPECT_EQ_VEC4(c, 2.f, 4.f, 6.f, 8.f);
  EXPECT_EQ_VEC4(d, 2.f, 4.f, 6.f, 8.f);
  EXPECT_EQ_VEC4(e, 2.f, 4.f, 6.f, 8.f);

  f = b.add(a);
  vec4 g = b.add(a);
  vec4 h{ b.add(a) };

  EXPECT_EQ_VEC4(f, 2.f, 4.f, 6.f, 8.f);
  EXPECT_EQ_VEC4(g, 2.f, 4.f, 6.f, 8.f);
  EXPECT_EQ_VEC4(h, 2.f, 4.f, 6.f, 8.f);

  i = a + b;
  vec4 j = a + b;
  vec4 k{ a + b };

  EXPECT_EQ_VEC4(i, 2.f, 4.f, 6.f, 8.f);
  EXPECT_EQ_VEC4(j, 2.f, 4.f, 6.f, 8.f);
  EXPECT_EQ_VEC4(k, 2.f, 4.f, 6.f, 8.f);

  l = b + a;
  vec4 m = b + a;
  vec4 n{ b + a };

  EXPECT_EQ_VEC4(l, 2.f, 4.f, 6.f, 8.f);
  EXPECT_EQ_VEC4(m, 2.f, 4.f, 6.f, 8.f);
  EXPECT_EQ_VEC4(n, 2.f, 4.f, 6.f, 8.f);

  f32 o{ 0.f };
  f32 p{ -2.f };

  c = a.add(o);
  d = a.add(p);

  EXPECT_EQ_VEC4(c, 1.f, 2.f, 3.f, 4.f);
  EXPECT_EQ_VEC4(d, -1.f, 0.f, 1.f, 2.f);

  c = a + o;
  d = a + p;

  EXPECT_EQ_VEC4(c, 1.f, 2.f, 3.f, 4.f);
  EXPECT_EQ_VEC4(d, -1.f, 0.f, 1.f, 2.f);

  c = o + a;
  d = p + a;

  EXPECT_EQ_VEC4(c, 1.f, 2.f, 3.f, 4.f);
  EXPECT_EQ_VEC4(d, -1.f, 0.f, 1.f, 2.f);
}


TEST(Vec4TestSet, SubtractionOperation) {
  vec4 a{ 1.f, 2.f, 3.f, 4.f };
  vec4 b{ 1.f, 3.f, 1.f, -2.f };
  vec4 c;
  vec4 f;
  vec4 i;
  vec4 l;

  c = a.subtract(b);
  vec4 d = a.subtract(b);
  vec4 e{ a.subtract(b) };

  EXPECT_EQ_VEC4(c, 0.f, -1.f, 2.f, 6.f);
  EXPECT_EQ_VEC4(d, 0.f, -1.f, 2.f, 6.f);
  EXPECT_EQ_VEC4(e, 0.f, -1.f, 2.f, 6.f);

  f = b.subtract(a);
  vec4 g = b.subtract(a);
  vec4 h{ b.subtract(a) };

  EXPECT_EQ_VEC4(f, 0.f, 1.f, -2.f, -6.f);
  EXPECT_EQ_VEC4(g, 0.f, 1.f, -2.f, -6.f);
  EXPECT_EQ_VEC4(h, 0.f, 1.f, -2.f, -6.f);

  i = a - b;
  vec4 j = a - b;
  vec4 k{ a - b };

  EXPECT_EQ_VEC4(i, 0.f, -1.f, 2.f, 6.f);
  EXPECT_EQ_VEC4(j, 0.f, -1.f, 2.f, 6.f);
  EXPECT_EQ_VEC4(k, 0.f, -1.f, 2.f, 6.f);

  l = b - a;
  vec4 m = b - a;
  vec4 n{ b - a };

  EXPECT_EQ_VEC4(l, 0.f, 1.f, -2.f, -6.f);
  EXPECT_EQ_VEC4(m, 0.f, 1.f, -2.f, -6.f);
  EXPECT_EQ_VEC4(n, 0.f, 1.f, -2.f, -6.f);

  f32 o{ 0.f };
  f32 p{ -2.f };

  c = a.subtract(o);
  d = a.subtract(p);

  EXPECT_EQ_VEC4(c, 1.f, 2.f, 3.f, 4.f);
  EXPECT_EQ_VEC4(d, 3.f, 4.f, 5.f, 6.f);

  c = a - o;
  d = a - p;

  EXPECT_EQ_VEC4(c, 1.f, 2.f, 3.f, 4.f);
  EXPECT_EQ_VEC4(d, 3.f, 4.f, 5.f, 6.f);

  c = o - a;
  d = p - a;

  EXPECT_EQ_VEC4(c, -1.f, -2.f, -3.f, -4.f);
  EXPECT_EQ_VEC4(d, -3.f, -4.f, -5.f, -6.f);
}


TEST(Vec4TestSet, MultiplicationOperation) {
  vec4 a{ 1.f, 2.f, 3.f, 4.f };
  vec4 b{ 0.f, 2.f, 1.5f, 1.f };
  vec4 c;
  vec4 f;
  vec4 i;
  vec4 l;

  c = a.multiply(b);
  vec4 d = a.multiply(b);
  vec4 e{ a.multiply(b) };

  EXPECT_EQ_VEC4(c, 0.f, 4.f, 4.5f, 4.f);
  EXPECT_EQ_VEC4(d, 0.f, 4.f, 4.5f, 4.f);
  EXPECT_EQ_VEC4(e, 0.f, 4.f, 4.5f, 4.f);

  f = b.multiply(a);
  vec4 g = b.multiply(a);
  vec4 h{ b.multiply(a) };

  EXPECT_EQ_VEC4(f, 0.f, 4.f, 4.5f, 4.f);
  EXPECT_EQ_VEC4(g, 0.f, 4.f, 4.5f, 4.f);
  EXPECT_EQ_VEC4(h, 0.f, 4.f, 4.5f, 4.f);

  i = a * b;
  vec4 j = a * b;
  vec4 k{ a * b };

  EXPECT_EQ_VEC4(i, 0.f, 4.f, 4.5f, 4.f);
  EXPECT_EQ_VEC4(j, 0.f, 4.f, 4.5f, 4.f);
  EXPECT_EQ_VEC4(k, 0.f, 4.f, 4.5f, 4.f);

  l = b * a;
  vec4 m = b * a;
  vec4 n{ b * a };

  EXPECT_EQ_VEC4(l, 0.f, 4.f, 4.5f, 4.f);
  EXPECT_EQ_VEC4(m, 0.f, 4.f, 4.5f, 4.f);
  EXPECT_EQ_VEC4(n, 0.f, 4.f, 4.5f, 4.f);

  f32 o{ 0.f };
  f32 p{ -2.f };

  c = a.multiply(o);
  d = a.multiply(p);

  EXPECT_EQ_VEC4(c, 0.f, 0.f, 0.f, 0.f);
  EXPECT_EQ_VEC4(d, -2.f, -4.f, -6.f, -8.f);

  c = a * o;
  d = a * p;

  EXPECT_EQ_VEC4(c, 0.f, 0.f, 0.f, 0.f);
  EXPECT_EQ_VEC4(d, -2.f, -4.f, -6.f, -8.f);

  c = o * a;
  d = p * a;

  EXPECT_EQ_VEC4(c, 0.f, 0.f, 0.f, 0.f);
  EXPECT_EQ_VEC4(d, -2.f, -4.f, -6.f, -8.f);
}


TEST(Vec4TestSet, DivisionOperation) {
  vec4 a{ 1.f, 2.f, 3.f, 4.f };
  vec4 b{ 0.f, 2.f, 1.5f, 2.f };
  vec4 c;
  vec4 f;
  vec4 i;
  vec4 l;

  c = a.divide(b);
  vec4 d = a.divide(b);
  vec4 e{ a.divide(b) };

  EXPECT_EQ_VEC4(c, FLT_MAX, 1.f, 2.f, 2.f);
  EXPECT_EQ_VEC4(d, FLT_MAX, 1.f, 2.f, 2.f);
  EXPECT_EQ_VEC4(e, FLT_MAX, 1.f, 2.f, 2.f);

  f = b.divide(a);
  vec4 g = b.divide(a);
  vec4 h{ b.divide(a) };

  EXPECT_EQ_VEC4(f, 0.f, 1.f, 0.5f, 0.5f);
  EXPECT_EQ_VEC4(g, 0.f, 1.f, 0.5f, 0.5f);
  EXPECT_EQ_VEC4(h, 0.f, 1.f, 0.5f, 0.5f);

  i = a / b;
  vec4 j = a / b;
  vec4 k{ a / b };

  EXPECT_EQ_VEC4(i, FLT_MAX, 1.f, 2.f, 2.f);
  EXPECT_EQ_VEC4(j, FLT_MAX, 1.f, 2.f, 2.f);
  EXPECT_EQ_VEC4(k, FLT_MAX, 1.f, 2.f, 2.f);

  l = b / a;
  vec4 m = b / a;
  vec4 n{ b / a };

  EXPECT_EQ_VEC4(l, 0.f, 1.f, 0.5f, 0.5f);
  EXPECT_EQ_VEC4(m, 0.f, 1.f, 0.5f, 0.5f);
  EXPECT_EQ_VEC4(n, 0.f, 1.f, 0.5f, 0.5f);

  f32 o{ 0.f };
  f32 p{ -2.f };

  c = a.divide(o);
  d = a.divide(p);

  EXPECT_EQ_VEC4(c, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
  EXPECT_EQ_VEC4(d, -0.5f, -1.f, -1.5f, -2.f);

  c = a / o;
  d = a / p;

  EXPECT_EQ_VEC4(c, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
  EXPECT_EQ_VEC4(d, -0.5f, -1.f, -1.5f, -2.f);

  c = o / a;
  d = p / a;

  EXPECT_EQ_VEC4(c, 0.f, 0.f, 0.f, 0.f);
  EXPECT_EQ_VEC4(d, -2.f, -1.f, -2.f / 3.f, -0.5f);
}


TEST(Vec4TestSet, ComparisonOperation) {
  vec4 a{ 1.f, 2.f, 3.f, 4.f };
  vec4 b{ 1.f, 2.f, 3.f, 4.f };
  vec4 c{ 1.f, 2.f, 3.0001f, 4.f };
  vec4 d{ 0.f, 2.f, 3.f, 4.f };

  EXPECT_EQ(vec4::compare(a, b), UTRUE);
  EXPECT_EQ(vec4::compare(b, a), UTRUE);

  EXPECT_EQ(vec4::compare(a, c), UFALSE);
  EXPECT_EQ(vec4::compare(a, d), UFALSE);
  EXPECT_EQ(vec4::compare(c, a), UFALSE);
  EXPECT_EQ(vec4::compare(d, a), UFALSE);

  EXPECT_EQ(vec4::compare(a, b, 0.f), UTRUE);
  EXPECT_EQ(vec4::compare(b, a, 0.f), UTRUE);

  EXPECT_EQ(vec4::compare(a, b, 1.f), UTRUE);
  EXPECT_EQ(vec4::compare(b, a, 1.f), UTRUE);

  EXPECT_EQ(vec4::compare(a, c, 0.f), UFALSE);
  EXPECT_EQ(vec4::compare(a, c, 0.0001f), UTRUE);
  EXPECT_EQ(vec4::compare(a, c, 0.00001f), UFALSE);

  EXPECT_EQ(vec4::compare(c, a, 0.f), UFALSE);
  EXPECT_EQ(vec4::compare(c, a, 0.0001f), UTRUE);
  EXPECT_EQ(vec4::compare(c, a, 0.00001f), UFALSE);
  
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
