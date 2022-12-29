#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D tex;
uniform vec3 color;
uniform float fmix;
uniform int type;

void main()
{
	switch (type)
	{
		case 1:	// Only Colored (shape rendering)
			FragColor = vec4(color.xyz, 1.0);
			break;

		case 2:	// Texture and Color is mixed
			FragColor = mix(texture(tex, TexCoord), vec4(color, 1.0), fmix);
			break;

		case 3:	// Textured with a color (text rendering)
			vec4 sampled = vec4(1.0, 1.0, 1.0, texture(tex, TexCoord).r);
    		FragColor = vec4(color, 1.0) * sampled;
			break;

		default: // Only Textured (texture drawing)
			FragColor = texture(tex, TexCoord);
			break;
	}
}