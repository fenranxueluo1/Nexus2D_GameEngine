#pragma once
#include "Entity.h"
#include <lua.hpp>
#include <LuaBridge3/LuaBridge.h>

namespace NEXUS_CORE::ECS {
	template <typename TComponent, typename ...Args>
	TComponent& Entity::AddComponent(Args&& ...args)
	{
		auto& registry = m_Registry.GetRegistry();
		return registry.emplace<TComponent>(m_Entity, std::forward<Args>(args) ...);
	}

	template <typename TComponent, typename ...Args>
	TComponent& Entity::ReplaceComponent(Args&& ...args)
	{
		auto& registry = m_Registry.GetRegistry();
		if (registry.all_of<TComponent>(m_Entity))
			return registry.replace<TComponent>(m_Entity, std::forward<Args>(args) ...);
		else
			return registry.emplace<TComponent>(m_Entity, std::forward<Args>(args) ...);
	}

	template <typename TComponent>
	TComponent& Entity::GetComponent()
	{
		auto& registry = m_Registry.GetRegistry();
		return registry.get<TComponent>(m_Entity);
	}

	template <typename TComponent>
	bool Entity::HasComponent()
	{
		auto& registry = m_Registry.GetRegistry();
		return registry.all_of<TComponent>(m_Entity);
	}

	template <typename TComponent>
	void Entity::RemoveComponent()
	{
		auto& registry = m_Registry.GetRegistry();
		return registry.remove<TComponent>(m_Entity);
	}

	template<typename TComponent>
	luabridge::LuaRef add_component(Entity& entity, const luabridge::LuaRef& comp, LuaState state)
	{
		auto& component = entity.AddComponent<TComponent>(
			comp.cast<TComponent>().valueOr(TComponent{})
		);

		return luabridge::LuaRef(state.state, &component);
	}

	template<typename TComponent>
	inline void Entity::RegisterMetaComponent()
	{
		using namespace entt::literals;
		using ComponentAdder = luabridge::LuaRef (*)(Entity&, const luabridge::LuaRef&, LuaState);
		entt::meta_factory<TComponent>()
			.type(entt::type_hash<TComponent>::value())
			.func<static_cast<ComponentAdder>(&add_component<TComponent>)>("add_component"_hs);
	}
}