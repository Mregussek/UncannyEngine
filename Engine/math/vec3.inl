
#ifndef UNCANNYENGINE_VEC3_INL
#define UNCANNYENGINE_VEC3_INL


#include "vec3.h"


namespace uncanny
{


template<typename T> vec3 vec3::add(T v) const {
  static_assert(
      std::is_same_v<T, vec3> || (std::is_arithmetic_v<T> && std::is_convertible_v<T, f32>),
      "T must be arithmetic and convertible to floating point or vec3!"
  );
  if constexpr (std::is_arithmetic<T>::value && std::is_convertible<T, f32>::value) {
    return { x + (f32)v, y + (f32)v, z + (f32)v };
  }
  else if constexpr (std::is_same<T, vec3>::value) {
    return { x + v.x, y + v.y, z + v.z };
  }
}


template<typename T> vec3 vec3::subtract(T v) const {
  static_assert(
      std::is_same_v<T, vec3> || (std::is_arithmetic_v<T> && std::is_convertible_v<T, f32>),
      "T must be arithmetic and convertible to floating point or vec3!"
  );
  if constexpr (std::is_arithmetic<T>::value && std::is_convertible<T, f32>::value) {
    return { x - (f32)v, y - (f32)v, z - (f32)v };
  }
  else if constexpr (std::is_same<T, vec3>::value) {
    return { x - v.x, y - v.y, z - v.z };
  }
}


template<typename T> vec3 vec3::multiply(T v) const {
  static_assert(
      std::is_same_v<T, vec3> || (std::is_arithmetic_v<T> && std::is_convertible_v<T, f32>),
      "T must be arithmetic and convertible to floating point or vec3!"
  );
  if constexpr (std::is_arithmetic<T>::value && std::is_convertible<T, f32>::value) {
    return { x * (f32)v, y * (f32)v, z * (f32)v };
  }
  else if constexpr (std::is_same<T, vec3>::value) {
    return { x * v.x, y * v.y, z * v.z };
  }
}


template<typename T> vec3 vec3::divide(T v) const {
  static_assert(
      std::is_same_v<T, vec3> || (std::is_arithmetic_v<T> && std::is_convertible_v<T, f32>),
      "T must be arithmetic and convertible to floating point or vec3!"
  );
  if constexpr (std::is_arithmetic<T>::value && std::is_convertible<T, f32>::value) {
    if ((f32)v == 0.f) {
      return { FLT_MAX, FLT_MAX, FLT_MAX };
    }
    return { x / (f32)v, y / (f32)v, z / (f32)v };
  }
  else if constexpr (std::is_same<T, vec3>::value) {
    return {
      v.x != 0.f ? x / v.x : FLT_MAX,
      v.y != 0.f ? y / v.y : FLT_MAX,
      v.z != 0.f ? z / v.z : FLT_MAX
    };
  }
}


}


#endif
