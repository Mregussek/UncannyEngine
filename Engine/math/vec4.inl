
#ifndef UNCANNYENGINE_VEC4_INL
#define UNCANNYENGINE_VEC4_INL


#include "vec4.h"


namespace uncanny
{


template<typename T> vec4 vec4::add(T v) const {
  if constexpr (std::is_arithmetic<T>::value && std::is_convertible<T, f32>::value) {
    return { x + (f32)v, y + (f32)v, z + (f32)v, w + (f32)v };
  }
  else if constexpr (std::is_same<T, vec4>::value) {
    return { x + v.x, y + v.y, z + v.z, w + v.w };
  }
}


template<typename T> vec4 vec4::subtract(T v) const {
  if constexpr (std::is_arithmetic<T>::value && std::is_convertible<T, f32>::value) {
    return { x - (f32)v, y - (f32)v, z - (f32)v, w - (f32)v };
  }
  else if constexpr (std::is_same<T, vec4>::value) {
    return { x - v.x, y - v.y, z - v.z, w - v.w };
  }
}


template<typename T> vec4 vec4::multiply(T v) const {
  if constexpr (std::is_arithmetic<T>::value && std::is_convertible<T, f32>::value) {
    return { x * (f32)v, y * (f32)v, z * (f32)v, w * (f32)v };
  }
  else if constexpr (std::is_same<T, vec4>::value) {
    return { x * v.x, y * v.y, z * v.z, w * v.w };
  }
}


template<typename T> vec4 vec4::divide(T v) const {
  if constexpr (std::is_arithmetic<T>::value && std::is_convertible<T, f32>::value) {
    if ((f32)v == 0.f) {
      return { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };
    }
    return { x / (f32)v, y / (f32)v, z / (f32)v, w / (f32)v };
  }
  else if constexpr (std::is_same<T, vec4>::value) {
    return {
        v.x != 0.f ? x / v.x : FLT_MAX,
        v.y != 0.f ? y / v.y : FLT_MAX,
        v.z != 0.f ? z / v.z : FLT_MAX,
        v.w != 0.f ? w / v.w : FLT_MAX
    };
  }
}


}


#endif
