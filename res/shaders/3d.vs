#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

//attribute vec3 aPos;
//attribute vec2 aTexCoord;
//attribute vec3 aNormal;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    TexCoord = aTexCoord;

    gl_Position = projection * view * model * vec4(aPos.xyz, 1.0);
}