#pragma once

struct lua_State;

namespace NEXUS_CORE::ECS {
	struct AnimationComponent
	{
		int numFrames{ 1 }, frameRate{ 1 }, frameOffset{ 0 }, currentFrame{ 0 };
		bool bVertical{ false };
		static void CreateAnimationLuaBind(lua_State* lua);
	};
}