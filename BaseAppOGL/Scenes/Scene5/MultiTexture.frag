#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord0;
in vec2 TexCoord1;

uniform sampler2D Texture0;
uniform sampler2D Texture1;

uniform float MixTextures = 0.2;

void main()
{
		vec4 Tex0 = texture(Texture0, TexCoord0);
		vec4 Tex1 = texture(Texture1, TexCoord1);
		
		//FragColor = mix(Tex0, Tex1, MixTextures);
		FragColor = Tex0 * Tex1;
}