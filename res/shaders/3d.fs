#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D tex;
uniform vec3 color;
uniform float tmc;

void main() {
    FragColor = mix(texture(tex, TexCoord), vec4(color.xyz, 1.0), tmc);
}