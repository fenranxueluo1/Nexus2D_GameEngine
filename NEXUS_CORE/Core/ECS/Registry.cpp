#include "Registry.h"
#include "Entity.h"
#include "MetaUtilities.h"
#include <lua.hpp>
#include <LuaBridge3/LuaBridge.h>

using namespace NEXUS_CORE::Utils;

namespace NEXUS_CORE::ECS {

Registry::Registry()
	: m_pRegistry{nullptr}
{
	m_pRegistry = std::make_unique<entt::registry>();
}

void Registry::CreateLuaRegistryBind(lua_State* L, Registry& registry)
{
	using namespace entt::literals;

	luabridge::getGlobalNamespace(L)
		.beginClass<entt::runtime_view>("runtime_view")
		.addFunction("for_each", [&](entt::runtime_view& view, const luabridge::LuaRef& callback) {
			if (!callback.isCallable())
				return;

			for (auto entity : view)
			{
				Entity ent{ registry, entity };
				callback(ent);
			}
		})
		.addFunction("exclude", [&](entt::runtime_view& view, lua_State* state) {
			const int n = lua_gettop(state);
			for (int i = 2; i <= n; i++)
			{
				const luabridge::LuaRef type = luabridge::LuaRef::fromStack(state, i);
				if (!type.isTable() && !type.isUserdata())
					continue;

				const auto excluded_view = InvokeMetaFunction(
					GetIdType(type),
					"exclude_component_from_view"_hs,
					&registry, view
				);

				if (excluded_view)
					view = excluded_view.cast<entt::runtime_view>();
			}
		})
		.endClass();

	luabridge::getGlobalNamespace(L)
		.beginClass<Registry>("Registry")
		.addStaticFunction("get_entities", [&registry](lua_State* state) -> luabridge::LuaRef {
			entt::runtime_view view{};
			const int n = lua_gettop(state);
			for (int i = 1; i <= n; i++)
			{
				const luabridge::LuaRef type = luabridge::LuaRef::fromStack(state, i);
				if (!type.isTable() && !type.isUserdata())
					continue;

				const auto entities = InvokeMetaFunction(
					GetIdType(type),
					"add_component_to_view"_hs,
					&registry, view
				);

				if (entities)
					view = entities.cast<entt::runtime_view>();
			}

			return luabridge::LuaRef(state, view);
		})
		.addStaticFunction("clear", [&registry]() { registry.GetRegistry().clear(); })
		.endClass();
}

}