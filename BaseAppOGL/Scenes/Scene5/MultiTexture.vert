#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec2 TexCoord0;
out vec2 TexCoord1;

uniform float tileS = 1.0f;
uniform float tileT = 1.0f;

void main()
{
	Normal = aNormal;

	TexCoord0 = vec2(aTexCoord.s * tileS, aTexCoord.t * tileT);
	TexCoord1 = aTexCoord;

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
