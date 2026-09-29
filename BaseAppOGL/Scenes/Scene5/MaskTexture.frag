#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord0;
in vec2 TexCoord1;
in vec2 TexCoord2;

uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform sampler2D Texture2;

void main()
{
		vec4 Tex0 = texture(Texture0, TexCoord0);
		vec4 Tex1 = texture(Texture1, TexCoord1);
		vec4 Tex2 = texture(Texture2, TexCoord2);
		
		FragColor = Tex0 * Tex1 + Tex2 * (1.0 - Tex1);
}