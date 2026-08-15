#version 460 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoords;
out vec2 fragUVs;
uniform mat4 uProjection;
void main()
{
   gl_Position = uProjection * vec4(aPosition, 1.0);
   fragUVs = aTexCoords;
}