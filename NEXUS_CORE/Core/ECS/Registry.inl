#include "Registry.h"

namespace NEXUS_CORE::ECS {
	template<typename TContext>
	inline TContext Registry::AddToContext(TContext context)
	{
		return m_pRegistry->ctx().emplace<TContext>(context);
	}

	template<typename TContext>
	inline TContext& Registry::GetContext()
	{
		return m_pRegistry->ctx().get<TContext>();
	}

	template <typename TComponent>
	entt::runtime_view& add_component_to_view(Registry* registry, entt::runtime_view& view)
	{
		return view.iterate(registry->GetRegistry().storage<TComponent>());
	}

	template <typename TComponent>
	entt::runtime_view& exclude_component_from_view(Registry* registry, entt::runtime_view& view)
	{
		return view.exclude(registry->GetRegistry().storage<TComponent>());
	}

	template <typename TComponent>
	void Registry::RegisterMetaComponent()
	{
		using namespace entt::literals;
		// 注意：meta_factory<TComponent> 是依赖类型，按 C++ 标准调用其成员模板
		// 必须写 .template func<...>()；MSVC 容忍省略，GCC/Clang 则要求显式写出。
		entt::meta_factory<TComponent>()
			.type(entt::type_hash<TComponent>::value())
			.template func<&add_component_to_view<TComponent>>("add_component_to_view"_hs)
			.template func<&exclude_component_from_view<TComponent>>("exclude_component_from_view"_hs);
	}
}
