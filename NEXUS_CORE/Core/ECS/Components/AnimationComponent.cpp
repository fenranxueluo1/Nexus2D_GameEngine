#include "AnimationComponent.h"
#include <entt.hpp>
#include <lua.hpp>
#include <LuaBridge3/LuaBridge.h>

void NEXUS_CORE::ECS::AnimationComponent::CreateAnimationLuaBind(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<AnimationComponent>("Animation")
		.addConstructor([](void* storage, int numFrames, int frameRate, int frameOffset, bool bVertical) -> AnimationComponent* {
			return new (storage) AnimationComponent{
				.numFrames = numFrames,
				.frameRate = frameRate,
				.frameOffset = frameOffset,
				.bVertical = bVertical
			};
		})
		.addStaticFunction("type_id", &entt::type_hash<AnimationComponent>::value)
		.addPropertyReadWrite("num_frames", &AnimationComponent::numFrames)
		.addPropertyReadWrite("frame_rate", &AnimationComponent::frameRate)
		.addPropertyReadWrite("frame_offset", &AnimationComponent::frameOffset)
		.addPropertyReadWrite("current_frame", &AnimationComponent::currentFrame)
		.addPropertyReadWrite("bVertical", &AnimationComponent::bVertical)
		.endClass();
}