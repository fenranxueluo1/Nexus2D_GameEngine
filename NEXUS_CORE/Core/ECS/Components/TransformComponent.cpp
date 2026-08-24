#include "TransformComponent.h"
#include <entt.hpp>
#include <lua.hpp>
#include <LuaBridge3/LuaBridge.h>
#include <tuple>

void NEXUS_CORE::ECS::TransformComponent::CreateLuaTransformBind(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<TransformComponent>("Transform")
		.addConstructor([](void* storage, float x, float y, float scale_x, float scale_y, float rotation) -> TransformComponent* {
			return new (storage) TransformComponent{
				.position = glm::vec2{ x, y },
				.scale = glm::vec2{ scale_x, scale_y },
				.rotation = rotation
			};
		})
		.addStaticFunction("type_id", &entt::type_hash<TransformComponent>::value)
		.addFunction("position", [](TransformComponent& transform) { return std::tuple<float, float>{ transform.position.x, transform.position.y }; })
		.addFunction("scale", [](TransformComponent& transform) { return std::tuple<float, float>{ transform.scale.x, transform.scale.y }; })
		.addPropertyReadWrite("rotation", &TransformComponent::rotation)
		.addFunction("set_pos", [](TransformComponent& transform, float x, float y) { transform.position = glm::vec2{ x, y }; })
		.addFunction("set_scale", [](TransformComponent& transform, float x, float y) { transform.scale = glm::vec2{ x, y }; })
		.endClass();
}