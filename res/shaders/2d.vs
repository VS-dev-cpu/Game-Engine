#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
uniform vec2 center;
uniform vec2 size;

void main()
{
    float x = center.x + aPos.x * size.x;
    float y = center.y + aPos.y * size.y;
    gl_Position = vec4(x, y, 0.0, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}