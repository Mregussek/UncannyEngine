
#include "mat4.h"
#include "vec4.h"
#include "vec3.h"


namespace uncanny
{


mat4::mat4(f32 diagonal) {
  for (u32 i = 0; i < 4; i++) {
    elements[i + i * 4] = diagonal;
  }
}


mat4 mat4::identity() {
  return mat4(1.0f);
}


mat4 mat4::perspective(f32 fov, f32 aspectRatio, f32 near, f32 far) {
  mat4 result(1.0f);
  const f32 tanfov2{ tan(fov / 2.f) };

  result.elements[0 + 0 * 4] = 1.f / (aspectRatio * tanfov2);
  result.elements[1 + 1 * 4] = 1.f / tanfov2;
  result.elements[2 + 2 * 4] = -((far + near) / (far - near));
  result.elements[3 + 2 * 4] = -1.f;
  result.elements[2 + 3 * 4] = -((2.f * far * near) / (far - near));

  return result;
}


mat4 mat4::lookAt(vec3 eye, vec3 center, vec3 y) {
  const vec3 fwd{ vec3::normalize(center - eye) };
  const vec3 side{ vec3::normalize(vec3::cross(fwd, y)) };
  const vec3 up{ vec3::cross(side, fwd) };

  mat4 rtn{ 1.f };

  rtn[0 + 0 * 4] = side.x;
  rtn[0 + 1 * 4] = side.y;
  rtn[0 + 2 * 4] = side.z;

  rtn[1 + 0 * 4] = up.x;
  rtn[1 + 1 * 4] = up.y;
  rtn[1 + 2 * 4] = up.z;

  rtn[2 + 0 * 4] = -fwd.x;
  rtn[2 + 1 * 4] = -fwd.y;
  rtn[2 + 2 * 4] = -fwd.z;

  rtn[0 + 3 * 4] = -vec3::dot(side, eye);
  rtn[1 + 3 * 4] = -vec3::dot(up, eye);
  rtn[2 + 3 * 4] = vec3::dot(fwd, eye);

  rtn[3 + 0 * 4] = 0.f;
  rtn[3 + 1 * 4] = 0.f;
  rtn[3 + 2 * 4] = 0.f;
  rtn[3 + 3 * 4] = 1.f;

  return rtn;
}


mat4 mat4::translation(vec3 t) {
  mat4 result(1.0f);
  result.elements[0 + 3 * 4] = t.x;
  result.elements[1 + 3 * 4] = t.y;
  result.elements[2 + 3 * 4] = t.z;

  return result;
}


mat4 mat4::rotation(f32 angle, vec3 axis) {
  mat4 result(1.f);

  const f32 cosine{ cos(angle) };
  const f32 sine{ sin(angle) };
  const f32 neg_cosine{ 1.f - cosine };

  const vec3 ax{ vec3::normalize(axis) };
  const f32 x{ ax.x };
  const f32 y{ ax.y };
  const f32 z{ ax.z };

  result.elements[0 + 0 * 4] = cosine + x * x * neg_cosine;
  result.elements[1 + 0 * 4] = y * x * neg_cosine + z * sine;
  result.elements[2 + 0 * 4] = z * x * neg_cosine - y * sine;

  result.elements[0 + 1 * 4] = x * y * neg_cosine - z * sine;
  result.elements[1 + 1 * 4] = cosine + y * y * neg_cosine;
  result.elements[2 + 1 * 4] = z * y * neg_cosine + x * sine;

  result.elements[0 + 2 * 4] = x * z * neg_cosine + y * sine;
  result.elements[1 + 2 * 4] = y * z * neg_cosine - x * sine;
  result.elements[2 + 2 * 4] = cosine + z * z * neg_cosine;

  return result;
}

mat4 mat4::scale(vec3 params) {
  mat4 result(1.0f);
  result.elements[0 + 0 * 4] = params.x;
  result.elements[1 + 1 * 4] = params.y;
  result.elements[2 + 2 * 4] = params.z;
  return result;
}


b32 mat4::compare(const mat4& left, const mat4& right) {
  for (u32 i = 0; i < 16; i++) {
    if (left[i] != right[i]) {
      return UFALSE;
    }
  }
  return UTRUE;
};


f32 mat4::operator[](u32 index) const {
  if (index >= 4 * 4) {
    static_assert(true, "matrix.elements[index] out of bound!\n");
  }
  return elements[index];
}


f32& mat4::operator[](u32 index) {
  if (index >= 4 * 4) {
    static_assert(true, "const matrix.elements[index] out of bound!\n");
  }
  return elements[index];
}


mat4 operator*(const mat4& left, const mat4& right) {
  return left.multiply<mat4, mat4>(right);
}


vec4 operator*(const mat4& left, vec4 right) {
  return left.multiply<vec4, vec4>(right);
}


vec4 operator*(vec4 left, const mat4& right) {
  return right.multiply<vec4, vec4>(left);
}


mat4 mat4::multiplyMat4Mat4(const mat4& m, const mat4& n) {
  mat4 rtn;

  const vec4 left[4]{ m.getRow<vec4>(0), m.getRow<vec4>(1), m.getRow<vec4>(2), m.getRow<vec4>(3) };

  const vec4 col1{ left[0] * n[0 + 0 * 4] + left[1] * n[1 + 0 * 4] + left[2] * n[2 + 0 * 4] + left[3] * n[3 + 0 * 4] };
  const vec4 col2{ left[0] * n[0 + 1 * 4] + left[1] * n[1 + 1 * 4] + left[2] * n[2 + 1 * 4] + left[3] * n[3 + 1 * 4] };
  const vec4 col3{ left[0] * n[0 + 2 * 4] + left[1] * n[1 + 2 * 4] + left[2] * n[2 + 2 * 4] + left[3] * n[3 + 2 * 4] };
  const vec4 col4{ left[0] * n[0 + 3 * 4] + left[1] * n[1 + 3 * 4] + left[2] * n[2 + 3 * 4] + left[3] * n[3 + 3 * 4] };

  rtn[0 + 0 * 4] = col1.x;
  rtn[1 + 0 * 4] = col1.y;
  rtn[2 + 0 * 4] = col1.z;
  rtn[3 + 0 * 4] = col1.w;

  rtn[0 + 1 * 4] = col2.x;
  rtn[1 + 1 * 4] = col2.y;
  rtn[2 + 1 * 4] = col2.z;
  rtn[3 + 1 * 4] = col2.w;

  rtn[0 + 2 * 4] = col3.x;
  rtn[1 + 2 * 4] = col3.y;
  rtn[2 + 2 * 4] = col3.z;
  rtn[3 + 2 * 4] = col3.w;

  rtn[0 + 3 * 4] = col4.x;
  rtn[1 + 3 * 4] = col4.y;
  rtn[2 + 3 * 4] = col4.z;
  rtn[3 + 3 * 4] = col4.w;

  return rtn;
}


vec4 mat4::multiplyMat4Vec4(const mat4& m, const vec4& v) {
  return vec4{
    m[0 + 0 * 4] + v.x + m[0 + 1 * 4] + v.y + m[0 + 2 * 4] + v.z + m[0 + 3 * 4] + v.w,
    m[1 + 0 * 4] + v.x + m[1 + 1 * 4] + v.y + m[1 + 2 * 4] + v.z + m[1 + 3 * 4] + v.w,
    m[2 + 0 * 4] + v.x + m[2 + 1 * 4] + v.y + m[2 + 2 * 4] + v.z + m[2 + 3 * 4] + v.w,
    m[3 + 0 * 4] + v.x + m[3 + 1 * 4] + v.y + m[3 + 2 * 4] + v.z + m[3 + 3 * 4] + v.w
  };
}


}
