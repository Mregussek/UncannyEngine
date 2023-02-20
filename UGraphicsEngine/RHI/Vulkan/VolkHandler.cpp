
#define VOLK_IMPLEMENTATION
#include <volk.h>

#include "Utilities.h"
#include "Instance.h"
#include "VolkHandler.h"


namespace uncanny::vulkan {


void FVolkHandler::Create() {
  VkResult result = volkInitialize();
  AssertVkAndThrow(result, "Could not initialize volk library");
}


void FVolkHandler::LoadInstance(const FInstance& instance) {
  volkLoadInstance(instance.GetHandle());
}


void FVolkHandler::Destroy() {

}


}
