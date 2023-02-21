
#define VOLK_IMPLEMENTATION
#include "UGraphicsEngine/3rdparty/volk/volk.h"

#include "Utilities.h"
#include "Instance.h"
#include "VolkHandler.h"


namespace uncanny::vulkan {


void FVolkHandler::Create() {
  VkResult result = volkInitialize();
  AssertVkAndThrow(result, "Could not initialize volk library");
}


void FVolkHandler::LoadInstance(VkInstance vkInstance) {
  volkLoadInstance(vkInstance);
}


void FVolkHandler::Destroy() {

}


}
