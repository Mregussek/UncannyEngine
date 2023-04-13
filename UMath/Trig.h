
#ifndef UNCANNYENGINE_TRIG_H
#define UNCANNYENGINE_TRIG_H


#include "Types.h"


namespace uncanny::math
{


template<ConceptMathType T>
T radians(T degrees)
{
  return degrees * 0.01745329251f;
}


template<ConceptMathType T>
T degrees(T radians)
{
  return radians * 57.29577951308f;
}


}


#endif //UNCANNYENGINE_TRIG_H
