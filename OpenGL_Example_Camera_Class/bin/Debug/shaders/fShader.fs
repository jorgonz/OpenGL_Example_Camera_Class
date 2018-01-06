//FRAGMENT SHADER
#version 330 core

in vec3 myColor;
in vec2 TexCoord;

out vec4 FragColor;

//This is the texture object
uniform sampler2D myTexture;
uniform sampler2D myTexture2;

void main()
{
    FragColor = mix(texture(myTexture, TexCoord), texture(myTexture2, TexCoord),
    0.2) * vec4(myColor, 1.0);
}
