
#ifndef UNCANNYENGINE_ENTITY_H
#define UNCANNYENGINE_ENTITY_H


#include <entt/entt.hpp>
#include "Components.h"


namespace uncanny
{


class FEntity
{

  friend class FEntityRegistry;

public:

  FEntity() = default;

  void Destroy();

  [[nodiscard]] b32 IsValid() const;

  template<ConceptComponent TComponent, typename... Args>
  TComponent& Add(Args&&... args) const
  {
    return m_pRegistry->emplace<TComponent>(m_Entity, std::forward<Args>(args)...);
  }

  template<ConceptComponent TComponent>
  [[nodiscard]] TComponent& Get() const
  {
    return m_pRegistry->get<TComponent>(m_Entity);
  }

  template<ConceptComponent TComponent>
  [[nodiscard]] TComponent& Remove() const
  {
    return m_pRegistry->erase<TComponent>(m_Entity);
  }

private:

  explicit FEntity(entt::registry* pRegistry);

  entt::registry* m_pRegistry{ nullptr };
  entt::entity m_Entity{ entt::null };

};


}


#endif //UNCANNYENGINE_ENTITY_H
