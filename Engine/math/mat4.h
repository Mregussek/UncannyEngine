
#ifndef UNCANNYENGINE_MAT4_H
#define UNCANNYENGINE_MAT4_H


#include <utilities/Includes.h>


namespace uncanny
{


struct vec3;
struct vec4;


struct mat4 {

  f32 elements[4 * 4]{ 0.f };

  mat4() = default;
  explicit mat4(f32 diagonal);

  template<typename T> T getColumn(u32 i) const;
  template<typename T> T getRow(u32 i) const;

  static mat4 identity();

  template<typename T, typename U> U multiply(const T& other) const;

  static mat4 perspective(f32 fov, f32 aspectRatio, f32 near, f32 far);
  static mat4 lookAt(vec3 eye, vec3 center, vec3 y);
  static mat4 translation(vec3 v);
  static mat4 rotation(f32 angle, vec3 axis);
  static mat4 scale(vec3 params);
  static b32 compare(const mat4& left, const mat4& right);

  f32 operator[](u32 index) const;
  f32& operator[](u32 i);

private:

  static mat4 multiplyMat4Mat4(const mat4& m, const mat4& n);
  static vec4 multiplyMat4Vec4(const mat4& m, const vec4& v);

};


mat4 operator*(const mat4& left, const mat4& right);
vec4 operator*(const mat4& left, vec4 right);
vec4 operator*(vec4 left, const mat4& right);


}


#include "mat4.inl"


#endif
