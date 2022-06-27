
#ifndef UNCANNYENGINE_VEC3_H
#define UNCANNYENGINE_VEC3_H


#include <utilities/Includes.h>


namespace uncanny
{


struct vec3 {

  union { f32 x, r; };
  union { f32 y, g; };
  union { f32 z, b; };

  vec3() = default;
  vec3(f32 _x, f32 _y, f32 _z);

  template<typename T> vec3 add(T v) const;
  template<typename T> vec3 subtract(T v) const;
  template<typename T> vec3 multiply(T v) const;
  template<typename T> vec3 divide(T v) const;

  static vec3 cross(vec3 a, vec3 b);
  static f32 dot(vec3 a, vec3 b);
  static f32 length(vec3 v);
  static vec3 normalize(vec3 v);
  static b32 compare(vec3 a, vec3 b);
  static b32 compare(vec3 a, vec3 b, f32 margin);

  b32 operator==(vec3 other) const;
  b32 operator!=(vec3 other) const;

};


vec3 operator+(vec3 left, vec3 right);
vec3 operator-(vec3 left, vec3 right);
vec3 operator*(vec3 left, vec3 right);
vec3 operator/(vec3 left, vec3 right);

vec3 operator+(vec3 left, f32 right);
vec3 operator-(vec3 left, f32 right);
vec3 operator*(vec3 left, f32 right);
vec3 operator/(vec3 left, f32 right);

vec3 operator+(f32 left, vec3 right);
vec3 operator-(f32 left, vec3 right);
vec3 operator*(f32 left, vec3 right);
vec3 operator/(f32 left, vec3 right);


}


#include "vec3.inl"


#endif
