
#ifndef UNCANNYENGINE_VECTOR4_H
#define UNCANNYENGINE_VECTOR4_H


#include "Types.h"


namespace uncanny::math
{


template<ConceptMathType T>
Vector4<T> Create(T x, T y, T z, T w)
{
return { .x = x, .y = y, .z = z, .w = w };
}


template<ConceptMathType T>
Vector4<T> Add(Vector4<T> left, T right)
{
  return { .x = left.x + right,
           .y = left.y + right,
           .z = left.z + right,
           .w = left.w + right };
}


template<ConceptMathType T>
Vector4<T> Subtract(Vector4<T> left, T right)
{
  return { .x = left.x - right,
           .y = left.y - right,
           .z = left.z - right,
           .w = left.w - right };
}


template<ConceptMathType T>
Vector4<T> Multiply(Vector4<T> left, T right)
{
  return { .x = left.x * right,
           .y = left.y * right,
           .z = left.z * right,
           .w = left.w * right };
}


template<ConceptMathType T>
Vector4<T> Divide(Vector4<T> left, T right)
{
  if (right == 0.f)
  {
    return { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };
  }
  T div = 1 / right;
  return { .x = left.x * div,
           .y = left.y * div,
           .z = left.z * div,
           .w = left.w * div };
}


template<ConceptMathType T>
Vector4<T> Add(Vector4<T> left, Vector4<T> right)
{
  return { .x = left.x + right.x,
           .y = left.y + right.y,
           .z = left.z + right.z,
           .w = left.w + right.w };
}


template<ConceptMathType T>
Vector4<T> Subtract(Vector4<T> left, Vector4<T> right)
{
  return { .x = left.x - right.x,
           .y = left.y - right.y,
           .z = left.z - right.z,
           .w = left.w - right.w };
}


template<ConceptMathType T>
Vector4<T> Multiply(Vector4<T> left, Vector4<T> right)
{
  return { .x = left.x * right.x,
           .y = left.y * right.y,
           .z = left.z * right.z,
           .w = left.w * right.w };
}


template<ConceptMathType T>
Vector4<T> Divide(Vector4<T> left, Vector4<T> right)
{
  return { .x = right.x != 0.f ? left.x / right.x : FLT_MAX,
           .y = right.y != 0.f ? left.y / right.y : FLT_MAX,
           .z = right.z != 0.f ? left.z / right.z : FLT_MAX,
           .w = right.w != 0.f ? left.w / right.w : FLT_MAX };
}


template<ConceptMathType T>
b8 Compare(Vector4<T> left, Vector4<T> right)
{
  if (left.x == right.x and left.y == right.y and left.z == right.z and left.w == right.w) {
    return UTRUE;
  }
  return UFALSE;
}


template<ConceptMathType T>
b32 operator==(Vector4<T> left, Vector4<T> right) {
  return Compare(left, right);
}


template<ConceptMathType T>
b32 operator!=(Vector4<T> left, Vector4<T> right) {
  return not Compare(left, right);
}


template<ConceptMathType T>
Vector4<T> operator+(Vector4<T> left, Vector4<T> right) {
  return Add(left, right);
}


template<ConceptMathType T>
Vector4<T> operator-(Vector4<T> left, Vector4<T> right) {
  return Subtract(left, right);
}


template<ConceptMathType T>
Vector4<T> operator*(Vector4<T> left, Vector4<T> right) {
  return Multiply(left, right);
}


template<ConceptMathType T>
Vector4<T> operator/(Vector4<T> left, Vector4<T> right) {
  return Divide(left, right);
}


template<ConceptMathType T>
Vector4<T> operator+(Vector4<T> left, T right) {
  return Add(left, right);
}


template<ConceptMathType T>
Vector4<T> operator-(Vector4<T> left, T right) {
  return Subtract(left, right);
}


template<ConceptMathType T>
Vector4<T> operator*(Vector4<T> left, T right) {
  return Multiply(left, right);
}


template<ConceptMathType T>
Vector4<T> operator/(Vector4<T> left, T right) {
  return Divide(left, right);
}


template<ConceptMathType T>
Vector4<T> operator+(T left, Vector4<T> right) {
return Add(right, left);
}


template<ConceptMathType T>
Vector4<T> operator-(T left, Vector4<T> right) {
  return {
    .x = left - right.x,
    .y = left - right.y,
    .z = left - right.z,
    .w = left - right.w
  };
}


template<ConceptMathType T>
Vector4<T> operator*(T left, Vector4<T> right) {
  return Multiply(right, left);
}


template<ConceptMathType T>
Vector4<T> operator/(T left, Vector4<T> right) {
  return { right.x != 0.f ? left / right.x : FLT_MAX,
           right.y != 0.f ? left / right.y : FLT_MAX,
           right.z != 0.f ? left / right.z : FLT_MAX,
           right.w != 0.f ? left / right.w : FLT_MAX };
}


}


#endif //UNCANNYENGINE_VECTOR4_H
