
#ifndef UNCANNYENGINE_ENTITYREGISTRY_H
#define UNCANNYENGINE_ENTITYREGISTRY_H


#include <entt/entt.hpp>
#include "Entity.h"


namespace uncanny
{


class FEntityRegistry
{
public:

  void Create();
  void Destroy();

  FEntity RegisterEntity();

private:

  entt::registry m_Registry{};

};


}


#endif //UNCANNYENGINE_ENTITYREGISTRY_H
