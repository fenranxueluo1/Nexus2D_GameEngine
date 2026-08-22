#pragma once
#include <lua.hpp>
#include <LuaBridge3/LuaBridge.h>

namespace NEXUS_CORE::ECS {
	struct ScriptComponent
	{
		explicit ScriptComponent(lua_State* state)
			: update(state), render(state) {}

		luabridge::LuaRef update;
		luabridge::LuaRef render;
	};
}
