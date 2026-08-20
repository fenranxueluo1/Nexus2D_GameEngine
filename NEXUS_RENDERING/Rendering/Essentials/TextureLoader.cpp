#include "TextureLoader.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <Logger/Logger.h>

namespace NEXUS_RENDERING {

    bool TextureLoader::LoadTexture(const std::string& filepath, GLuint& id, int& width, int& height, bool blended)
    {
		 // 用 SDL3_image 解码图片为 SDL_Surface
    SDL_Surface* surface = IMG_Load(filepath.c_str());
    if (!surface)
    {
        const char* err = SDL_GetError();
        NEXUS_ERROR("无法加载纹理 [{}] -- {}", filepath, err);
        return false;
    }

    // 统一转换为当前平台的 "RGBA 逐字节数组" 格式。
    // 注意：SDL 的 PACKEDORDER_RGBA 在小端机内存里实际是 A B G R，
    // 与 OpenGL 的 GL_RGBA/UNSIGNED_BYTE（按内存字节序 R,G,B,A）相反。
    // 因此不能用 SDL_PIXELFORMAT_RGBA8888，而要用 SDL_PIXELFORMAT_RGBA32——
    // 它正是 SDL 为 "当前平台的 RGBA 字节数组" 定义的别名（见 SDL_pixels.h 注释），
    // 与 GL_RGBA 上传语义一致，可避免红蓝颠倒。
    SDL_Surface* converted = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(surface);
    if (!converted)
    {
        const char* err = SDL_GetError();
		NEXUS_ERROR("无法转换纹理格式 [{}] -- {}", filepath, err);
        return false;
    }

    width  = converted->w;
    height = converted->h;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (!blended)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    // 直接使用 SDL 自上而下的像素行序。
    // 不做 Y 翻转：纹理坐标 (0,0) 将对应原图左上角（与 SDL surface 的 (0,0) 一致）。
    // 若翻转为 OpenGL 默认的“自下而上”，则 (0,0) 反而会指向原图左下角，导致画面上下颠倒。
    // converted 的 pitch 可能含行尾 padding，需按 UNPACK_ROW_LENGTH 告知 GL 真实行步长。
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, converted->pitch / 4);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, converted->pixels);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    SDL_DestroySurface(converted);

    return true;
}

    std::shared_ptr<Texture> TextureLoader::Create(Texture::TextureType type, const std::string& texturePath)
    {
		GLuint id;
		int width, height;

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		switch (type)
		{
		case Texture::TextureType::PIXEL:
			LoadTexture(texturePath, id, width, height, false);
			break;
		case Texture::TextureType::BLENDED:
			LoadTexture(texturePath, id, width, height, true);
			break;
		// TODO: 根据需要提供其它纹理类型以供加载 -- 例如 Framebuffer texture
		default:
			assert(false && "当前类型尚未定义，请使用已定义的纹理类型！");
			return nullptr;
		}

        return std::make_shared<Texture>(id, width, height, type, texturePath);
    }

}