#pragma once
#include <map>
#include <memory>
#include <string>

#include <Rendering/Essentials/Shader.h>
#include <Rendering/Essentials/Texture.h>

#include "../ECS/Registry.h"

struct lua_State;

namespace NEXUS_RESOURCES {

	class AssetManager
	{
	private:
		std::map<std::string, std::shared_ptr<NEXUS_RENDERING::Texture>> m_mapTextures{};
		std::map<std::string, std::shared_ptr<NEXUS_RENDERING::Shader>> m_mapShader{};
	public:
		AssetManager() = default;
		~AssetManager() = default;

		/*
		* @brief Checks to see if the texture exists, and if not, creates and loads the texture into the
		* asset manager.
		* @param An std::string for the texture name to be use as the key.
		* @param An std::string for the texture file path to be loaded.
		* @param A bool value to determine if it is pixel art. That controls the type of Min/Mag filter to
		* use.
		* @return Returns true if the texture was created and loaded successfully, false otherwise.
		*/
		bool AddTexture(const std::string& textureName, const std::string& texturePath, bool pixelArt = true);

		/*
		* @brief Checks to see if the texture exists based on the name and returns the texture.
		* @param An std::string for the texture name to lookup.
		* @return Returns the desired texture if it exists, else returns an empty texture object
		*/
		const NEXUS_RENDERING::Texture& GetTexture(const std::string& textureName);
		
		/*
		* @brief Checks to see if the Shader exists, and if not, creates and loads the Shader into the
		* asset manager.
		* @param An std::string for the shader name to be use as the key.
		* @param An std::string for the vertex shader file path to be loaded.
		* @param An std::string for the fragment shader file path to be loaded.
		* @return Returns true if the shader was created and loaded successfully, false otherwise.
		*/
		bool AddShader(const std::string& shaderName, const std::string& vertexPath, const std::string& fragmentPath);

		/*
		* @brief Checks to see if the shader exists based on the name and returns the Shader.
		* @param An std::string for the shader name to lookup.
		* @return Returns the desired shader if it exists, else returns an empty Shader object
		*/
		NEXUS_RENDERING::Shader& GetShader(const std::string& shaderName);

		static void CreateLuaAssetManager(lua_State* lua, NEXUS_CORE::ECS::Registry& registry);
	};
}
