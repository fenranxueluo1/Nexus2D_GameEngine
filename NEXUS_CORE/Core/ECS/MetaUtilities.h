#pragma once
#include <entt.hpp>
#include <lua.hpp>
#include <LuaBridge3/LuaBridge.h>

#include <Logger/Logger.h>

namespace NEXUS_CORE::Utils {

	[[nodiscard]] entt::id_type GetIdType(const luabridge::LuaRef& comp);

	template <typename ...Args>
	inline auto InvokeMetaFunction(entt::meta_type meta, entt::id_type func_id, Args&& ...args)
	{
		if (!meta)
		{
			NEXUS_ERROR("未提供有效的entt::meta_type值或该值无效!");
			assert(false && "未提供有效的entt::meta_type值或该值无效!");
			return entt::meta_any{};
		}

		if (auto meta_function = meta.func(func_id); meta_function)
			return meta_function.invoke({}, std::forward<Args>(args) ...);

		return entt::meta_any{};
	}

	template <typename ...Args>
	inline auto InvokeMetaFunction(entt::id_type id, entt::id_type func_id, Args&& ...args)
	{
		return InvokeMetaFunction(entt::resolve(id), func_id, std::forward<Args>(args) ...);
	}
}
