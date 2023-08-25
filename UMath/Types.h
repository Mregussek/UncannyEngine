
#ifndef UNCANNYENGINE_TYPES_H
#define UNCANNYENGINE_TYPES_H


#include <type_traits>
#include "UTools/UTypes.h"
#include <cmath>
#include <float.h>


namespace uncanny::math
{


template<typename T>
concept ConceptMathType = std::is_integral_v<T> or std::is_floating_point_v<T>;


template<ConceptMathType T>
struct Vector2
{
  T x{ 0 };
  T y{ 0 };
};


template<ConceptMathType T>
struct Vector3
{
  T x{ 0 };
  T y{ 0 };
  T z{ 0 };
};


template<ConceptMathType T>
struct Vector4
{
  T x{ 0 };
  T y{ 0 };
  T z{ 0 };
  T w{ 0 };
};


template<ConceptMathType T>
struct Matrix4x4
{
  T values[4 * 4]{ 0 };

  T& operator[](u32 i) { return values[i]; }
  T operator[](u32 i) const { return values[i]; }

};


typedef Vector2<f32> Vector2f;
typedef Vector3<f32> Vector3f;
typedef Vector4<f32> Vector4f;
typedef Matrix4x4<f32> Matrix4x4f;

typedef Vector2<f64> Vector2d;
typedef Vector3<f64> Vector3d;
typedef Vector4<f64> Vector4d;
typedef Matrix4x4<f64> Matrix4x4d;

typedef Vector2<i32> Vector2i;
typedef Vector3<i32> Vector3i;
typedef Vector4<i32> Vector4i;
typedef Matrix4x4<i32> Matrix4x4i;

typedef Vector2<u32> Vector2u;
typedef Vector3<u32> Vector3u;
typedef Vector4<u32> Vector4u;
typedef Matrix4x4<u32> Matrix4x4u;


}


#endif //UNCANNYENGINE_TYPES_H
