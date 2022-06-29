
#ifndef UNCANNYENGINE_VEC3_H
#define UNCANNYENGINE_VEC3_H


#include <utilities/Includes.h>


namespace uncanny
{


struct vec3 {

  f32 x{ 0.f };
  f32 y{ 0.f };
  f32 z{ 0.f };

  // @brief Default constructor, setups x, y, z values to 0.f
  vec3() = default;

  // @brief Setups values accordingly to arguments
  vec3(f32 _x, f32 _y, f32 _z);

  /*
   * @brief Adds given argument to vec3.
   * @tparam T should be vec3 or arithmetic type convertible to f32
   * @param v value of arithmetic type
   * @return computation result
   */
  template<typename T> vec3 add(T v) const;

  /*
   * @brief Subtracts given argument with vec3.
   * @tparam T should be vec3 or arithmetic type convertible to f32
   * @param v value of arithmetic type
   * @return computation result
   */
  template<typename T> vec3 subtract(T v) const;

  /*
   * @brief Multiplies given argument with vec3.
   * @tparam T should be vec3 or arithmetic type convertible to f32
   * @param v value of arithmetic type
   * @return computation result
   */
  template<typename T> vec3 multiply(T v) const;

  /*
   * @brief Divides given argument with vec3. When dividing by 0, values are set to FLT_MAX.
   * @tparam T should be vec3 or arithmetic type convertible to f32
   * @param v value of arithmetic type
   * @return computation result
   */
  template<typename T> vec3 divide(T v) const;

  /*
   * @brief Calculates cross product for two vectors
   * (vector that is perpendicular to both a and b)
   * @param a first vector
   * @param b second vector
   * @return cross product
   */
  static vec3 cross(vec3 a, vec3 b);

  /*
   * @brief Calculates dot product (inner or rarely projection product).
   * @param a first vector
   * @param b second vector
   * @return dot product
   */
  static f32 dot(vec3 a, vec3 b);

  /*
   * @brief Calculates length of given vector
   * @param v just vector
   * @return computed length
   */
  static f32 length(vec3 v);

  /*
   * @brief Normalizes given vector to range <0, 1>
   * @param v just vector
   * @return normalized vector
   */
  static vec3 normalize(vec3 v);

  /*
   * Compares two vectors to each other and returns result.
   * @param a first vector
   * @param b second vector
   * @return b32 value describing result (1 - the same, 0 - other)
   */
  static b32 compare(vec3 a, vec3 b);

  /*
   * Compares two vectors to each other with given margin and returns result.
   * @param a first vector
   * @param b second vector
   * @return b32 value describing result (1 - the same, 0 - other)
   */
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
