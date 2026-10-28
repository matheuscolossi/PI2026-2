#version 330 core
in vec3 worldPos,worldNormal;
in vec2 uv;
out vec4 FragColor;
uniform bool uHasDiffuse;
uniform sampler2D uDiffuseMap;
uniform vec4 uMaterialTint;
void main(){
 vec4 base=uMaterialTint;
 if(uHasDiffuse)base*=texture(uDiffuseMap,uv);
 if(base.a<.06)discard;
 vec3 n=normalize(worldNormal);
 if(!gl_FrontFacing)n=-n;
 vec3 key=normalize(vec3(-.7,1.3,1.6));
 vec3 fill=normalize(vec3(1.5,.5,-.4));
 vec3 eye=normalize(vec3(0,1.65,2.65)-worldPos);
 float spec=pow(max(dot(n,normalize(key+eye)),0.),36.);
 vec3 light=vec3(.40,.44,.45)+vec3(.76,.69,.53)*max(dot(n,key),0.)+vec3(.17,.26,.29)*max(dot(n,fill),0.);
 vec3 color=base.rgb*light+spec*.11;
 color=color/(color+vec3(.28));
 FragColor=vec4(color,base.a);
}
