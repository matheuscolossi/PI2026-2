#version 330 core
in vec3 TexCoords;
out vec4 FragColor;
uniform samplerCube skyboxTextures;
void main(){vec3 c=texture(skyboxTextures,TexCoords).rgb;float h=abs(normalize(TexCoords).y);FragColor=vec4(mix(vec3(.19,.25,.29),c*vec3(.45,.55,.64),smoothstep(0.0,.45,h)),1);}
