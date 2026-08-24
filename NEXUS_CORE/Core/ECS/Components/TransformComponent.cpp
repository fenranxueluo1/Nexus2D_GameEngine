#include "TransformComponent.h"
#include <entt.hpp>
#include <lua.hpp>
#include <LuaBridge3/LuaBridge.h>

void NEXUS_CORE::ECS::TransformComponent::CreateLuaTransformBind(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<TransformComponent>("Transform")
		.addConstructor(
			[](void* storage, glm::vec2 position, glm::vec2 scale, float rotation) -> TransformComponent* {
				return new (storage) TransformComponent{
					.position = position,
					.scale = scale,
					.rotation = rotation
				};
			},
			[](void* storage, float x, float y, float scale_x, float scale_y, float rotation) -> TransformComponent* {
				return new (storage) TransformComponent{
					.position = glm::vec2{ x, y },
					.scale = glm::vec2{ scale_x, scale_y },
					.rotation = rotation
				};
			}
		)
		.addStaticFunction("type_id", &entt::type_hash<TransformComponent>::value)
		.addPropertyReadWrite("position", &TransformComponent::position)
		.addPropertyReadWrite("scale", &TransformComponent::scale)
		.addPropertyReadWrite("rotation", &TransformComponent::rotation)
		.endClass();
}