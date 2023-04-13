
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
  return FEntity{ &m_Registry };
}


}
