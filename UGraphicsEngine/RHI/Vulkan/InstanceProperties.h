
#ifndef UNCANNYENGINE_INSTANCEPROPERTIES_H
#define UNCANNYENGINE_INSTANCEPROPERTIES_H


#include <volk.h>
#include <vector>
#include <string_view>
#include "UGraphicsEngine/UTypes.h"


namespace uncanny::vulkan {


class FInstanceProperties {
public:

  void Initialize();

  b8 AddLayerName(const char* layerName);
  b8 AddExtensionName(const char* extensionName);

  [[nodiscard]] const std::vector<const char*>& GetRequestedLayers() const noexcept { return m_RequestedLayers; }
  [[nodiscard]] const std::vector<const char*>& GetRequestedExtensions() const noexcept { return m_RequestExtensions; }

private:

  void GatherAvailableLayers();
  void GatherAvailableExtensions();

  [[nodiscard]] b8 IsLayerAvailable(const char* layerName) const;
  [[nodiscard]] b8 IsExtensionAvailable(const char* extensionName) const;


  std::vector<VkLayerProperties> m_AvailableLayerProperties;
  std::vector<VkExtensionProperties> m_AvailableExtensionsProperties;

  std::vector<const char*> m_RequestedLayers;
  std::vector<const char*> m_RequestExtensions;

};


}


#endif //UNCANNYENGINE_INSTANCEPROPERTIES_H
