
#include "Utilities.h"
#include <utilities/Logger.h>


#define U_NV_DRIVER_VERSION_MAJOR(version)  (((version) >> 22) & 0x3ff)
#define U_NV_DRIVER_VERSION_MINOR(version)  (((version) >> 14) & 0x0ff)
#define U_NV_DRIVER_VERSION_PATCH(version)  (((version) >> 6) & 0x0ff)


namespace uncanny::vkf {


void AssertResultVulkan(VkResult result) {
  if (result == VK_SUCCESS) {
    return;
  }

  UERROR("{}", retrieveInfoAboutVkResult(result));
  __debugbreak();
}


FDriverVersionInfo decodeDriverVersionVulkan(u32 version, u32 vendorID) {
  UTRACE("Trying to decode driver version for vendorID: {}", vendorID);
  if (vendorID == 4318) { // NVIDIA
    FDriverVersionInfo info{};
    info.major = U_NV_DRIVER_VERSION_MAJOR(version);
    info.minor = U_NV_DRIVER_VERSION_MINOR(version);
    info.patch = U_NV_DRIVER_VERSION_PATCH(version);
    return info;
  }
  if (vendorID == 4098) { // AMD
    FDriverVersionInfo info{};
    info.major = VK_API_VERSION_MAJOR(version);
    info.minor = VK_API_VERSION_MINOR(version);
    info.patch = VK_API_VERSION_PATCH(version);
    return info;
  }
  return {};
}


const char* retrieveInfoAboutVkResult(VkResult result) {
  if (result == VK_NOT_READY) {
    return "VK_NOT_READY A fence or query has not yet completed";
  }
  if (result == VK_TIMEOUT) {
    return "VK_TIMEOUT A wait operation has not completed in the specified time";
  }
  if (result == VK_EVENT_SET) {
    return "VK_EVENT_SET An event is signaled";
  }
  if (result == VK_EVENT_RESET) {
    return "VK_EVENT_RESET An event is unsignaled";
  }
  if (result == VK_INCOMPLETE) {
    return "VK_INCOMPLETE A return array was too small for the result";
  }
  if (result == VK_PIPELINE_COMPILE_REQUIRED) {
    return "VK_PIPELINE_COMPILE_REQUIRED A requested pipeline creation would have required "
           "compilation, but the application requested compilation to not be performed";
  }
  if (result == VK_ERROR_OUT_OF_HOST_MEMORY) {
    return "VK_ERROR_OUT_OF_HOST_MEMORY A host memory allocation has failed";
  }
  if (result == VK_ERROR_OUT_OF_DEVICE_MEMORY) {
    return "VK_ERROR_OUT_OF_DEVICE_MEMORY A device memory allocation has failed";
  }
  if (result == VK_ERROR_INITIALIZATION_FAILED) {
    return "VK_ERROR_INITIALIZATION_FAILED Initialization of an object could not be completed for"
           " implementation-specific reasons";
  }
  if (result == VK_ERROR_DEVICE_LOST) {
    return "VK_ERROR_DEVICE_LOST The logical or physical device has been lost";
  }
  if (result == VK_ERROR_MEMORY_MAP_FAILED) {
    return "VK_ERROR_MEMORY_MAP_FAILED Mapping of a memory object has failed";
  }
  if (result == VK_ERROR_LAYER_NOT_PRESENT) {
    return "VK_ERROR_LAYER_NOT_PRESENT A requested layer is not present or could not be loaded";
  }
  if (result == VK_ERROR_EXTENSION_NOT_PRESENT) {
    return "VK_ERROR_EXTENSION_NOT_PRESENT A requested extension is not supported";
  }
  if (result == VK_ERROR_FEATURE_NOT_PRESENT) {
    return "VK_ERROR_FEATURE_NOT_PRESENT A requested feature is not supported";
  }
  if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
    return "VK_ERROR_INCOMPATIBLE_DRIVER The requested version of Vulkan is not supported by the "
           "driver or is otherwise incompatible for implementation-specific reasons";
  }
  if (result == VK_ERROR_TOO_MANY_OBJECTS) {
    return "VK_ERROR_TOO_MANY_OBJECTS Too many objects of the type have already been created";
  }
  if (result == VK_ERROR_FORMAT_NOT_SUPPORTED) {
    return "VK_ERROR_FORMAT_NOT_SUPPORTED A requested format is not supported on this device";
  }
  if (result == VK_ERROR_FRAGMENTED_POOL) {
    return "VK_ERROR_FRAGMENTED_POOL A pool allocation has failed due to fragmentation of the "
           "pool\'s memory";
  }
  if (result == VK_ERROR_OUT_OF_POOL_MEMORY) {
    return "VK_ERROR_OUT_OF_POOL_MEMORY A pool memory allocation has failed";
  }
  if (result == VK_ERROR_INVALID_EXTERNAL_HANDLE) {
    return "VK_ERROR_INVALID_EXTERNAL_HANDLE An external handle is not a valid handle of the "
           "specified type";
  }
  if (result == VK_ERROR_FRAGMENTATION) {
    return "VK_ERROR_FRAGMENTATION A descriptor pool creation has failed due to fragmentation";
  }
  if (result == VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS) {
    return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS A buffer creation or memory allocation failed"
           " because the requested address is not available";
  }
  if (result == VK_ERROR_UNKNOWN) {
    return "VK_ERROR_UNKNOWN An unknown error has occurred; either the application has provided "
           "invalid input, or an implementation failure has occurred";
  }

  return "TOTALLY UNKNOWN ERROR, even Vulkan Docs does not have it defined!";
}


}
