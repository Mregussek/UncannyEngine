
#include "vec4.h"


namespace uncanny
{


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
  return right.subtract(left);
}


vec4 operator*(f32 left, vec4 right) {
  return right.multiply(left);
}


vec4 operator/(f32 left, vec4 right) {
  return right.divide(left);
}


}
