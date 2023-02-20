
#ifndef UNCANNYENGINE_INSTANCEPROPERTIES_H
#define UNCANNYENGINE_INSTANCEPROPERTIES_H


#include <volk.h>
#include <vector>
#include <string_view>
#include "UTools/UTypes.h"


namespace uncanny::vulkan {


/*
 * @brief FInstanceProperties is a helper class for FInstance. It is responsible for validation of all
 * requested instance layers and extensions with actual available ones. Afterwards object of this class
 * is handed to the FInstance.Create().
 */
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
