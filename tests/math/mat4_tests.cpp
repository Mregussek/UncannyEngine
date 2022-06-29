
#include <gtest/gtest.h>
#include <math/mat4.h>
#include <math/vec3.h>
#include <math/vec4.h>


using namespace uncanny;


/*
 * [ 0  1  2  3  ]
 * [ 4  5  6  7  ]
 * [ 8  9  10 11 ]
 * [ 12 13 14 15 ]
 */


TEST(Mat4TestSet, DefaultInitialization) {
  mat4 m{};
  for (const f32 i : m.elements) {
    EXPECT_EQ(i, 0.f);
  }

  const mat4 n;
  for (const f32 i : n.elements) {
    EXPECT_EQ(i, 0.f);
  }

  mat4 o = m;
  for (const f32 i : o.elements) {
    EXPECT_EQ(i, 0.f);
  }
}


TEST(Mat4TestSet, SpecificInitialization) {
  mat4 m{ 1.f };

  EXPECT_EQ(m.elements[0], 1.f);
  EXPECT_EQ(m.elements[5], 1.f);
  EXPECT_EQ(m.elements[10], 1.f);
  EXPECT_EQ(m.elements[15], 1.f);

  for (u32 i = 0; i < 4 * 4; i++) {
    if (i == 0 || i == 5 || i == 10 || i == 15) {
      continue;
    }
    EXPECT_EQ(m.elements[i], 0.f);
  }

  m = mat4{ 3.f };

  EXPECT_EQ(m.elements[0], 3.f);
  EXPECT_EQ(m.elements[5], 3.f);
  EXPECT_EQ(m.elements[10], 3.f);
  EXPECT_EQ(m.elements[15], 3.f);

  for (u32 i = 0; i < 4 * 4; i++) {
    if (i == 0 || i == 5 || i == 10 || i == 15) {
      continue;
    }
    EXPECT_EQ(m.elements[i], 0.f);
  }
}


TEST(Mat4TestSet, IdentityMatrix) {
  mat4 m{ mat4::identity() };

  EXPECT_EQ(m.elements[0], 1.f);
  EXPECT_EQ(m.elements[5], 1.f);
  EXPECT_EQ(m.elements[10], 1.f);
  EXPECT_EQ(m.elements[15], 1.f);

  for (u32 i = 0; i < 4 * 4; i++) {
    if (i == 0 || i == 5 || i == 10 || i == 15) {
      continue;
    }
    EXPECT_EQ(m.elements[i], 0.f);
  }

  const mat4 n = mat4::identity();

  EXPECT_EQ(n.elements[0], 1.f);
  EXPECT_EQ(n.elements[5], 1.f);
  EXPECT_EQ(n.elements[10], 1.f);
  EXPECT_EQ(n.elements[15], 1.f);

  for (u32 i = 0; i < 4 * 4; i++) {
    if (i == 0 || i == 5 || i == 10 || i == 15) {
      continue;
    }
    EXPECT_EQ(n.elements[i], 0.f);
  }
}


TEST(Mat4TestSet, ColumnRetrieval) {
  /*
  * [ 1 2 0 0 ]
  * [ 3 1 0 0 ]
  * [ 0 0 1 0 ]
  * [ 0 0 4 1 ]
  */

  mat4 m{ mat4::identity() };
  m.elements[1] = 2.f;
  m.elements[3] = 5.f;
  m.elements[4] = 3.f;
  m.elements[14] = 4.f;

  const vec3 v[4]{
    m.getColumn<vec3>(0),
    m.getColumn<vec3>(1),
    m.getColumn<vec3>(2),
    m.getColumn<vec3>(3)
  };

  EXPECT_EQ(v[0].x, 1.f);
  EXPECT_EQ(v[0].y, 3.f);
  EXPECT_EQ(v[0].z, 0.f);

  EXPECT_EQ(v[1].x, 2.f);
  EXPECT_EQ(v[1].y, 1.f);
  EXPECT_EQ(v[1].z, 0.f);

  EXPECT_EQ(v[2].x, 0.f);
  EXPECT_EQ(v[2].y, 0.f);
  EXPECT_EQ(v[2].z, 1.f);

  EXPECT_EQ(v[3].x, 5.f);
  EXPECT_EQ(v[3].y, 0.f);
  EXPECT_EQ(v[3].z, 0.f);

  const vec4 w[4]{
      m.getColumn<vec4>(0),
      m.getColumn<vec4>(1),
      m.getColumn<vec4>(2),
      m.getColumn<vec4>(3)
  };

  EXPECT_EQ(w[0].x, 1.f);
  EXPECT_EQ(w[0].y, 3.f);
  EXPECT_EQ(w[0].z, 0.f);
  EXPECT_EQ(w[0].w, 0.f);

  EXPECT_EQ(w[1].x, 2.f);
  EXPECT_EQ(w[1].y, 1.f);
  EXPECT_EQ(w[1].z, 0.f);
  EXPECT_EQ(w[1].w, 0.f);

  EXPECT_EQ(w[2].x, 0.f);
  EXPECT_EQ(w[2].y, 0.f);
  EXPECT_EQ(w[2].z, 1.f);
  EXPECT_EQ(w[2].w, 4.f);

  EXPECT_EQ(w[3].x, 5.f);
  EXPECT_EQ(w[3].y, 0.f);
  EXPECT_EQ(w[3].z, 0.f);
  EXPECT_EQ(w[3].w, 1.f);
}


