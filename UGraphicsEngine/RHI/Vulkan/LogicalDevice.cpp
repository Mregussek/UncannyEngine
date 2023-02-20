
#include "LogicalDevice.h"
#include "Utilities.h"


namespace uncanny::vulkan {


class FLogicalDeviceCreator {
public:

  void AddQueueFamilyToDeviceQueueCreateInfo(FQueueFamilyIndex queueFamilyIndex, FQueueIndex queueIndex);

  [[nodiscard]] const std::vector<VkDeviceQueueCreateInfo>& GetDeviceQueueCreateInfoVector() const { return m_DeviceQueueCreateInfoVector; }

private:

  [[nodiscard]] std::pair<b8, u32> IsQueueFamilyPresent(FQueueFamilyIndex queueFamilyIndex) const;

  std::vector<VkDeviceQueueCreateInfo> m_DeviceQueueCreateInfoVector{};
  f32 m_QueuePriority{ 1.f };

};


void FLogicalDevice::Create(const FLogicalDeviceAttributes& attributes, VkPhysicalDevice vkPhysicalDevice) {
  m_Attributes = attributes;

  FLogicalDeviceCreator creator{};
  creator.AddQueueFamilyToDeviceQueueCreateInfo(m_Attributes.GetGraphicsQueueFamilyIndex(),
                                                m_Attributes.GetGraphicsQueueIndex());
  creator.AddQueueFamilyToDeviceQueueCreateInfo(m_Attributes.GetPresentQueueFamilyIndex(),
                                                m_Attributes.GetPresentQueueIndex());
  creator.AddQueueFamilyToDeviceQueueCreateInfo(m_Attributes.GetTransferQueueFamilyIndex(),
                                                m_Attributes.GetTransferQueueIndex());

  const auto& requiredExtensions = m_Attributes.GetRequiredExtensions();
  const auto& deviceQueueCreateInfo = creator.GetDeviceQueueCreateInfoVector();
  const auto& deviceFeatures = m_Attributes.GetDeviceFeatures();

  VkDeviceCreateInfo createInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.queueCreateInfoCount = deviceQueueCreateInfo.size();
  createInfo.pQueueCreateInfos = deviceQueueCreateInfo.data();
  createInfo.enabledLayerCount = 0;             // deprecated!
  createInfo.ppEnabledLayerNames = nullptr;    // deprecated!
  createInfo.enabledExtensionCount = requiredExtensions.size();
  createInfo.ppEnabledExtensionNames = requiredExtensions.data();
  createInfo.pEnabledFeatures = &deviceFeatures;

  VkResult result = vkCreateDevice(vkPhysicalDevice, &createInfo, nullptr, &m_Device);
  AssertVkAndThrow(result);
}


void FLogicalDevice::Destroy() {
  if (m_Device != VK_NULL_HANDLE) {
    vkDeviceWaitIdle(m_Device);
    vkDestroyDevice(m_Device, nullptr);
  }
}



void FLogicalDeviceCreator::AddQueueFamilyToDeviceQueueCreateInfo(FQueueFamilyIndex queueFamilyIndex,
                                                                  FQueueIndex queueIndex) {
  auto [isPresent, index] = IsQueueFamilyPresent(queueFamilyIndex);
  if (!isPresent) {
    VkDeviceQueueCreateInfo &createInfo = m_DeviceQueueCreateInfoVector.emplace_back();
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.queueFamilyIndex = queueFamilyIndex;
    createInfo.queueCount = 1;
    createInfo.pQueuePriorities = &m_QueuePriority;
    return;
  }

  if (queueIndex > m_DeviceQueueCreateInfoVector.at(index).queueCount - 1) {
    m_DeviceQueueCreateInfoVector.at(index).queueCount += 1;
  }
}


std::pair<b8, u32> FLogicalDeviceCreator::IsQueueFamilyPresent(FQueueFamilyIndex queueFamilyIndex) const {
  auto it = std::ranges::find_if(m_DeviceQueueCreateInfoVector, [queueFamilyIndex](const VkDeviceQueueCreateInfo& createInfo) -> b32 {
    return queueFamilyIndex == createInfo.queueFamilyIndex;
  });
  if (it != m_DeviceQueueCreateInfoVector.end()) {
    return { UTRUE, std::distance(m_DeviceQueueCreateInfoVector.begin(), it) };
  }

  return { UFALSE,  UUNUSED };
}


}
