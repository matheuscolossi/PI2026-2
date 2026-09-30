#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D Texture0;
uniform vec4 Color = vec4(1.0, 1.0, 1.0, 1.0);
uniform bool isTextured = false;

void main()
{
	if (isTextured)
		FragColor = texture(Texture0, TexCoord);
	else
		FragColor = Color;
}