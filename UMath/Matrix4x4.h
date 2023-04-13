
#ifndef UNCANNYENGINE_MATRIX4X4_H
#define UNCANNYENGINE_MATRIX4X4_H


#include "Types.h"
#include "Vector3.h"
#undef near
#undef far


namespace uncanny::math
{


/// @brief constructor for identity matrix creation with chosen diagonal
template<ConceptMathType T>
Matrix4x4<T> CreateDiagonal(T diagonal)
{
  Matrix4x4<T> rtn{};
  for (u32 i = 0; i < 4; i++) {
    rtn[i + i * 4] = diagonal;
  }
  return rtn;
}


/// @brief Creates identity matrix
/// [ 1   0   0   0 ]
/// [ 0   1   0   0 ]
/// [ 0   0   1   0 ]
/// [ 0   0   0   1 ]
template<ConceptMathType T>
Matrix4x4<T> Identity()
{
  return CreateDiagonal(1.f);
}


/// @brief Column getter function
/// @details indexed return values:\n
/// [ c0.x c1.x c2.x c3.x ]\n
/// [ c0.y c1.y c2.y c3.y ]\n
/// [ c0.z c1.z c2.z c3.z ]\n
/// [ c0.w c1.w c2.w c3.w ]
template<ConceptMathType T>
Vector3<T> GetColumn3(const Matrix4x4<T>& matrix, u32 idx)
{
  return { matrix[idx + 0 * 4],
           matrix[idx + 1 * 4],
           matrix[idx + 2 * 4] };
}


/// @brief Column getter function
/// @details indexed return values:\n
/// [ c0.x c1.x c2.x c3.x ]\n
/// [ c0.y c1.y c2.y c3.y ]\n
/// [ c0.z c1.z c2.z c3.z ]\n
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
/// @details indexed return values:\n
/// [ r0.x r0.y r0.z r0.w ]\n
/// [ r1.x r1.y r1.z r1.w ]\n
/// [ r2.x r2.y r2.z r2.w ]\n
/// [ r3.x r3.y r3.z r3.w ]
template<ConceptMathType T>
Vector3<T> GetRow3(const Matrix4x4<T>& matrix, u32 idx)
{
  return { matrix[0 + idx * 4],
           matrix[1 + idx * 4],
           matrix[2 + idx * 4] };
}


/// @brief Row getter function
/// @details indexed return values:\n
/// [ r0.x r0.y r0.z r0.w ]\n
/// [ r1.x r1.y r1.z r1.w ]\n
/// [ r2.x r2.y r2.z r2.w ]\n
/// [ r3.x r3.y r3.z r3.w ]
template<ConceptMathType T>
Vector4<T> GetRow4(const Matrix4x4<T>& matrix, u32 idx)
{
  return { matrix[0 + idx * 4],
           matrix[1 + idx * 4],
           matrix[2 + idx * 4],
           matrix[3 + idx * 4] };
}


/// @brief Performs multiplication of matrix with matrix
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


/// @brief Performs multiplication of matrix  with vector
template<ConceptMathType T>
Vector4<T> Multiply(const Matrix4x4<T>& m, const Vector4<T>& v)
{
  return { m[0 + 0 * 4] + v.x + m[0 + 1 * 4] + v.y + m[0 + 2 * 4] + v.z + m[0 + 3 * 4] + v.w,
           m[1 + 0 * 4] + v.x + m[1 + 1 * 4] + v.y + m[1 + 2 * 4] + v.z + m[1 + 3 * 4] + v.w,
           m[2 + 0 * 4] + v.x + m[2 + 1 * 4] + v.y + m[2 + 2 * 4] + v.z + m[2 + 3 * 4] + v.w,
           m[3 + 0 * 4] + v.x + m[3 + 1 * 4] + v.y + m[3 + 2 * 4] + v.z + m[3 + 3 * 4] + v.w };
}


/// @brief Creates perspective (3D) matrix
/// @returns
/// [ 1/(aspectRatio*tan(fov/2))  0              0                       0 ]\n
/// [ 0                           1/tan(fov/2)   0                       0 ]\n
/// [ 0                           0             -(far+near)/(far-near)  -1 ]\n
/// [ 0                           0             -2*far*near/(far-near)   0 ]
template<ConceptMathType T>
Matrix4x4<T> Perspective(T fov, T aspectRatio, T near, T far)
{
  Matrix4x4<T> result = Identity<T>();
  const T tanfov2{ (T)tan(fov / 2) };

  result[0 + 0 * 4] = (T)1 / (aspectRatio * tanfov2);
  result[1 + 1 * 4] = (T)1 / tanfov2;
  result[2 + 2 * 4] = -((far + near) / (far - near));
  result[3 + 2 * 4] = -(T)1;
  result[2 + 3 * 4] = -(((T)2 * far * near) / (far - near));

  return result;
}


/// @brief Creates lookAt (view) matrix.
/// @returns
/// fwd = normalize(target - eye)\n
/// side = normalize(fwd x y)\n
/// up = normalize(side x fwd)\n
/// [ side.x       up.x      -fwd.x     0 ]\n
/// [ side.y       up.y      -fwd.y     0 ]\n
/// [ side.z       up.z      -fwd.z     0 ]\n
/// [ -(side*eye) -(up*eye)  fwd*eye    1 ]
template<ConceptMathType T>
Matrix4x4<T> LookAt(Vector3<T> eye, Vector3<T> target, Vector3<T> y)
{
  Vector3<T> fwd{ Normalize(target - eye) };
  Vector3<T> side{ Normalize(CrossProduct(fwd, y)) };
  Vector3<T> up{ CrossProduct(side, fwd) };

  Matrix4x4<T> rtn{ 1.f };

  rtn[0 + 0 * 4] = side.x;
  rtn[0 + 1 * 4] = side.y;
  rtn[0 + 2 * 4] = side.z;

  rtn[1 + 0 * 4] = up.x;
  rtn[1 + 1 * 4] = up.y;
  rtn[1 + 2 * 4] = up.z;

  rtn[2 + 0 * 4] = -fwd.x;
  rtn[2 + 1 * 4] = -fwd.y;
  rtn[2 + 2 * 4] = -fwd.z;

  rtn[0 + 3 * 4] = -DotProduct(side, eye);
  rtn[1 + 3 * 4] = -DotProduct(up, eye);
  rtn[2 + 3 * 4] = DotProduct(fwd, eye);

  rtn[3 + 0 * 4] = 0.f;
  rtn[3 + 1 * 4] = 0.f;
  rtn[3 + 2 * 4] = 0.f;
  rtn[3 + 3 * 4] = 1.f;

  return rtn;
}


/// @brief Creates translation matrix from given vec3
/// @returns
/// [ 1   0   0   0 ]\n
/// [ 0   1   0   0 ]\n
/// [ 0   0   1   0 ]\n
/// [ t.x t.y t.z 1 ]
template<ConceptMathType T>
Matrix4x4<T> Translation(Vector3<T> vec)
{
  Matrix4x4<T> rtn(1);
  rtn[0 + 3 * 4] = vec.x;
  rtn[1 + 3 * 4] = vec.y;
  rtn[2 + 3 * 4] = vec.z;
  return rtn;
}


/// @brief Creates rotation matrix along given axis and angle
template<ConceptMathType T>
Matrix4x4<T> Rotation(T angle, Vector3<T> axis)
{
  Matrix4x4<T> rtn(1);

  T cosine{ (T)cos(angle) };
  T sine{ (T)sin(angle) };
  T neg_cosine{ (T)1 - cosine };

  Vector3<T> ax = Normalize(axis);
  T x{ (T)ax.x };
  T y{ (T)ax.y };
  T z{ (T)ax.z };

  rtn[0 + 0 * 4] = cosine + x * x * neg_cosine;
  rtn[1 + 0 * 4] = y * x * neg_cosine + z * sine;
  rtn[2 + 0 * 4] = z * x * neg_cosine - y * sine;

  rtn[0 + 1 * 4] = x * y * neg_cosine - z * sine;
  rtn[1 + 1 * 4] = cosine + y * y * neg_cosine;
  rtn[2 + 1 * 4] = z * y * neg_cosine + x * sine;

  rtn[0 + 2 * 4] = x * z * neg_cosine + y * sine;
  rtn[1 + 2 * 4] = y * z * neg_cosine - x * sine;
  rtn[2 + 2 * 4] = cosine + z * z * neg_cosine;

  return rtn;
}


/// @brief Creates scale matrix from given vec3
/// @returns
/// [ s.x 0   0   0 ]\n
/// [ 0   s.y 0   0 ]\n
/// [ 0   0   s.z 0 ]\n
/// [ 0   0   0   1 ]
template<ConceptMathType T>
Matrix4x4<T> Scale(Vector3<T> vec)
{
  Matrix4x4<T> rtn(1.0f);
  rtn[0 + 0 * 4] = vec.x;
  rtn[1 + 1 * 4] = vec.y;
  rtn[2 + 2 * 4] = vec.z;
  return rtn;
}


template<ConceptMathType T>
Matrix4x4<T> Inverse(const Matrix4x4<T>& m)
{
  Matrix4x4<T> inv{};
  inv[0] = m[5]  * m[10] * m[15] - m[5]  * m[11] * m[14] -
           m[9]  * m[6]  * m[15] + m[9]  * m[7]  * m[14] +
           m[13] * m[6]  * m[11] - m[13] * m[7]  * m[10];
  inv[4] = -m[4]  * m[10] * m[15] + m[4]  * m[11] * m[14] +
            m[8]  * m[6]  * m[15] - m[8]  * m[7]  * m[14] -
            m[12] * m[6]  * m[11] + m[12] * m[7]  * m[10];
  inv[8] = m[4]  * m[9]  * m[15] - m[4]  * m[11] * m[13] -
           m[8]  * m[5]  * m[15] + m[8]  * m[7]  * m[13] +
           m[12] * m[5]  * m[11] - m[12] * m[7]  * m[9];
  inv[12] = -m[4]  * m[9]  * m[14] + m[4]  * m[10] * m[13] +
             m[8]  * m[5]  * m[14] - m[8]  * m[6]  * m[13] -
             m[12] * m[5]  * m[10] + m[12] * m[6]  * m[9];
  inv[1] = -m[1]  * m[10] * m[15] + m[1]  * m[11] * m[14] +
            m[9]  * m[2]  * m[15] - m[9]  * m[3]  * m[14] -
            m[13] * m[2]  * m[11] + m[13] * m[3]  * m[10];
  inv[5] = m[0]  * m[10] * m[15] - m[0]  * m[11] * m[14] -
           m[8]  * m[2]  * m[15] + m[8]  * m[3]  * m[14] +
           m[12] * m[2]  * m[11] - m[12] * m[3]  * m[10];
  inv[9] = -m[0]  * m[9]  * m[15] + m[0]  * m[11] * m[13] +
            m[8]  * m[1]  * m[15] - m[8]  * m[3]  * m[13] -
            m[12] * m[1]  * m[11] + m[12] * m[3]  * m[9];
  inv[13] = m[0]  * m[9]  * m[14] - m[0]  * m[10] * m[13] -
            m[8]  * m[1]  * m[14] + m[8]  * m[2]  * m[13] +
            m[12] * m[1]  * m[10] - m[12] * m[2]  * m[9];
  inv[2] = m[1]  * m[6] * m[15] - m[1]  * m[7] * m[14] -
           m[5]  * m[2] * m[15] + m[5]  * m[3] * m[14] +
           m[13] * m[2] * m[7]  - m[13] * m[3] * m[6];
  inv[6] = -m[0]  * m[6] * m[15] + m[0]  * m[7] * m[14] +
            m[4]  * m[2] * m[15] - m[4]  * m[3] * m[14] -
            m[12] * m[2] * m[7]  + m[12] * m[3] * m[6];
  inv[10] = m[0]  * m[5] * m[15] - m[0]  * m[7] * m[13] -
            m[4]  * m[1] * m[15] + m[4]  * m[3] * m[13] +
            m[12] * m[1] * m[7]  - m[12] * m[3] * m[5];
  inv[14] = -m[0]  * m[5] * m[14] + m[0]  * m[6] * m[13] +
             m[4]  * m[1] * m[14] - m[4]  * m[2] * m[13] -
             m[12] * m[1] * m[6]  + m[12] * m[2] * m[5];
  inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] +
            m[5] * m[2] * m[11] - m[5] * m[3] * m[10] -
            m[9] * m[2] * m[7]  + m[9] * m[3] * m[6];
  inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] -
           m[4] * m[2] * m[11] + m[4] * m[3] * m[10] +
           m[8] * m[2] * m[7]  - m[8] * m[3] * m[6];
  inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] +
             m[4] * m[1] * m[11] - m[4] * m[3] * m[9] -
             m[8] * m[1] * m[7]  + m[8] * m[3] * m[5];
  inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] -
            m[4] * m[1] * m[10] + m[4] * m[2] * m[9] +
            m[8] * m[1] * m[6]  - m[8] * m[2] * m[5];

  T det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
  if (det == (T)0)
  {
    return Matrix4x4<T>{ FLT_MAX };
  }
  T invDet = (T)1 / det;
  for (T& value : inv.values)
  {
    value *= invDet;
  }
  return inv;
}


template<ConceptMathType T>
Matrix4x4<T> operator*(const Matrix4x4<T>& left, const Matrix4x4<T>& right) {
  return Multiply(left, right);
}


template<ConceptMathType T>
Vector4<T> operator*(const Matrix4x4<T>& left, const Vector4<T>& right) {
  return Multiply(left, right);
}


template<ConceptMathType T>
Vector4<T> operator*(const Vector4<T>& left, const Matrix4x4<T>& right) {
  return Multiply(right, left);
}


}


#endif //UNCANNYENGINE_MATRIX4X4_H
