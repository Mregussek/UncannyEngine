
#ifndef UNCANNYENGINE_MAT4_INL
#define UNCANNYENGINE_MAT4_INL


#include "mat4.h"


namespace uncanny
{


template<typename T> T mat4::getColumn(u32 i) const {
  static_assert(std::is_same<T, vec3>::value || std::is_same<T, vec4>::value,
                 "T must be vec3 or vec4!");
  if constexpr (std::is_same<T, vec3>::value) {
    return {
        elements[i + 0 * 4],
        elements[i + 1 * 4],
        elements[i + 2 * 4]
    };
  }
  else if constexpr (std::is_same<T, vec4>::value) {
    return {
        elements[i + 0 * 4],
        elements[i + 1 * 4],
        elements[i + 2 * 4],
        elements[i + 3 * 4]
    };
  }
}


template<typename T> T mat4::getRow(u32 i) const {
  static_assert(std::is_same<T, vec3>::value || std::is_same<T, vec4>::value,
                "T must be vec3 or vec4!");
  if constexpr (std::is_same<T, vec3>::value) {
    return {
        elements[0 + i * 4],
        elements[1 + i * 4],
        elements[2 + i * 4]
    };
  }
  else if constexpr (std::is_same<T, vec4>::value) {
    return {
        elements[0 + i * 4],
        elements[1 + i * 4],
        elements[2 + i * 4],
        elements[3 + i * 4]
    };
  }
}


template<typename T, typename U> U mat4::multiply(const T& other) const {
  static_assert(std::is_same<T, mat4>::value || std::is_same<T, vec4>::value,
                "T must be mat4 or vec4!");
  if constexpr (std::is_same<T, vec4>::value && std::is_same<U, vec4>::value) {
    return mat4::multiplyMat4Vec4(*this, other);
  }
  else if constexpr (std::is_same<T, mat4>::value && std::is_same<U, mat4>::value) {
    return mat4::multiplyMat4Mat4(*this, other);
  }
}


}


#endif