TEST(Mat4TestSet, RowRetrieval) {
  /*
  * [ 1 2 0 0 ]
  * [ 3 1 0 0 ]
  * [ 0 0 1 0 ]
  * [ 0 0 4 1 ]
  */

  mat4 m{ mat4::identity() };
  m.elements[1] = 2.f;
  m.elements[3] = 5.f;
  m.elements[4] = 3.f;
  m.elements[14] = 4.f;

  const vec3 v[4]{
      m.getRow<vec3>(0),
      m.getRow<vec3>(1),
      m.getRow<vec3>(2),
      m.getRow<vec3>(3)
  };

  EXPECT_EQ(v[0].x, 1.f);
  EXPECT_EQ(v[0].y, 2.f);
  EXPECT_EQ(v[0].z, 0.f);

  EXPECT_EQ(v[1].x, 3.f);
  EXPECT_EQ(v[1].y, 1.f);
  EXPECT_EQ(v[1].z, 0.f);

  EXPECT_EQ(v[2].x, 0.f);
  EXPECT_EQ(v[2].y, 0.f);
  EXPECT_EQ(v[2].z, 1.f);

  EXPECT_EQ(v[3].x, 0.f);
  EXPECT_EQ(v[3].y, 0.f);
  EXPECT_EQ(v[3].z, 4.f);

  const vec4 w[4]{
      m.getRow<vec4>(0),
      m.getRow<vec4>(1),
      m.getRow<vec4>(2),
      m.getRow<vec4>(3)
  };

  EXPECT_EQ(w[0].x, 1.f);
  EXPECT_EQ(w[0].y, 2.f);
  EXPECT_EQ(w[0].z, 0.f);
  EXPECT_EQ(w[0].w, 5.f);

  EXPECT_EQ(w[1].x, 3.f);
  EXPECT_EQ(w[1].y, 1.f);
  EXPECT_EQ(w[1].z, 0.f);
  EXPECT_EQ(w[1].w, 0.f);

  EXPECT_EQ(w[2].x, 0.f);
  EXPECT_EQ(w[2].y, 0.f);
  EXPECT_EQ(w[2].z, 1.f);
  EXPECT_EQ(w[2].w, 0.f);

  EXPECT_EQ(w[3].x, 0.f);
  EXPECT_EQ(w[3].y, 0.f);
  EXPECT_EQ(w[3].z, 4.f);
  EXPECT_EQ(w[3].w, 1.f);
}


TEST(Mat4TestSet, Comparison) {
  mat4 m{ mat4::identity() };
  m.elements[1] = 2.f;
  m.elements[3] = 5.f;
  m.elements[4] = 3.f;
  m.elements[14] = 4.f;

  mat4 n{ mat4::identity() };
  n.elements[1] = 2.f;
  n.elements[3] = 5.f;
  n.elements[4] = 3.f;
  n.elements[14] = 4.f;

  EXPECT_EQ(mat4::compare(m, n), UTRUE);

  n.elements[4] = 5.f;
  EXPECT_EQ(mat4::compare(m, n), UFALSE);

  m.elements[4] = 5.f;
  EXPECT_EQ(mat4::compare(m, n), UTRUE);
}


TEST(Mat4TestSet, AssignOperator) {
  mat4 m = mat4::identity();
  m[1] = 2.f;
  m[3] = 5.f;
  m[4] = 3.f;
  m[14] = 4.f;

  EXPECT_EQ(m[1], 2.f);
  EXPECT_EQ(m[3], 5.f);
  EXPECT_EQ(m[4], 3.f);
  EXPECT_EQ(m[14], 4.f);

  EXPECT_EQ(m.elements[1], 2.f);
  EXPECT_EQ(m.elements[3], 5.f);
  EXPECT_EQ(m.elements[4], 3.f);
  EXPECT_EQ(m.elements[14], 4.f);

  m = mat4::identity();

  for(u32 i = 0; i < 15; i++) {
    if (i == 0 || i == 5 || i == 10 || i == 15) {
      EXPECT_EQ(m[i], 1.f);
      EXPECT_EQ(m.elements[i], 1.f);
      continue;
    }
    EXPECT_EQ(m[i], 0.f);
    EXPECT_EQ(m.elements[i], 0.f);
  }

  const mat4 n = mat4::identity();
  for(u32 i = 0; i < 15; i++) {
    if (i == 0 || i == 5 || i == 10 || i == 15) {
      EXPECT_EQ(n[i], 1.f);
      EXPECT_EQ(n.elements[i], 1.f);
      continue;
    }
    EXPECT_EQ(n[i], 0.f);
    EXPECT_EQ(n.elements[i], 0.f);
  }
}


GTEST_API_ int main(int argc, char **argv) {
  printf("Running main() from %s\n", __FILE__);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
