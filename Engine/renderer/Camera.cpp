
#include "Camera.h"
#include <math/vec3.h>
#include <math/trigonometry.h>


namespace uncanny
{


static void updateCameraVectors(const FCameraSpecification& specs, vec3* pOutFront,
                                vec3* pOutRight, vec3* pOutUp);


void FCamera::init(const FCameraSpecification& specs) {
  mSpecs = specs;
  updateCameraVectors(mSpecs, &mSpecs.front, &mRight, &mUp);
}


mat4 FCamera::retrieveMatrixMVP() const {
  return {
    mat4::perspective(mSpecs.fieldOfView, mSpecs.aspectRatio, mSpecs.near, mSpecs.far) *
    mat4::lookAt(mSpecs.position, mSpecs.position + mSpecs.front, mUp)
  };
}


void updateCameraVectors(const FCameraSpecification& specs, vec3* pOutFront, vec3* pOutRight,
                         vec3* pOutUp) {
  const f32 yaw{ specs.yaw };
  const f32 pitch{ specs.pitch };
  const vec3 front{
      cosine(deg2rad(yaw)) * cosine(deg2rad(pitch)),
      sine(deg2rad(pitch)),
      sine(deg2rad(yaw)) * cosine(deg2rad(pitch))
  };
  *pOutFront = vec3::normalize(front);
  *pOutRight = vec3::normalize(vec3::cross(*pOutFront, specs.worldUp));
  *pOutUp = vec3::normalize(vec3::cross(*pOutRight, *pOutFront));
}


}
