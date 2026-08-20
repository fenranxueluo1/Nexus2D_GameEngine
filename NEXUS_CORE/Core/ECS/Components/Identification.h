#pragma once
#include <string>

namespace NEXUS_CORE::ECS {
	struct Identification 
	{
		std::string name{"GameObject"}, group{ "" };
		int32_t entity_id{ -1 };
	};
}
