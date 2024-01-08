
#ifndef UNCANNYENGINE_PHYSICALDEVICE_H
#define UNCANNYENGINE_PHYSICALDEVICE_H


#include <volk.h>
#include "PhysicalDeviceAttributes.h"


namespace uncanny::vulkan
{


/// @brief FPhysicalDevice wrapper class for vulkan handle.
/// @details Initialize method is implemented because VkPhysicalDevice is rather "retrieved" from
/// VkInstance than created and then destroyed. Even I don't like those GetHandle() methods I have decided
/// to put it there, so that every VkPhysicalDevice-dependent wrapper class can store and use it for its
/// creation / destruction. Other operations must not be allowed! Remember about single responsibility rule!
/// @friend As this physical device travels across several classes in RenderDevice I decided to hide
/// Initialize() method from the end user and make friendship with RenderContext so that only RenderContext
/// can manage its lifetime.
class FPhysicalDevice
{
  // I want only RenderContext to access Initialize() method, not more! Only reason for friendship
  friend class FRenderContext;

public:

  FPhysicalDevice() = default;

  FPhysicalDevice(const FPhysicalDevice& other) = delete;
  FPhysicalDevice(FPhysicalDevice&& other) = delete;

  FPhysicalDevice& operator=(const FPhysicalDevice& other) = delete;
  FPhysicalDevice& operator=(FPhysicalDevice&& other) = delete;

// Getters
public:

  [[nodiscard]] VkPhysicalDevice GetHandle() const { return m_PhysicalDevice; }
  [[nodiscard]] const FPhysicalDeviceAttributes& GetAttributes() const { return m_Attributes; }

// Private methods
private:

  void Initialize(VkPhysicalDevice physicalDevice);

// Members
private:

  FPhysicalDeviceAttributes m_Attributes{};
  VkPhysicalDevice m_PhysicalDevice{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_PHYSICALDEVICE_H
