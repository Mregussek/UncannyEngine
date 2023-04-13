
#include "Entity.h"


namespace uncanny
{


FEntity::FEntity(entt::registry *pRegistry)
  : m_pRegistry(pRegistry)
{
  m_Entity = m_pRegistry->create();
}


void FEntity::Destroy()
{
  m_pRegistry->destroy(m_Entity);
}


b32 FEntity::IsValid() const
{
  return m_pRegistry->valid(m_Entity);
}


}
