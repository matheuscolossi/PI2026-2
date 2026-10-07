#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoord;
uniform mat4 model,view,projection;
out vec3 worldPos,worldNormal;
out vec2 uv;
void main(){
 vec4 p=model*vec4(aPos,1.0);worldPos=p.xyz;
 worldNormal=mat3(transpose(inverse(model)))*aNormal;uv=aTexCoord;
 gl_Position=projection*view*p;
}
