
#ifndef UNCANNYENGINE_MATRIX4X4_H
#define UNCANNYENGINE_MATRIX4X4_H


#include "Types.h"
#undef near
#undef far


namespace uncanny::math
{


template<ConceptMathType T>
Matrix4x4<T> CreateDiagonal(T diagonal)
{
  Matrix4x4<T> rtn{};
  for (u32 i = 0; i < 4; i++) {
    rtn.values[i + i * 4] = diagonal;
  }
  return rtn;
}


template<ConceptMathType T>
Matrix4x4<T> Identity()
{
  return CreateDiagonal(1.f);
}


/// @brief Column getter function
/// [ c0.x c1.x c2.x c3.x ]
/// [ c0.y c1.y c2.y c3.y ]
/// [ c0.z c1.z c2.z c3.z ]
/// [ c0.w c1.w c2.w c3.w ]
template<ConceptMathType T>
Vector3<T> GetColumn3(const Matrix4x4<T>& matrix, u32 idx)
{
  return { matrix[idx + 0 * 4],
           matrix[idx + 1 * 4],
           matrix[idx + 2 * 4] };
}


/// @brief Column getter function
/// [ c0.x c1.x c2.x c3.x ]
/// [ c0.y c1.y c2.y c3.y ]
/// [ c0.z c1.z c2.z c3.z ]
/// [ c0.w c1.w c2.w c3.w ]
template<ConceptMathType T>
Vector4<T> GetColumn4(const Matrix4x4<T>& matrix, u32 idx)
{
  return { matrix[idx + 0 * 4],
           matrix[idx + 1 * 4],
           matrix[idx + 2 * 4],
           matrix[idx + 3 * 4] };
}


/// @brief Row getter function
/// [ r0.x r0.y r0.z r0.w ]
/// [ r1.x r1.y r1.z r1.w ]
/// [ r2.x r2.y r2.z r2.w ]
/// [ r3.x r3.y r3.z r3.w ]
template<ConceptMathType T>
Vector3<T> GetRow3(const Matrix4x4<T>& matrix, u32 idx)
{
  return { matrix[0 + idx * 4],
           matrix[1 + idx * 4],
           matrix[2 + idx * 4] };
}


/// @brief Row getter function
/// [ r0.x r0.y r0.z r0.w ]
/// [ r1.x r1.y r1.z r1.w ]
/// [ r2.x r2.y r2.z r2.w ]
/// [ r3.x r3.y r3.z r3.w ]
template<ConceptMathType T>
Vector4<T> GetRow4(const Matrix4x4<T>& matrix, u32 idx)
{
  return { matrix[0 + idx * 4],
           matrix[1 + idx * 4],
           matrix[2 + idx * 4],
           matrix[3 + idx * 4] };
}


template<ConceptMathType T>
Matrix4x4<T> Multiply(const Matrix4x4<T>& a, const Matrix4x4<T>& b)
{
  Matrix4x4<T> rtn{};
  Vector4<T> cols[4]{ GetRow4(a, 0), GetRow4(a, 1), GetRow4(a, 2), GetRow4(a, 3) };

  Vector4<T> col1{ cols[0] * b[0 + 0 * 4] + cols[1] * b[1 + 0 * 4] + cols[2] * b[2 + 0 * 4] + cols[3] * b[3 + 0 * 4] };
  Vector4<T> col2{ cols[0] * b[0 + 1 * 4] + cols[1] * b[1 + 1 * 4] + cols[2] * b[2 + 1 * 4] + cols[3] * b[3 + 1 * 4] };
  Vector4<T> col3{ cols[0] * b[0 + 2 * 4] + cols[1] * b[1 + 2 * 4] + cols[2] * b[2 + 2 * 4] + cols[3] * b[3 + 2 * 4] };
  Vector4<T> col4{ cols[0] * b[0 + 3 * 4] + cols[1] * b[1 + 3 * 4] + cols[2] * b[2 + 3 * 4] + cols[3] * b[3 + 3 * 4] };

  rtn[0 + 0 * 4] = col1.x;
  rtn[1 + 0 * 4] = col1.y;
  rtn[2 + 0 * 4] = col1.z;
  rtn[3 + 0 * 4] = col1.w;

  rtn[0 + 1 * 4] = col2.x;
  rtn[1 + 1 * 4] = col2.y;
  rtn[2 + 1 * 4] = col2.z;
  rtn[3 + 1 * 4] = col2.w;

  rtn[0 + 2 * 4] = col3.x;
  rtn[1 + 2 * 4] = col3.y;
  rtn[2 + 2 * 4] = col3.z;
  rtn[3 + 2 * 4] = col3.w;

  rtn[0 + 3 * 4] = col4.x;
  rtn[1 + 3 * 4] = col4.y;
  rtn[2 + 3 * 4] = col4.z;
  rtn[3 + 3 * 4] = col4.w;

  return rtn;
}


template<ConceptMathType T>
Vector4<T> Multiply(const Matrix4x4<T>& m, const Vector4<T>& v)
{
  return { m[0 + 0 * 4] + v.x + m[0 + 1 * 4] + v.y + m[0 + 2 * 4] + v.z + m[0 + 3 * 4] + v.w,
           m[1 + 0 * 4] + v.x + m[1 + 1 * 4] + v.y + m[1 + 2 * 4] + v.z + m[1 + 3 * 4] + v.w,
           m[2 + 0 * 4] + v.x + m[2 + 1 * 4] + v.y + m[2 + 2 * 4] + v.z + m[2 + 3 * 4] + v.w,
           m[3 + 0 * 4] + v.x + m[3 + 1 * 4] + v.y + m[3 + 2 * 4] + v.z + m[3 + 3 * 4] + v.w };
}


template<ConceptMathType T>
Matrix4x4<T> Perspective(T fov, T aspectRatio, T near, T far)
{
  Matrix4x4<T> result(1);
  const T tanfov2{ (T)tan(fov / 2) };

  result.values[0 + 0 * 4] = (T)1 / (aspectRatio * tanfov2);
  result.values[1 + 1 * 4] = (T)1 / tanfov2;
  result.values[2 + 2 * 4] = -((far + near) / (far - near));
  result.values[3 + 2 * 4] = -(T)1;
  result.values[2 + 3 * 4] = -(((T)2 * far * near) / (far - near));

  return result;
}


}


#endif //UNCANNYENGINE_MATRIX4X4_H
