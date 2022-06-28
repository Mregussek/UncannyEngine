
#include "vec4.h"


namespace uncanny
{


vec4::vec4(f32 _x, f32 _y, f32 _z, f32 _w) :
  x(_x),
  y(_y),
  z(_z),
  w(_w)
{ }


b32 vec4::compare(vec4 a, vec4 b) {
  if (a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w) {
    return UTRUE;
  }
  return UFALSE;
}


b32 vec4::operator==(vec4 other) const {
  return compare(*this, other);
}


b32 vec4::operator!=(vec4 other) const {
  return ~compare(*this, other);
}


vec4 operator+(vec4 left, vec4 right) {
  return left.add(right);
}


vec4 operator-(vec4 left, vec4 right) {
  return left.subtract(right);
}


vec4 operator*(vec4 left, vec4 right) {
  return left.multiply(right);
}


vec4 operator/(vec4 left, vec4 right) {
  return left.divide(right);
}


vec4 operator+(vec4 left, f32 right) {
  return left.add(right);
}


vec4 operator-(vec4 left, f32 right) {
  return left.subtract(right);
}


vec4 operator*(vec4 left, f32 right) {
  return left.multiply(right);
}


vec4 operator/(vec4 left, f32 right) {
  return left.divide(right);
}


vec4 operator+(f32 left, vec4 right) {
  return right.add(left);
}


vec4 operator-(f32 left, vec4 right) {
  return {
    left - right.x,
    left - right.y,
    left - right.z,
    left - right.w
  };
}


vec4 operator*(f32 left, vec4 right) {
  return right.multiply(left);
}


vec4 operator/(f32 left, vec4 right) {
  return {
      right.x != 0.f ? left / right.x : FLT_MAX,
      right.y != 0.f ? left / right.y : FLT_MAX,
      right.z != 0.f ? left / right.z : FLT_MAX,
      right.w != 0.f ? left / right.w : FLT_MAX
  };
}


}
