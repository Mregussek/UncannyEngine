
#ifndef UNCANNYENGINE_VECTOR2_H
#define UNCANNYENGINE_VECTOR2_H


#include "Types.h"


namespace uncanny::math
{


template<ConceptMathType T>
Vector2<T> Create(T x, T y)
{
  return { .x = x, .y = y };
}


template<ConceptMathType T>
Vector2<T> Add(Vector2<T> left, T right)
{
  return { .x = left.x + right,
           .y = left.y + right };
}


template<ConceptMathType T>
Vector2<T> Subtract(Vector2<T> left, T right)
{
  return { .x = left.x - right,
           .y = left.y - right };
}


template<ConceptMathType T>
Vector2<T> Multiply(Vector2<T> left, T right)
{
  return { .x = left.x * right,
           .y = left.y * right, };
}


template<ConceptMathType T>
Vector2<T> Divide(Vector2<T> left, T right)
{
  if (right == 0.f)
  {
    return { FLT_MAX, FLT_MAX };
  }
  T div = 1 / right;
  return { .x = left.x * div,
           .y = left.y * div };
}


template<ConceptMathType T>
Vector2<T> Add(Vector2<T> left, Vector2<T> right)
{
  return { .x = left.x + right.x,
           .y = left.y + right.y };
}


template<ConceptMathType T>
Vector2<T> Subtract(Vector2<T> left, Vector2<T> right)
{
  return { .x = left.x - right.x,
           .y = left.y - right.y };
}


template<ConceptMathType T>
Vector2<T> Multiply(Vector2<T> left, Vector2<T> right)
{
  return { .x = left.x * right.x,
           .y = left.y * right.y };
}


template<ConceptMathType T>
Vector2<T> Divide(Vector2<T> left, Vector2<T> right)
{
  return { .x = right.x != 0.f ? left.x / right.x : FLT_MAX,
           .y = right.y != 0.f ? left.y / right.y : FLT_MAX};
}


template<ConceptMathType T>
b8 Compare(Vector2<T> left, Vector2<T> right)
{
  if (left.x == right.x and left.y == right.y) {
    return UTRUE;
  }
  return UFALSE;
}


template<ConceptMathType T>
b32 operator==(Vector2<T> left, Vector2<T> right) {
  return Compare(left, right);
}


template<ConceptMathType T>
b32 operator!=(Vector2<T> left, Vector2<T> right) {
  return not Compare(left, right);
}


template<ConceptMathType T>
Vector2<T> operator+(Vector2<T> left, Vector2<T> right) {
  return Add(left, right);
}


template<ConceptMathType T>
Vector2<T> operator-(Vector2<T> left, Vector2<T> right) {
  return Subtract(left, right);
}


template<ConceptMathType T>
Vector2<T> operator*(Vector2<T> left, Vector2<T> right) {
  return Multiply(left, right);
}


template<ConceptMathType T>
Vector2<T> operator/(Vector2<T> left, Vector2<T> right) {
  return Divide(left, right);
}


template<ConceptMathType T>
Vector2<T> operator+(Vector2<T> left, T right) {
  return Add(left, right);
}


template<ConceptMathType T>
Vector2<T> operator-(Vector2<T> left, T right) {
  return Subtract(left, right);
}


template<ConceptMathType T>
Vector2<T> operator*(Vector2<T> left, T right) {
  return Multiply(left, right);
}


template<ConceptMathType T>
Vector2<T> operator/(Vector2<T> left, T right) {
  return Divide(left, right);
}


template<ConceptMathType T>
Vector2<T> operator+(T left, Vector2<T> right) {
  return Add(right, left);
}


template<ConceptMathType T>
Vector2<T> operator-(T left, Vector2<T> right) {
  return {
      .x = left - right.x,
      .y = left - right.y
  };
}


template<ConceptMathType T>
Vector2<T> operator*(T left, Vector2<T> right) {
  return Multiply(right, left);
}


template<ConceptMathType T>
Vector2<T> operator/(T left, Vector2<T> right) {
  return { right.x != 0.f ? left / right.x : FLT_MAX,
           right.y != 0.f ? left / right.y : FLT_MAX };
}


}


#endif //UNCANNYENGINE_VECTOR2_H
