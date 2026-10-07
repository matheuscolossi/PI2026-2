#version 330 core
in vec3 worldPos,worldNormal;
in vec2 uv;
out vec4 FragColor;
uniform vec4 Color;
uniform vec3 eye,look;
uniform sampler2D grassTex,stoneTex,normalTex;
uniform samplerCube environment;
uniform int material;
uniform bool useNormal,useFog,usePoint,useSpot,useEnvironment,useMulti;
uniform float time;
vec3 light(vec3 n,vec3 p,vec3 tint,vec3 albedo){
 vec3 delta=p-worldPos;float d=length(delta);
 return albedo*tint*max(dot(n,normalize(delta)),0.0)/(1.0+.09*d+.025*d*d);
}
void main(){
 vec3 n=normalize(worldNormal);
 vec3 an=abs(n);
 vec2 tc=an.y>max(an.x,an.z)?worldPos.xz:an.x>an.z?worldPos.zy:worldPos.xy;
 tc*=.65;
 vec3 albedo=Color.rgb;
 if(material==1){
  float coast=smoothstep(9.0,12.8,abs(worldPos.x))+smoothstep(16.0,20.8,abs(worldPos.z));
  float path=1.0-smoothstep(.8,1.9,abs(worldPos.x));
  float mask=clamp(max(coast,path),0.0,1.0);
  albedo*=useMulti?mix(texture(grassTex,tc).rgb,texture(stoneTex,tc).rgb,mask):texture(grassTex,tc).rgb;
 }else if(material==2){
  albedo*=texture(stoneTex,tc).rgb;
  if(useNormal){
   vec3 dp1=dFdx(worldPos),dp2=dFdy(worldPos);vec2 duv1=dFdx(tc),duv2=dFdy(tc);
   float det=duv1.x*duv2.y-duv1.y*duv2.x;
   if(abs(det)>0.0000001){
    vec3 t=normalize((dp1*duv2.y-dp2*duv1.y)/det);
    t=normalize(t-n*dot(n,t));
    vec3 b=normalize((dp2*duv1.x-dp1*duv2.x)/det);b=normalize(b-n*dot(n,b)-t*dot(t,b));
    vec3 mapped=texture(normalTex,tc).xyz*2.0-1.0;
    n=normalize(mat3(t,b,n)*mapped);
   }
  }
 }else if(material==3){
  float grain=.90+.10*sin(worldPos.x*39.0+sin(worldPos.z*8.0));albedo*=grain;
 }
 vec3 lighting=albedo*(.40+.25*max(dot(n,normalize(vec3(-.4,.8,.3))),0.0));
 if(usePoint){lighting+=light(n,vec3(0,2.8,2),vec3(1.6,1.14,.62),albedo);lighting+=light(n,vec3(0,2.8,-3),vec3(1.2,.87,.48),albedo);}
 if(useSpot){vec3 d=eye-worldPos;float cone=smoothstep(.84,.96,dot(normalize(-d),normalize(look)));lighting+=light(n,eye,vec3(.95,1.05,1.15)*cone,albedo);}
 float alpha=Color.a;
 if(material==4||material==5){
  vec3 I=normalize(worldPos-eye);
  if(material==4)n=normalize(n+vec3(.045*sin(worldPos.x*.8+time*.6),0,.04*cos(worldPos.z*.65+time*.5)));
  if(useEnvironment){
   vec3 r=texture(environment,reflect(I,n)).rgb*.6;
   vec3 refr=texture(environment,refract(I,n,1.0/1.5)).rgb*.6;
   float fresnel=.06+.94*pow(1.0-max(dot(-I,n),0.0),5.0);
   lighting=material==4?mix(lighting,r,.6+.3*fresnel):mix(refr,r,fresnel)*Color.rgb;
  }
 }
 if(material==6)lighting=Color.rgb;
 float fog=useFog?1.0-exp(-pow(length(eye-worldPos)*.023,1.5)):0.0;
 FragColor=vec4(mix(lighting,vec3(.19,.25,.29),fog),alpha);
}
