
#include "Queue.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"


namespace uncanny::vulkan
{


void FQueue::Initialize(VkQueue queue, FQueueFamilyIndex familyIndex)
{
  m_Queue = queue;
  m_FamilyIndex = familyIndex;
}


}
