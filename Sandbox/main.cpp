
#include <window/Window.h>
#include <window/WindowFactory.h>
#include <math/mat4.h>
#include <math/vec3.h>
#include <math/vec4.h>
#include <math/trigonometry.h>
#include <renderer/Mesh.h>
#include <renderer/Factory.h>
#include <renderer/Camera.h>
#include <utilities/Logger.h>
#undef near
#undef far


using namespace uncanny;


auto main() -> i32 {
  // Utilities setup
  FLogger::init(FLogger::sLoggerPtr);
  FRendererFactory rendererFactory{};
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
  FRendererSpecification rendererSpecification{};
  rendererSpecification.pWindow = pWindow;
  rendererSpecification.appVersion = UENGINE_MAKE_VERSION(0, 0, 0);
  rendererSpecification.engineVersion = (u32)EEngineVersion::LATEST;
  rendererSpecification.appName = "Uncanny Sandbox";

  FRenderer* pRenderer{ rendererFactory.create<ERenderLibrary::VULKAN>() };
  b32 initializedRenderer{ pRenderer->init(rendererSpecification) };
  if (not initializedRenderer) {
    pWindow->close();
    pRenderer->terminate();
    pWindow->terminate();
    return -1;
  }

  FCameraSpecification cameraSpecification{};
  cameraSpecification.position = { 2.f, -0.5f, 2.f };
  cameraSpecification.front = { 0.f, 0.f, 0.f };
  cameraSpecification.worldUp = { 0.f, 1.f, 0.f };
  cameraSpecification.fieldOfView = 45.f;
  cameraSpecification.aspectRatio =
      (f32)windowSpecification.width / (f32)windowSpecification.height;
  cameraSpecification.near = 0.1f;
  cameraSpecification.far = 10.f;
  cameraSpecification.yaw = -90.f;
  cameraSpecification.pitch = 0.f;
  cameraSpecification.movementSpeed = 2.5f;
  cameraSpecification.sensitivity = 1.f;
  cameraSpecification.zoom = 45.f;

  FCamera camera{};
  camera.init(cameraSpecification);

  FMeshQuads meshQuads{};
  meshQuads.transformLocal = mat4::rotation(deg2rad(45.f), { 0.f, 0.f, 1.f });

  FRenderSceneConfiguration renderSceneConfig{};
  renderSceneConfig.pMesh = &meshQuads;
  renderSceneConfig.pCamera = &camera;

  b32 rendererParsedScene{ pRenderer->parseSceneForRendering(renderSceneConfig) };
  if (not rendererParsedScene) {
    pWindow->close();
    pRenderer->terminate();
    pWindow->terminate();
    return -1;
  }

  b32 rendererPrepared{ pRenderer->prepareStateForRendering() };
  if (not rendererPrepared) {
    pWindow->close();
    pRenderer->closeScene();
    pRenderer->terminate();
    pWindow->terminate();
    return -1;
  }

  // Running loop
  while(pWindow->isNotGoingToClose()) {
    pWindow->pollEvents();
    if (pWindow->isMinimized()) {
      continue;
    }

    FRendererPrepareFrameSpecification prepareFrameSpecs{};
    prepareFrameSpecs.pWindow = pWindow;

    ERendererState renderState{ pRenderer->prepareFrame(prepareFrameSpecs) };
    if (renderState != ERendererState::RENDERING) {
      continue;
    }

    meshQuads.transformLocal = mat4::rotation(glfwGetTime(), { 0.f, 0.f, 1.f });
    b32 updatedRenderScene{ pRenderer->updateSceneDuringRendering(renderSceneConfig) };
    if (not updatedRenderScene) {
      break;
    }

    pRenderer->submitFrame();
    pRenderer->endFrame();
  }

  // Terminating everything
  pRenderer->closeScene();
  pRenderer->terminate();
  pWindow->terminate();

  return 0;
}
