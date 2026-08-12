#define SDL_MAIN_HANDLED 1
#include <Windowing/Window/Window.h>
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <iostream>
#include <SOIL/SOIL.h>

bool LoadTexture(const std::string& filepath, int& width, int& height, bool blended)
{
    int channels = 0;

    unsigned char* image = SOIL_load_image(filepath.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);
    if (!image)
    {
        std::cout << "无法加载纹理 [" << filepath << "] -- " << SOIL_last_result();
        return false;
    }

    GLint format = GL_RGBA;

    switch (channels)
    {
        case 3: format = GL_RGB; break;
        case 4: format = GL_RGBA; break;
    }

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

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);

    SOIL_free_image_data(image);

    return true;
}

int main() 
{
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
    NEXUS_WINDOWING::Window window("测试窗口", 480, 480, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, true, SDL_WINDOW_OPENGL);
    
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

    //临时加载纹理
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    int width {0}, height {0};
    if(!LoadTexture("assets/textures/tileset.png", width, height, false))
    {
        std::cout << "无法加载纹理!" << std::endl;
        return -1;
    }

    //创建顶点数据
    float vertices[] = {
        -0.5f, 0.5f, 0.0f,  0.f, 1.f,
        0.5f, 0.5f, 0.0f,   1.f, 1.f,
        0.5f, -0.5f, 0.0f,  1.f, 0.f,
        -0.5f, -0.5f, 0.0f, 0.f, 0.f
    };
 
    GLuint indices[] = 
    {
        0, 1, 2,
        2, 3, 0
    };

    //创建顶点着色器代码
    const char* vertexSource = 
        "#version 460 core\n"
        "layout (location = 0) in vec3 aPosition;\n"
        "layout (location = 1) in vec2 aTexCoords;\n"
        "out vec2 fragUVs;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPosition, 1.0);\n"
        "   fragUVs = aTexCoords;\n"
        "}\0";

    //创建顶点着色器
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //添加顶点着色器代码
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    //编译顶点着色器
    glCompileShader(vertexShader);
    //获取编译状态
    int status;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "顶点着色器编译失败: " << infoLog << std::endl;
        return -1;
    }

    //创建片段着色器代码
    const char* fragmentSource = 
        "#version 460 core\n"
        "in vec2 fragUVs;\n"
        "out vec4 color;\n"
        "uniform sampler2D uTexture;\n"
        "void main()\n"
        "{\n"
        //"   color = vec4(1.0f, 0.0f, 1.0f, 1.0f);\n"
        "   color = texture(uTexture, fragUVs);\n"
        "}\0";

    //创建片段着色器
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    //添加片段着色器代码
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    //编译片段着色器
    glCompileShader(fragmentShader);
    //获取编译状态
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "片段着色器编译失败: " << infoLog << std::endl;
        return -1;
    }

    //创建着色器程序
    GLuint shaderProgram = glCreateProgram();
    //附加着色器
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    //链接程序
    glLinkProgram(shaderProgram);
    //获取链接状态
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if (!status)
    {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "着色器程序链接失败: " << infoLog << std::endl;
        return -1;
    }

    //启用着色器程序
    glUseProgram(shaderProgram);

    //删除着色器，着色器程序链接成功后，着色器不再需要
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //创建顶点数组和顶点缓冲对象
    GLuint VAO, VBO, IBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    //绑定顶点数组对象
    glBindVertexArray(VAO);
    //绑定顶点缓冲对象
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //将顶点数据复制到缓冲对象
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)*3*sizeof(float), vertices, GL_STATIC_DRAW);

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

        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);

        SDL_GL_SwapWindow(window.GetWindow().get());

    }

    std::cout << "结束" << std::endl;
    return 0;
}
