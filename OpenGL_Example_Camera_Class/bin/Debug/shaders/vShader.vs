//VERTEX SHADER
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 myColor;
out vec2 TexCoord;

uniform mat4 localMat;
uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

void main()
{

    gl_Position = projMat * viewMat * modelMat * localMat * vec4(aPos, 1.0);
    myColor = aColor;
    TexCoord = aTexCoord;
}
