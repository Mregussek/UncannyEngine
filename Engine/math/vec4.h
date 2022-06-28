
#ifndef UNCANNYENGINE_VEC4_H
#define UNCANNYENGINE_VEC4_H


#include <utilities/Includes.h>


namespace uncanny
{


struct vec4 {

  f32 x{ 0.f };
  f32 y{ 0.f };
  f32 z{ 0.f };
  f32 w{ 0.f };

  vec4() = default;
  vec4(f32 _x, f32 _y, f32 _z, f32 _w);

  template<typename T> vec4 add(T v) const;
  template<typename T> vec4 subtract(T v) const;
  template<typename T> vec4 multiply(T v) const;
  template<typename T> vec4 divide(T v) const;

  static b32 compare(vec4 a, vec4 b);
  static b32 compare(vec4 a, vec4 b, f32 margin);

  b32 operator==(vec4 other) const;
  b32 operator!=(vec4 other) const;

};


vec4 operator+(vec4 left, vec4 right);
vec4 operator-(vec4 left, vec4 right);
vec4 operator*(vec4 left, vec4 right);
vec4 operator/(vec4 left, vec4 right);

vec4 operator+(vec4 left, f32 right);
vec4 operator-(vec4 left, f32 right);
vec4 operator*(vec4 left, f32 right);
vec4 operator/(vec4 left, f32 right);

vec4 operator+(f32 left, vec4 right);
vec4 operator-(f32 left, vec4 right);
vec4 operator*(f32 left, vec4 right);
vec4 operator/(f32 left, vec4 right);


}


#include "vec4.inl"


#endif
