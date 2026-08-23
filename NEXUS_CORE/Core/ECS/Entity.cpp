#include "Entity.h"
#include "Components/Identification.h"
#include "MetaUtilities.h"
#include <LuaBridge3/LuaBridge.h>
#include <entt.hpp>

using namespace NEXUS_CORE::Utils;

namespace NEXUS_CORE::ECS {

	Entity::Entity(Registry& registry)
		: Entity(registry, "GameObject", "")
	{
	}

	Entity::Entity(Registry& registry, const std::string& name, const std::string& group)
		: m_Registry(registry), m_Entity{registry.CreateEntity()}, m_sName{name}, m_sGroup{group}
	{
		AddComponent<Identification>(Identification{
				.name = name,
				.group = group,
				.entity_id = static_cast<int32_t>(m_Entity)
			});
	}
    Entity::Entity(Registry &registry, const entt::entity &entity)
		: m_Registry(registry), m_Entity(entity), m_sName{""}, m_sGroup{""}
    {
		if (HasComponent<Identification>())
		{
			const auto& id = GetComponent<Identification>();
			m_sName = id.name;
			m_sGroup = id.group;
		}
    }

	void Entity::CreateLuaEntityBind(lua_State* L, Registry& registry)
	{
		using namespace entt::literals;

		luabridge::getGlobalNamespace(L)
			.beginClass<Entity>("Entity")
			.addConstructor([&registry](void* storage, const std::string& name, const std::string& group) -> Entity* {
				return new (storage) Entity{ registry, name, group };
			})
			.addFunction("add_component", [&](Entity& entity, const luabridge::LuaRef& comp, lua_State* state) -> luabridge::LuaRef {
				if (comp.isNil())
					return luabridge::LuaRef(state);

				const auto component = InvokeMetaFunction(
					GetIdType(comp),
					"add_component"_hs,
					entity, comp, LuaState{ state }
				);

				if (auto* ref = component.try_cast<luabridge::LuaRef>())
					return *ref;

				return luabridge::LuaRef(state);
			})
			.endClass();
	}
}