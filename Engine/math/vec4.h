
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

  // @brief Default constructor, setups x, y, z, w values to 0.f
  vec4() = default;

  // @brief Setups values accordingly to arguments
  vec4(f32 _x, f32 _y, f32 _z, f32 _w);

  /*
   * @brief Adds given argument to vec4.
   * @tparam T should be vec4 or arithmetic type convertible to f32
   * @param v value of arithmetic type
   * @return computation result
   */
  template<typename T> vec4 add(T v) const;

  /*
   * @brief Subtracts given argument with vec4.
   * @tparam T should be vec4 or arithmetic type convertible to f32
   * @param v value of arithmetic type
   * @return computation result
   */
  template<typename T> vec4 subtract(T v) const;

  /*
   * @brief Multiplies given argument with vec4.
   * @tparam T should be vec4 or arithmetic type convertible to f32
   * @param v value of arithmetic type
   * @return computation result
   */
  template<typename T> vec4 multiply(T v) const;

  /*
   * @brief Divides given argument with vec4. When dividing by 0, values are set to FLT_MAX.
   * @tparam T should be vec4 or arithmetic type convertible to f32
   * @param v value of arithmetic type
   * @return computation result
   */
  template<typename T> vec4 divide(T v) const;

  /*
   * Compares two vectors to each other and returns result.
   * @param a first vector
   * @param b second vector
   * @return b32 value describing result (1 - the same, 0 - other)
   */
  static b32 compare(vec4 a, vec4 b);

  /*
   * Compares two vectors to each other with given margin and returns result.
   * @param a first vector
   * @param b second vector
   * @return b32 value describing result (1 - the same, 0 - other)
   */
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
