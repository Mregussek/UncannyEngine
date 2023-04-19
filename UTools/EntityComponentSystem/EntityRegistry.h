
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

  FEntity Register();

  template<ConceptComponent TComponent>
  void ForEach(std::function<void(TComponent&)> func)
  {
    auto view = m_Registry.view<TComponent>();
    for (auto entity : view)
    {
      auto& component = view.template get<TComponent>(entity);
      func(component);
    }
  }

private:

  entt::registry m_Registry{};

};


}


#endif //UNCANNYENGINE_ENTITYREGISTRY_H
