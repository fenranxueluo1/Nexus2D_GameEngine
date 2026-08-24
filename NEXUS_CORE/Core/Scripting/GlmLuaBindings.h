#pragma once

struct lua_State;

namespace NEXUS_CORE::Scripting {
	struct GLMBindings
	{
		static void CreateGLMBindings(lua_State* lua);
	};
}