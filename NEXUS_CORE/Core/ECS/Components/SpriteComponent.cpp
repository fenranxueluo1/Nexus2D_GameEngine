#include "SpriteComponent.h"
#include "../../Resources/AssetManager.h"
#include <Logger/Logger.h>
#include <lua.hpp>
#include <LuaBridge3/LuaBridge.h>
#include <entt.hpp>

using namespace NEXUS_RESOURCES;

void NEXUS_CORE::ECS::SpriteComponent::CreateSpriteLuaBind(lua_State* L, NEXUS_CORE::ECS::Registry& registry)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<SpriteComponent>("Sprite")
		.addConstructor([](void* storage, const std::string& textureName, float width, float height, int start_x, int start_y, int layer) -> SpriteComponent* {
			return new (storage) SpriteComponent{
				.width = width,
				.height = height,
				.uvs = UVs{},
				.color = NEXUS_RENDERING::Color{ 255, 255, 255, 255 },
				.start_x = start_x,
				.start_y = start_y,
				.layer = layer,
				.texture_name = textureName
			};
		})
		.addStaticFunction("type_id", &entt::type_hash<SpriteComponent>::value)
		.addPropertyReadWrite("texture_name", &SpriteComponent::texture_name)
		.addPropertyReadWrite("width", &SpriteComponent::width)
		.addPropertyReadWrite("height", &SpriteComponent::height)
		.addPropertyReadWrite("start_x", &SpriteComponent::start_x)
		.addPropertyReadWrite("start_y", &SpriteComponent::start_y)
		.addPropertyReadWrite("layer", &SpriteComponent::layer)
		.addFunction("generate_uvs", [&registry](SpriteComponent& sprite) {
			auto& assetManager = registry.GetContext<std::shared_ptr<AssetManager>>();
			auto& texture = assetManager->GetTexture(sprite.texture_name);

			if (texture.GetID() == 0)
			{
				NEXUS_ERROR("无法生成纹理坐标uVS -- 纹理 [{}] -- 不存在或无效", sprite.texture_name);
				return;
			}

			sprite.generate_uvs(texture.GetWidth(), texture.GetHeight());
		})
		.endClass();
}