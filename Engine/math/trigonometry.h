
#ifndef UNCANNYENGINE_TRIGONOMETRY_H
#define UNCANNYENGINE_TRIGONOMETRY_H


#include <utilities/Variables.h>
#include <cmath>


namespace uncanny
{


inline f32 deg2rad(f32 degrees) {
  return degrees * 0.01745329251f;
}


inline f32 rad2deg(f32 radians) {
  return radians * 57.29577951308f;
}


inline f32 sine(f32 radians) {
  return sin(radians);
}


inline f32 cosine(f32 radians) {
  return cos(radians);
}


}


#endif //UNCANNYENGINE_TRIGONOMETRY_H
