#pragma once
#include "../ECS/Registry.h"

namespace NEXUS_CORE::Systems {
	class AnimationSystem
	{
	private:
		NEXUS_CORE::ECS::Registry& m_Registry;
	public:
		AnimationSystem(NEXUS_CORE::ECS::Registry& registry);
		~AnimationSystem() = default;

		void Update();
	};
}
