
#include "vec3.h"


namespace uncanny
{


vec3::vec3(f32 _x, f32 _y, f32 _z) :
  x(_x),
  y(_y),
  z(_z)
{ }


vec3 vec3::cross(vec3 a, vec3 b) {
  return {
    a.y * b.z - b.y * a.z,
    a.z * b.x - b.z * a.x,
    a.x * b.y - b.x * a.y
  };
}


f32 vec3::dot(vec3 a, vec3 b) {
  const vec3 tmp{ a * b };
  return tmp.x + tmp.y + tmp.z;
}


f32 vec3::length(vec3 v) {
  return sqrt(vec3::dot(v, v));
}


vec3 vec3::normalize(vec3 v) {
  const f32 magnitude{ length(v) };
  if (magnitude == 0.f) {
    static_assert(true, "vec3::normalize(magnitude=0.f) - cannot divide by zero!");
  }
  const f32 inverseMagnitude{ 1.f / magnitude };
  return v * inverseMagnitude;
}


b32 vec3::compare(vec3 a, vec3 b) {
  if (a.x == b.x && a.y == b.y && a.z == b.z) {
    return UTRUE;
  }
  return UFALSE;
}


b32 vec3::compare(vec3 a, vec3 b, f32 margin) {
  if (
      a.x - margin <= b.x && b.x <= a.x + margin &&
      a.y - margin <= b.y && b.y <= a.y + margin &&
      a.z - margin <= b.z && b.z <= a.z + margin
      ) {
    return UTRUE;
  }
  return UFALSE;
}


b32 vec3::operator==(vec3 other) const {
  return compare(*this, other);
}


b32 vec3::operator!=(vec3 other) const {
  return !compare(*this, other);
}


vec3 operator+(vec3 left, vec3 right) {
  return left.add(right);
}


vec3 operator-(vec3 left, vec3 right) {
  return left.subtract(right);
}


vec3 operator*(vec3 left, vec3 right) {
  return left.multiply(right);
}


vec3 operator/(vec3 left, vec3 right) {
  return left.divide(right);
}


vec3 operator+(vec3 left, f32 right) {
  return left.add(right);
}


vec3 operator-(vec3 left, f32 right) {
  return left.subtract(right);
}


vec3 operator*(vec3 left, f32 right) {
  return left.multiply(right);
}


vec3 operator/(vec3 left, f32 right) {
  return left.divide(right);
}


vec3 operator+(f32 left, vec3 right) {
  return right.add(left);
}


vec3 operator-(f32 left, vec3 right) {
  return {
    left - right.x,
    left - right.y,
    left - right.z
  };
}


vec3 operator*(f32 left, vec3 right) {
  return right.multiply(left);
}


vec3 operator/(f32 left, vec3 right) {
  return {
      right.x != 0.f ? left / right.x : FLT_MAX,
      right.y != 0.f ? left / right.y : FLT_MAX,
      right.z != 0.f ? left / right.z : FLT_MAX
  };
}


}
