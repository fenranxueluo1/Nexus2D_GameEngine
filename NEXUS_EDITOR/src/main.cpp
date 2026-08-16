#define SDL_MAIN_HANDLED 1
#include <Windowing/Window/Window.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Rendering/Essentials/ShaderLoader.h>
#include <Logger/Logger.h>

class Camera2D
{
private:
    int m_Width, m_Height;
    float m_Scale;

    glm::vec2 m_Position;
    glm::mat4 m_CameraMatrix, m_OrthoProjection;

    bool m_bNeedsUpdate;
    
public:
    Camera2D() : Camera2D(640,480)
    {

    }
    Camera2D(int width, int height) : m_Width{width}, m_Height{height}, m_Scale{1.f}, m_Position{glm::vec2{0}}, m_CameraMatrix{1.f}, m_OrthoProjection{1.f}, m_bNeedsUpdate{true}
    {
       m_OrthoProjection = glm::ortho(0.f, static_cast<float>(m_Width), static_cast<float>(m_Height), 0.f, 0.f, 1.f);
    }

    inline void SetScale(float scale) { m_Scale = scale; m_bNeedsUpdate = true; }

    inline glm::mat4 GetCameraMatrix() { return m_CameraMatrix; }

    void Update()
    {
        if(!m_bNeedsUpdate)
        {
            return;
        }

        glm::vec3 translate{ -m_Position.x, -m_Position.y, 0.f };
        m_CameraMatrix = glm::translate(m_OrthoProjection, translate);

        glm::vec3 scale{ m_Scale, m_Scale, 0.f };
        m_CameraMatrix *= glm::scale(glm::mat4{1.f}, scale);

        m_bNeedsUpdate = false;
    }
};

struct UVs
{
    float u, v, width, height;
    UVs() : u{0.f}, v{0.f}, width{0.f}, height{0.f}
    {

    }
};

bool LoadTexture(const std::string& filepath, int& width, int& height, bool blended)
{
    // 用 SDL3_image 解码图片为 SDL_Surface
    SDL_Surface* surface = IMG_Load(filepath.c_str());
    if (!surface)
    {
        std::cout << "无法加载纹理 [" << filepath << "] -- " << SDL_GetError() << std::endl;
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
        std::cout << "无法转换纹理格式 [" << filepath << "] -- " << SDL_GetError() << std::endl;
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

int main() 
{
    NEXUS_INIT_LOGS(true, true);

    bool running { true };

    // SDL3 已移除 SDL_INIT_EVERYTHING，需显式列出要初始化的子系统
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
        std::string error = SDL_GetError();
        std::cout <<  "无法初始化SDL: " << error << std::endl;
        running = false;
        return -1;
    }

    //设置opengl属性
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    //设置每个通道的位数
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    //创建窗口
    NEXUS_WINDOWING::Window window("测试窗口", 640, 480, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, true, SDL_WINDOW_OPENGL);
    
    if (!window.GetWindow())
    {
        std::cout << "无法创建窗口!" << std::endl;
        return -1;
    }

    //创建opengl上下文（SDL3 会自动加载 GL 库，无需手动 SDL_GL_LoadLibrary）
    SDL_GLContext glContext = SDL_GL_CreateContext(window.GetWindow().get());
    if (glContext == nullptr)
    {
        std::string error = SDL_GetError();
        std::cout << "无法创建opengl上下文: " << error << std::endl;
        running = false;
        return -1;
    }
    window.SetGLContext(glContext);

    SDL_GL_MakeCurrent(window.GetWindow().get(), window.GetGLContext());
    SDL_GL_SetSwapInterval(1);

    //初始glad（GLAD 内置加载器会自动加载 opengl32.dll 并通过 wglGetProcAddress 获取函数指针）
    if (gladLoadGL() == 0)
    {
        std::cout << "无法初始化glad!" << std::endl;
        running = false;
        return -1;
    }

    //启用Alpha Blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //临时加载纹理
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    int width {0}, height {0};
    if(!LoadTexture("assets/textures/tileset.png", width, height, false))
    {
        NEXUS_ERROR("无法加载纹理!");
        return -1;
    }

    UVs uvs{};
    NEXUS_LOG("加载纹理：[宽度={0}, 高度={1}]", width, height);
    NEXUS_WARN("加载纹理：[宽度={0}, 高度={1}]", width, height);
    auto generateUVs = [&](float startX, float startY, float spriteWidth, float spriteHeight)
    {
        // startX/startY/spriteWidth/spriteHeight 均为像素坐标，
        // 转成归一化 UV 时直接除以纹理总宽/总高。
        uvs.width  = spriteWidth  / width;
        uvs.height = spriteHeight / height;

        uvs.u = startX / width;
        uvs.v = startY / height;
    };

    generateUVs(0, 0, 16, 16);

    //创建顶点数据
    // 位置(x,y,z) | 纹理坐标(u,v)
    // 屏幕坐标 y 向下：y=10 在上，y=26 在下。
    // 已取消像素 Y 翻转：纹理 V=0 对应原图顶部，V 自上而下递增。
    // 故屏幕上方顶点取子图顶部 V（uvs.v），下方顶点取底部 V（uvs.v + uvs.height）。
    // 四个顶点按顺时针排列：左上 -> 右上 -> 右下 -> 左下
    float vertices[] = {
        10.f, 10.f, 0.0f,  uvs.u,              uvs.v,               // 屏幕左上 -> 子图左上
        26.f, 10.f, 0.0f, (uvs.u + uvs.width), uvs.v,               // 屏幕右上 -> 子图右上
        26.f, 26.f, 0.0f, (uvs.u + uvs.width),(uvs.v + uvs.height), // 屏幕右下 -> 子图右下
        10.f, 26.f, 0.0f,  uvs.u,             (uvs.v + uvs.height)  // 屏幕左下 -> 子图左下
    };
 
    GLuint indices[] = 
    {
        0, 1, 2,
        2, 3, 0
    };

    //创建临时相机
    Camera2D camera{};
    camera.SetScale(5.f);
    // 在使用相机矩阵前先更新一次，否则 GetCameraMatrix() 返回的是初始单位矩阵
    camera.Update();

    //创建第一个着色器
    auto shader = NEXUS_RENDERING::ShaderLoader::Create("assets/shaders/basicShader.vert", "assets/shaders/basicShader.frag");

    if (!shader)
    {
        std::cout << "无法创建着色器!" << std::endl;
        return -1;
    }
    

    //创建顶点数组和顶点缓冲对象
    GLuint VAO, VBO, IBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    //绑定顶点数组对象
    glBindVertexArray(VAO);
    //绑定顶点缓冲对象
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //将顶点数据复制到缓冲对象（sizeof(vertices) 已是总字节数，不要再乘 stride）
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(GLuint), indices, GL_STATIC_DRAW);

    //设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    SDL_Event event;
    //窗口循环
    while (running)
    {
        //处理事件
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_ESCAPE)
                {
                    running = false;
                }
                break;
            default:
                break;
            }
        }

        glViewport(
            0,
            0,
            window.GetWidth(),
            window.GetHeight()
        );

        glClearColor(1.f, 1.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        shader->Enable();
        glBindVertexArray(VAO);

        // 先更新相机，再取矩阵，确保本帧使用的是最新投影
        camera.Update();

        auto projection = camera.GetCameraMatrix();

        shader->SetUniformMat4("uProjection", projection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);
        SDL_GL_SwapWindow(window.GetWindow().get());

        camera.Update();
        shader->Disable();

    }

    std::cout << "结束" << std::endl;
    return 0;
}
