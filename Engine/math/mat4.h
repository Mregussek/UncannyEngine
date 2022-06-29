
#ifndef UNCANNYENGINE_MAT4_H
#define UNCANNYENGINE_MAT4_H


#include <utilities/Includes.h>


namespace uncanny
{


struct vec3;
struct vec4;


struct mat4 {

  f32 elements[4 * 4]{ 0.f };

  // @brief Default constructor
  mat4() = default;

  // @brief Explicit constructor for identity matrix creation
  explicit mat4(f32 diagonal);

  /*
   * @brief Column retrieval function.
   * [ c0.x c1.x c2.x c3.x ]
   * [ c0.y c1.y c2.y c3.y ]
   * [ c0.z c1.z c2.z c3.z ]
   * [ c0.w c1.w c2.w c3.w ]
   * @tparam T vec3 or vec4
   * @param index of column to be extracted
   * @return vec3 or vec4 typed column
   */
  template<typename T> T getColumn(u32 index) const;

  /*
   * @brief Row retrieval function.
   * [ c0.x c0.y c0.z c0.w ]
   * [ c1.x c1.y c1.z c1.w ]
   * [ c2.x c2.y c2.z c2.w ]
   * [ c3.x c3.y c3.z c3.w ]
   * @tparam T vec3 or vec4
   * @param index of row to be extracted
   * @return vec3 or vec4 typed row
   */
  template<typename T> T getRow(u32 index) const;

  /*
   * @brief Creates identity matrix
   * [ 1   0   0   0 ]
   * [ 0   1   0   0 ]
   * [ 0   0   1   0 ]
   * [ 0   0   0   1 ]
   * @return constructed identity matrix
   */
  static mat4 identity();

  /*
   * @brief Performs matrix multiplication with given type.
   * @tparam T type that will be multiplied with matrix (mat4 or vec4 allowed)
   * @tparam U type that will be returned after matrix multiplication (mat4 or vec4 allowed)
   * @param other T-type object that will be multiplied with matrix
   * @return U-type object as a result of multiplication
   */
  template<typename T, typename U> U multiply(const T& other) const;

  static mat4 perspective(f32 fov, f32 aspectRatio, f32 near, f32 far);
  static mat4 lookAt(vec3 eye, vec3 center, vec3 y);

  /*
   * @brief Creates translation matrix from given vec3.
   * [ 1   0   0   0 ]
   * [ 0   1   0   0 ]
   * [ 0   0   1   0 ]
   * [ t.x t.y t.z 1 ]
   * @param t vec3 that describes translation along x, y and z-axis.
   * @return constructed translation matrix
   */
  static mat4 translation(vec3 t);
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
