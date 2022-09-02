
#include <window/Window.h>
#include <window/WindowFactory.h>
#include <math/mat4.h>
#include <math/vec3.h>
#include <math/vec4.h>
#include <renderer/Mesh.h>
#include <renderer/Context.h>
#include <renderer/Factory.h>
#include <renderer/Camera.h>
#include <utilities/Logger.h>


using namespace uncanny;


auto main() -> i32 {
  // Utilities setup
  FLogger::init(FLogger::sLoggerPtr);
  FRenderContextFactory renderContextFactory{};
  FWindowFactory windowFactory{};

  // Window subsystem configuration
  FWindowConfiguration windowConfiguration{};
  windowConfiguration.resizable = UTRUE;

  FWindowSpecification windowSpecification{};
  windowSpecification.name = "UncannySandbox";
  windowSpecification.width = 720;
  windowSpecification.height = 405;
  windowSpecification.config = windowConfiguration;

  FWindow* pWindow{ windowFactory.create<EWindowLibrary::GLFW>() };
  pWindow->init(windowSpecification);

  // Rendering subsystem configuration
  FRenderContextSpecification renderContextSpecification{};
  renderContextSpecification.pWindow = pWindow;
  renderContextSpecification.pAppName = "Uncanny Sandbox";
  renderContextSpecification.appVersion = UENGINE_MAKE_VERSION(0, 0, 1);
  renderContextSpecification.engineVersion = (u32)EEngineVersion::LATEST;

  FRenderContext* pRenderContext{ renderContextFactory.create<ERenderLibrary::VULKAN>() };
  b32 initializedContext{ pRenderContext->init(renderContextSpecification) };
  if (not initializedContext) {
    pWindow->close();
  }

  FRenderer* pRenderer{ renderContextFactory.retrieveRendererFrom(pRenderContext) };
  b32 initializedRenderer{ pRenderer->init() };
  if (not initializedRenderer) {
    pWindow->close();
    pRenderer->terminate();
    pRenderContext->terminate();
    pWindow->terminate();
    return -1;
  }

  f32 fieldOfView{ 45.f };
  f32 aspectRatio{ (f32)windowSpecification.width / (f32)windowSpecification.height };

  vec3 eye{ 2.f, 2.f, 2.f };
  vec3 center{ 0.f, 0.f, 0.f };
  vec3 up{ 0.f, 0.f, 1.f };

  FCamera camera{};
  camera.mMatrixMVP =
      mat4::perspective(fieldOfView, aspectRatio, 0.1f, 10.0f) *
      mat4::lookAt(eye, center, up) *
      mat4::rotation(45.f, { 0.f, 0.f, 1.f });
  FMeshTriangle meshTriangle{};

  FRenderSceneConfiguration renderSceneConfig{};
  renderSceneConfig.pMesh = &meshTriangle;
  renderSceneConfig.pCamera = &camera;

  b32 rendererParsedScene{ pRenderer->parseSceneForRendering(renderSceneConfig) };
  if (not rendererParsedScene) {
    pWindow->close();
    pRenderer->terminate();
    pRenderContext->terminate();
    pWindow->terminate();
    return -1;
  }

  b32 rendererPrepared{ pRenderer->prepareStateForRendering() };
  if (not rendererPrepared) {
    pWindow->close();
    pRenderer->closeScene();
    pRenderer->terminate();
    pRenderContext->terminate();
    pWindow->terminate();
    return -1;
  }

  // Running loop
  while(pWindow->isNotGoingToClose()) {
    pWindow->pollEvents();
    if (pWindow->isMinimized()) {
      continue;
    }

    ERendererState renderState{ pRenderer->prepareFrame() };
    if (renderState == ERendererState::SURFACE_MINIMIZED) {
      continue;
    }

    if (renderState == ERendererState::RENDERING) {
      pRenderer->submitFrame();
      pRenderer->endFrame();
    }
  }

  // Terminating everything
  pRenderer->closeScene();
  pRenderer->terminate();
  pRenderContext->terminate();
  pWindow->terminate();

  return 0;
}
