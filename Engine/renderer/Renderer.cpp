
#include "Renderer.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRenderer::init() {
  UFATAL("should not be used!");
  return UFALSE;
}


void FRenderer::terminate() {
  UFATAL("should not be used!");
}


b32 FRenderer::parseSceneForRendering(const FRenderSceneConfiguration& sceneConfig) {
  UFATAL("should not be used!");
  return UFALSE;
}


b32 FRenderer::updateSceneDuringRendering(const uncanny::FRenderSceneConfiguration& sceneConfig) {
  UFATAL("should not be used!");
  return UFALSE;
}


b32 FRenderer::closeScene() {
  UFATAL("should not be used!");
  return UFALSE;
}


b32 FRenderer::prepareStateForRendering() {
  UFATAL("should not be used!");
  return UFALSE;
}


ERendererState FRenderer::prepareFrame() {
  UFATAL("should not be used!");
  return ERendererState::NONE;
}


b32 FRenderer::submitFrame() {
  UFATAL("should not be used!");
  return UFALSE;
}


b32 FRenderer::endFrame() {
  UFATAL("should not be used!");
  return UFALSE;
}


}
