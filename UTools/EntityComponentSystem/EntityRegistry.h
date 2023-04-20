
#ifndef UNCANNYENGINE_ENTITYREGISTRY_H
#define UNCANNYENGINE_ENTITYREGISTRY_H


#include <entt/entt.hpp>
#include "Entity.h"
#include <span>


namespace uncanny
{


class FEntityRegistry
{
public:

  void Create();
  void Destroy();

  FEntity Register();
  std::span<FEntity> Register(u32 count);

  [[nodiscard]] std::span<const FEntity> GetEntities() const { return m_Entities; }

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
  std::vector<FEntity> m_Entities{};

};


}


#endif //UNCANNYENGINE_ENTITYREGISTRY_H
