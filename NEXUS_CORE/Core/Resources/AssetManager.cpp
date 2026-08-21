#include "AssetManager.h"
#include <Rendering/Essentials/TextureLoader.h>
#include <Rendering/Essentials/ShaderLoader.h>
#include <Logger/Logger.h>

namespace NEXUS_RESOURCES {

    bool AssetManager::AddTexture(const std::string& textureName, const std::string& texturePath, bool pixelArt)
    {
        // Check to see if the texture already exists
        if (m_mapTextures.find(textureName) != m_mapTextures.end())
        {
            NEXUS_ERROR("无法添加纹理 [{}] -- 纹理已存在!", textureName);
            return false;
        }

        auto texture = NEXUS_RENDERING::TextureLoader::Create(
            pixelArt ? NEXUS_RENDERING::Texture::TextureType::PIXEL : NEXUS_RENDERING::Texture::TextureType::BLENDED,
            texturePath
        );

        if (!texture)
        {
            NEXUS_ERROR("无法加载纹理 [{}] 路径为 [{}]", textureName, texturePath);
            return false;
        }

        m_mapTextures.emplace(textureName, std::move(texture));
        return true;
    }

    const NEXUS_RENDERING::Texture& AssetManager::GetTexture(const std::string& textureName)
    {
        auto texItr = m_mapTextures.find(textureName);
        if (texItr == m_mapTextures.end())
        {
            NEXUS_ERROR("无法获取纹理 [{}] -- 纹理不存在!", textureName);
            static NEXUS_RENDERING::Texture emptyTexture{};
            return emptyTexture;
        }

        return *texItr->second;
    }

    bool AssetManager::AddShader(const std::string& shaderName, const std::string& vertexPath, const std::string& fragmentPath)
    {
        // Check to see if the shader already exists
        if (m_mapShader.find(shaderName) != m_mapShader.end())
        {
            NEXUS_ERROR("无法加载着色器 [{}] -- 着色器已存在!", shaderName);
            return false;
        }

        // Create and load the shader
        auto shader = NEXUS_RENDERING::ShaderLoader::Create(vertexPath, fragmentPath);

        if (!shader)
        {
            NEXUS_ERROR("无法加载着色器 [{}] 其顶点着色器位于 [{}] 其片段着色器位于 [{}]", shaderName, vertexPath, fragmentPath);
            return false;
        }

        m_mapShader.emplace(shaderName, std::move(shader));
        return true;
    }

    NEXUS_RENDERING::Shader& AssetManager::GetShader(const std::string& shaderName)
    {
        auto shaderItr = m_mapShader.find(shaderName);
        if (shaderItr == m_mapShader.end())
        {
            NEXUS_ERROR("无法获取着色器 [{}] -- 着色器不存在!", shaderName);
            static NEXUS_RENDERING::Shader emptyShader{};
            return emptyShader;
        }

        return *shaderItr->second;
    }
}
