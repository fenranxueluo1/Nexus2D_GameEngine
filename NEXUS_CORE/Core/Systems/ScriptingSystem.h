#pragma once
#include "../ECS/Registry.h"

struct lua_State;

namespace NEXUS_CORE::Systems {
	class ScriptingSystem
	{
	private:
		NEXUS_CORE::ECS::Registry& m_Registry;
		bool m_bMainLoaded;

	public:
		ScriptingSystem(NEXUS_CORE::ECS::Registry& registry);
		~ScriptingSystem() = default;

		bool LoadMainScript(lua_State* lua);
		void Update();
		void Render();

		static void RegisterLuaBindings(lua_State* lua, NEXUS_CORE::ECS::Registry& registry);

		static void RegisterLuaFunctions(lua_State* lua);
	};

}
