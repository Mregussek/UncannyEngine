
#ifndef UNCANNYENGINE_VECTOR3_H
#define UNCANNYENGINE_VECTOR3_H


#include "Types.h"


namespace uncanny::math
{


template<ConceptMathType T>
Vector3<T> Create(T x, T y, T z)
{
  return { .x = x, .y = y, .z = z };
}


template<ConceptMathType T>
Vector3<T> Add(Vector3<T> left, T right)
{
  return { .x = left.x + right,
           .y = left.y + right,
           .z = left.z + right };
}


template<ConceptMathType T>
Vector3<T> Subtract(Vector3<T> left, T right)
{
  return { .x = left.x - right,
           .y = left.y - right,
           .z = left.z - right };
}


template<ConceptMathType T>
Vector3<T> Multiply(Vector3<T> left, T right)
{
  return { .x = left.x * right,
           .y = left.y * right,
           .z = left.z * right };
}


template<ConceptMathType T>
Vector3<T> Divide(Vector3<T> left, T right)
{
  if (right == 0.f)
  {
    return { FLT_MAX, FLT_MAX, FLT_MAX };
  }
  T div = 1 / right;
  return { .x = left.x * div,
           .y = left.y * div,
           .z = left.z * div };
}


template<ConceptMathType T>
Vector3<T> Add(Vector3<T> left, Vector3<T> right)
{
  return { .x = left.x + right.x,
           .y = left.y + right.y,
           .z = left.z + right.z };
}


template<ConceptMathType T>
Vector3<T> Subtract(Vector3<T> left, Vector3<T> right)
{
  return { .x = left.x - right.x,
           .y = left.y - right.y,
           .z = left.z - right.z };
}


template<ConceptMathType T>
Vector3<T> Multiply(Vector3<T> left, Vector3<T> right)
{
  return { .x = left.x * right.x,
           .y = left.y * right.y,
           .z = left.z * right.z };
}


template<ConceptMathType T>
Vector3<T> Divide(Vector3<T> left, Vector3<T> right)
{
  return { .x = right.x == 0.f ? left.x / right.x : FLT_MAX,
           .y = right.y == 0.f ? left.y / right.y : FLT_MAX,
           .z = right.z == 0.f ? left.z / right.z : FLT_MAX };
}


template<ConceptMathType T>
Vector3<T> CrossProduct(Vector3<T> left, Vector3<T> right)
{
  return { left.y * right.z - right.y * left.z,
           left.z * right.x - right.z * left.x,
           left.x * right.y - right.x * left.y };
}


template<ConceptMathType T>
T DotProduct(Vector3<T> left, Vector3<T> right)
{
  Vector3<T> tmp{ Multiply(left, right) };
  return tmp.x + tmp.y + tmp.z;
}


template<ConceptMathType T>
T Length(Vector3<T> vec)
{
  return (T)std::sqrt(DotProduct(vec, vec));
}


template<ConceptMathType T>
Vector3<T> Normalize(Vector3<T> vec)
{
  T magnitude{ Length(vec) };
  if (magnitude == 0.f)
  {
    return { FLT_MAX, FLT_MAX, FLT_MAX };
  }
  T inverseMagnitude{ 1.f / magnitude };
  return Multiply(vec, inverseMagnitude);
}


template<ConceptMathType T>
b8 Compare(Vector3<T> left, Vector3<T> right)
{
  if (left.x == right.x && left.y == right.y && left.z == right.z) {
    return UTRUE;
  }
  return UFALSE;
}


template<ConceptMathType T>
b32 operator==(Vector3<T> left, Vector3<T> right) {
  return Compare(left, right);
}


template<ConceptMathType T>
b32 operator!=(Vector3<T> left, Vector3<T> right) {
  return not Compare(left, right);
}


template<ConceptMathType T>
Vector3<T> operator+(Vector3<T> left, Vector3<T> right) {
  return Add(left, right);
}


template<ConceptMathType T>
Vector3<T> operator-(Vector3<T> left, Vector3<T> right) {
  return Subtract(left, right);
}


template<ConceptMathType T>
Vector3<T> operator*(Vector3<T> left, Vector3<T> right) {
  return Multiply(left, right);
}


template<ConceptMathType T>
Vector3<T> operator/(Vector3<T> left, Vector3<T> right) {
  return Divide(left, right);
}


template<ConceptMathType T>
Vector3<T> operator+(Vector3<T> left, T right) {
  return Add(left, right);
}


template<ConceptMathType T>
Vector3<T> operator-(Vector3<T> left, T right) {
  return Subtract(left, right);
}


template<ConceptMathType T>
Vector3<T> operator*(Vector3<T> left, T right) {
  return Multiply(left, right);
}


template<ConceptMathType T>
Vector3<T> operator/(Vector3<T> left, T right) {
  return Divide(left, right);
}


template<ConceptMathType T>
Vector3<T> operator+(T left, Vector3<T> right) {
  return Add(right, left);
}


template<ConceptMathType T>
Vector3<T> operator-(T left, Vector3<T> right) {
  return {
      left - right.x,
      left - right.y,
      left - right.z
  };
}


template<ConceptMathType T>
Vector3<T> operator*(T left, Vector3<T> right) {
  return Multiply(right, left);
}


template<ConceptMathType T>
Vector3<T> operator/(T left, Vector3<T> right) {
  return { right.x != 0.f ? left / right.x : FLT_MAX,
           right.y != 0.f ? left / right.y : FLT_MAX,
           right.z != 0.f ? left / right.z : FLT_MAX };
}


template<ConceptMathType T>
Vector3<T> operator+=(Vector3<T> left, Vector3<T> right)
{
  return Add(left, right);
}


template<ConceptMathType T>
Vector3<T> operator-=(Vector3<T> left, Vector3<T> right)
{
  return Subtract(left, right);
}


template<ConceptMathType T>
Vector3<T> operator*=(Vector3<T> left, Vector3<T> right)
{
  return Multiply(left, right);
}


template<ConceptMathType T>
Vector3<T> operator/=(Vector3<T> left, Vector3<T> right)
{
  return Divide(left, right);
}


}


#endif //UNCANNYENGINE_VECTOR3_H
