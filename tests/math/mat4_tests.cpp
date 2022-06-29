
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


TEST(Mat4TestSet, Translation) {
  /*
  * [ 1 0 0 0 ]
  * [ 0 1 0 0 ]
  * [ 0 0 1 0 ]
  * [ 2 3 4 1 ]
  */

  mat4 t = mat4::translation({ 2.f, 3.f, 4.f });
  for(u32 i = 0; i < 12; i++) {
    if (i == 0 || i == 5 || i == 10) {
      EXPECT_EQ(t[i], 1.f);
      EXPECT_EQ(t.elements[i], 1.f);
      continue;
    }
    EXPECT_EQ(t[i], 0.f);
    EXPECT_EQ(t.elements[i], 0.f);
  }

  EXPECT_EQ(t[12], 2.f);
  EXPECT_EQ(t[13], 3.f);
  EXPECT_EQ(t[14], 4.f);
  EXPECT_EQ(t[15], 1.f);

  EXPECT_EQ(t.elements[12], 2.f);
  EXPECT_EQ(t.elements[13], 3.f);
  EXPECT_EQ(t.elements[14], 4.f);
  EXPECT_EQ(t.elements[15], 1.f);
}


TEST(Mat4TestSet, Scale) {
  /*
  * [ 2 0 0 0 ]
  * [ 0 3 0 0 ]
  * [ 0 0 4 0 ]
  * [ 0 0 0 1 ]
  */

  mat4 t = mat4::scale({ 2.f, 3.f, 4.f });
  for(u32 i = 0; i < 12; i++) {
    if (i == 0) {
      EXPECT_EQ(t[i], 2.f);
      EXPECT_EQ(t.elements[i], 2.f);
      continue;
    }
    if (i == 5) {
      EXPECT_EQ(t[i], 3.f);
      EXPECT_EQ(t.elements[i], 3.f);
      continue;
    }
    if (i == 10) {
      EXPECT_EQ(t[i], 4.f);
      EXPECT_EQ(t.elements[i], 4.f);
      continue;
    }
    if (i == 15) {
      EXPECT_EQ(t[i], 1.f);
      EXPECT_EQ(t.elements[i], 1.f);
      continue;
    }
    EXPECT_EQ(t[i], 0.f);
    EXPECT_EQ(t.elements[i], 0.f);
  }
}


TEST(Mat4TestSet, Perspective) {
  const f32 fov{ 2.f };
  const f32 aspectRatio{ 16.f / 9.f };
  const f32 near{ 0.001f };
  const f32 far{ 100.f };

  const f32 a{ 1.f / (aspectRatio * tan(fov / 2.f)) };
  const f32 b{ 1.f / tan(fov / 2.f) };
  const f32 c{ - (far + near) / (far - near) };
  const f32 d{ -2 * far * near / (far - near) };

  mat4 m{ mat4::perspective(fov, aspectRatio, near, far) };

  EXPECT_EQ(m[0], a);
  EXPECT_EQ(m[5], b);
  EXPECT_EQ(m[10], c);
  EXPECT_EQ(m[11], -1.f);
  EXPECT_EQ(m[14], d);

  for(u32 i = 0; i < 15; i++) {
    if (i == 0 || i == 5 || i == 10 || i == 11 || i == 14) {
      continue;
    }
    EXPECT_EQ(m[i], 0.f);
    EXPECT_EQ(m.elements[i], 0.f);
  }
}


static vec3 crossProduct(vec3 a, vec3 b) {
  return {
      a.y * b.z - b.y * a.z,
      a.z * b.x - b.z * a.x,
      a.x * b.y - b.x * a.y
  };
}


static f32 dotProduct(vec3 a, vec3 b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}


static vec3 normalizeVector(vec3 a) {
  f32 length{ sqrt(dotProduct(a, a))  };
  return a / length;
}


TEST(Mat4TestSet, LookAt) {
  const vec3 eye{ 0.f, 0.f, 0.f };
  const vec3 center{ 2.f, 1.f, -1.f };
  const vec3 y{ 0.f, 1.f, 0.f };

  mat4 m{ mat4::lookAt(eye, center, y) };

  const vec3 fwd{ normalizeVector(center - eye) };
  const vec3 side{ normalizeVector(crossProduct(fwd, y)) };
  const vec3 up{ crossProduct(side, fwd) };

  EXPECT_EQ(m[0], side.x);
  EXPECT_EQ(m[1], up.x);
  EXPECT_EQ(m[2], -fwd.x);
  EXPECT_EQ(m[3], 0.f);

  EXPECT_EQ(m[4], side.y);
  EXPECT_EQ(m[5], up.y);
  EXPECT_EQ(m[6], -fwd.y);
  EXPECT_EQ(m[7], 0.f);

  EXPECT_EQ(m[8], side.z);
  EXPECT_EQ(m[9], up.z);
  EXPECT_EQ(m[10], -fwd.z);
  EXPECT_EQ(m[11], 0.f);

  EXPECT_EQ(m[12], -dotProduct(side, eye));
  EXPECT_EQ(m[13], -dotProduct(up, eye));
  EXPECT_EQ(m[14], dotProduct(fwd, eye));
  EXPECT_EQ(m[15], 1.f);
}


static f32 deg2rad(f32 deg) {
  const f32 pi = 3.14159265359f;
  return deg * pi / 180.f;
}


TEST(Mat4TestSet, Rotation) {
  const f32 angle{ deg2rad(90.f) };
  const vec3 axis{ 0.5f, 1.f, 0.f };

  mat4 m{ mat4::rotation(angle, axis) };

  const vec3 naxis{ normalizeVector(axis) };
  const f32 x{ naxis.x };
  const f32 y{ naxis.y };
  const f32 z{ naxis.z };

  EXPECT_EQ(m[0], cos(angle) + x * x * (1 - cos(angle)));
  EXPECT_EQ(m[1], y * x * (1 - cos(angle)) + z * sin(angle));
  EXPECT_EQ(m[2], z * x * (1 - cos(angle)) - y * sin(angle));
  EXPECT_EQ(m[3], 0.f);

  EXPECT_EQ(m[4], x * y * (1 - cos(angle)) - z * sin(angle));
  EXPECT_EQ(m[5], cos(angle) + y * y * (1 - cos(angle)));
  EXPECT_EQ(m[6], z * y * (1 - cos(angle)) + x * sin(angle));
  EXPECT_EQ(m[7], 0.f);

  EXPECT_EQ(m[8], x * z * (1 - cos(angle)) + y * sin(angle));
  EXPECT_EQ(m[9], y * z * (1 - cos(angle)) - x * sin(angle));
  EXPECT_EQ(m[10], cos(angle) + z * z * (1 - cos(angle)));
  EXPECT_EQ(m[11], 0.f);

  EXPECT_EQ(m[12], 0.f);
  EXPECT_EQ(m[13], 0.f);
  EXPECT_EQ(m[14], 0.f);
  EXPECT_EQ(m[15], 1.f);
}


GTEST_API_ int main(int argc, char **argv) {
  printf("Running main() from %s\n", __FILE__);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
