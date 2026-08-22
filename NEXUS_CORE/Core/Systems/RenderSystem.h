#pragma once
#include "../ECS/Registry.h"
#include <Rendering/Core/BatchRenderer.h>

namespace NEXUS_CORE::Systems {
	class RenderSystem
	{
	private: 
		NEXUS_CORE::ECS::Registry& m_Registry;
		std::unique_ptr<NEXUS_RENDERING::BatchRenderer> m_pBatchRenderer;

	public:
		RenderSystem(NEXUS_CORE::ECS::Registry& registry);
		~RenderSystem() = default;

		/*
		* @brief Loops through all of the entities in the registry that have a sprite
		* and transform component. Applies all the necessary transformations and adds them
		* to a Batch to be rendered.
		*/
		void Update();
	};
}
