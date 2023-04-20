
#include "EntityRegistry.h"


namespace uncanny
{


void FEntityRegistry::Create()
{
  m_Registry = entt::registry();
}


void FEntityRegistry::Destroy()
{
  m_Registry.clear();
}


FEntity FEntityRegistry::Register()
{
  m_Entities.push_back(FEntity{ &m_Registry });
  return m_Entities[m_Entities.size() - 1];
}


std::span<FEntity> FEntityRegistry::Register(u32 count)
{
  u32 savingSize = m_Entities.size();
  for (u32 i = 0; i < count; i++)
  {
    Register();
  }

  auto it = std::next(m_Entities.begin(), savingSize);
  return { it, m_Entities.end() };
}


}
