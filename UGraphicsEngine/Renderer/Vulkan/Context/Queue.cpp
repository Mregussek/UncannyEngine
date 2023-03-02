
#include "Queue.h"


namespace uncanny::vulkan
{


void FQueue::Initialize(VkQueue queue, FQueueFamilyIndex familyIndex)
{
  m_Queue = queue;
  m_FamilyIndex = familyIndex;
}


}
