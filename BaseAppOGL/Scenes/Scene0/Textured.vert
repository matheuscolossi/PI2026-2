#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float fTileS = 1.0f;
uniform float fTileT = 1.0f;

void main()
{
	// Tile Texture
	TexCoord = vec2(aTex.s * fTileS, aTex.t * fTileT);

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
