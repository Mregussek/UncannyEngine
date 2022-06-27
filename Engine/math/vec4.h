
#ifndef UNCANNYENGINE_VEC4_H
#define UNCANNYENGINE_VEC4_H


#include <utilities/Includes.h>


namespace uncanny
{


struct vec4 {

  union { f32 x, r; };
  union { f32 y, g; };
  union { f32 z, b; };
  union { f32 w, a; };

  template<typename T> vec4 add(T v) const;
  template<typename T> vec4 subtract(T v) const;
  template<typename T> vec4 multiply(T v) const;
  template<typename T> vec4 divide(T v) const;

  static b32 compare(vec4 a, vec4 b);

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
