#version 330 core

out vec4 FragColor;

in vec3 vertexColor;
in vec2 vertexTexCoord;

uniform sampler2D Texture0;
uniform bool bBlendEnabled = false; 
uniform float fAlpha = 1.0;

void main()
{
	vec4 texSample = texture(Texture0, vertexTexCoord);

	//FragColor = vec4(vertexColor, 1.0);

	if (bBlendEnabled)
		FragColor = texSample * vec4(1.0, 1.0, 1.0, fAlpha);
	else
		FragColor = texSample * vec4(1.0, 1.0, 1.0, 1.0);
}