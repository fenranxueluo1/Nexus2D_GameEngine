#include "ScriptingSystem.h"
#include "../ECS/Components/ScriptComponent.h"
#include "../ECS/Components/TransformComponent.h"
#include "../ECS/Components/SpriteComponent.h"
#include "../ECS/Components/AnimationComponent.h"
#include "../ECS/Entity.h"
#include "../Scripting/GlmLuaBindings.h"
#include "../Scripting/InputManager.h"
#include "../Resources/AssetManager.h"
#include <Logger/Logger.h>
#include <lua.hpp>
#include <LuaBridge3/LuaBridge.h>

using namespace NEXUS_CORE::ECS;

namespace NEXUS_CORE::Systems {

	ScriptingSystem::ScriptingSystem(NEXUS_CORE::ECS::Registry& registry)
		: m_Registry(registry), m_bMainLoaded{ false }
	{
	}

	bool ScriptingSystem::LoadMainScript(lua_State* lua)
	{
		//执行 main.lua 脚本，出错时错误信息位于栈顶
		if (luaL_dofile(lua, "./assets/scripts/main.lua") != LUA_OK)
		{
			NEXUS_ERROR("加载lua脚本时出错: {}", lua_tostring(lua, -1));
			lua_pop(lua, 1);
			return false;
		}

		//获取全局表 main
		luabridge::LuaRef main_lua = luabridge::getGlobal(lua, "main");
		if (main_lua.isNil())
		{
			NEXUS_ERROR("main.lua文件中没有全局 main 表");
			return false;
		}

		//main[1] 为更新脚本表
		luabridge::LuaRef update_script = main_lua[1];
		if (!update_script.isTable())
		{
			NEXUS_ERROR("main.lua文件中没有更新方法");
			return false;
		}
		luabridge::LuaRef update = update_script["update"];

		//main[2] 为渲染脚本表
		luabridge::LuaRef render_script = main_lua[2];
		if (!render_script.isTable())
		{
			NEXUS_ERROR("main.lua文件中没有渲染方法");
			return false;
		}
		luabridge::LuaRef render = render_script["render"];

		NEXUS_CORE::ECS::Entity mainLuaScript{m_Registry, "main_script", ""};
		mainLuaScript.AddComponent<NEXUS_CORE::ECS::ScriptComponent>(lua);
		auto& script = mainLuaScript.GetComponent<NEXUS_CORE::ECS::ScriptComponent>();
		script.update = update;
		script.render = render;

		m_bMainLoaded = true;
		return true;
	}

	void ScriptingSystem::Update()
	{
		if (!m_bMainLoaded)
		{
			NEXUS_ERROR("主lua脚本尚未加载!");
			return;
		}

		auto view = m_Registry.GetRegistry().view<NEXUS_CORE::ECS::ScriptComponent>();

		for (const auto& entity : view)
		{
			NEXUS_CORE::ECS::Entity ent{m_Registry, entity};
			if (ent.GetName() != "main_script")
				continue;

			auto& script = ent.GetComponent<NEXUS_CORE::ECS::ScriptComponent>();
			try
			{
				script.update();
			}
			catch (const luabridge::LuaException& err)
			{
				NEXUS_ERROR("运行更新脚本时出现错误: {}", err.what());
			}
		}
	}

	void ScriptingSystem::Render()
	{
		if (!m_bMainLoaded)
		{
			NEXUS_ERROR("主lua脚本尚未加载!");
			return;
		}

		auto view = m_Registry.GetRegistry().view<NEXUS_CORE::ECS::ScriptComponent>();

		for (const auto& entity : view)
		{
			NEXUS_CORE::ECS::Entity ent{m_Registry, entity};
			if (ent.GetName() != "main_script")
				continue;

			auto& script = ent.GetComponent<NEXUS_CORE::ECS::ScriptComponent>();
			try
			{
				script.render();
			}
			catch (const luabridge::LuaException& err)
			{
				NEXUS_ERROR("运行渲染脚本时出现错误: {}", err.what());
			}
		}
	}

	void ScriptingSystem::RegisterLuaBindings(lua_State* lua, NEXUS_CORE::ECS::Registry& registry)
	{
		NEXUS_CORE::Scripting::GLMBindings::CreateGLMBindings(lua);
		NEXUS_CORE::InputManager::CreateLuaInputBindings(lua);

		NEXUS_RESOURCES::AssetManager::CreateLuaAssetManager(lua, registry);

		Registry::CreateLuaRegistryBind(lua, registry);
		Entity::CreateLuaEntityBind(lua, registry);
		TransformComponent::CreateLuaTransformBind(lua);
		SpriteComponent::CreateSpriteLuaBind(lua, registry);
		AnimationComponent::CreateAnimationLuaBind(lua);

		Entity::RegisterMetaComponent<TransformComponent>();
		Entity::RegisterMetaComponent<SpriteComponent>();
		Entity::RegisterMetaComponent<AnimationComponent>();

		Registry::RegisterMetaComponent<TransformComponent>();
		Registry::RegisterMetaComponent<SpriteComponent>();
		Registry::RegisterMetaComponent<AnimationComponent>();
	}

	void ScriptingSystem::RegisterLuaFunctions(lua_State* lua)
	{
		luabridge::getGlobalNamespace(lua)
			.addFunction(
				"run_script", [](lua_State* state)
				{
					// 首个参数即脚本路径
					const char* path = luaL_checkstring(state, 1);

					if (luaL_dofile(state, path) != LUA_OK)
					{
						NEXUS_ERROR("Error loading Lua Script: {}", lua_tostring(state, -1));
						lua_pop(state, 1);
						return false;
					}

					return true;
				}
			);
	}
}
