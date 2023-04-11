
#ifndef UNCANNYENGINE_SHADER_H
#define UNCANNYENGINE_SHADER_H


#include <volk.h>
#include <vector>
#include <UTools/UTypes.h>


namespace uncanny::vulkan
{


class FShader
{
public:

  explicit FShader(VkDevice vkDevice);

  ~FShader();

  void Create(const std::vector<u32>& spvSource);

  void Destroy();

  [[nodiscard]] VkShaderModule GetHandle() const { return m_ShaderModule; }

private:

  VkShaderModule m_ShaderModule{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_SHADER_H
