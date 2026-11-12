#version 330 core
in vec3 TexCoords;
out vec4 FragColor;
uniform samplerCube skyboxTextures;
uniform float brightness,time;
uniform bool hdrOutput;
uniform float storm,clearing;
vec3 filmic(vec3 c){
    c=max(c,vec3(0));
    return clamp((c*(2.51*c+.03))/(c*(2.43*c+.59)+.14),0.0,1.0);
}
float dawnGlow(vec3 direction,float h){
    vec3 level=vec3(direction.x,0,direction.z);
    level/=max(length(level),.00001);
    float toward=pow(max(dot(level,normalize(vec3(-.62,0,.65))),0.0),3.0);
    return toward*exp(-h*4.5);
}
void main(){
    vec3 direction=normalize(TexCoords);
    // Keep the class cubemap, grading its daylight toward a wet coastal dawn.
    // A very slow cloud drift is independent of camera input and shares the
    // paused/reduced-motion world clock. It does not rotate the horizon.
    float cloudZone=smoothstep(.10,.55,direction.y);
    vec3 sampleDirection=normalize(direction+vec3(time*.00014*cloudZone,0,0));
    vec3 source=texture(skyboxTextures,sampleDirection).rgb;
    float gray=dot(source,vec3(.2126,.7152,.0722));
    vec3 cloud=mix(vec3(gray),source,.20)*vec3(.34,.43,.535);
    float h=max(direction.y,0.0);
    vec3 color=mix(vec3(.25,.32,.385),cloud,smoothstep(0.0,.55,h));
    vec3 horizontal=vec3(direction.x,0,direction.z);
    horizontal/=max(length(horizontal),.00001);
    float dawn=pow(max(dot(horizontal,normalize(vec3(-.62,0,.65))),0.0),5.0);
    color+=vec3(.150,.070,.018)*dawn*exp(-h*7.0);
    // After the report the overcast thins and the low sun warms the horizon.
    vec3 cleared=color*vec3(1.20,1.04,.86)+vec3(.20,.085,.012)*dawnGlow(direction,h);
    color=mix(color,cleared,clearing);
    // Sheet lightning brightens the cloud layer; no bolt or camera flash.
    float cloudLight=smoothstep(-.04,.55,direction.y)*(.50+.50*gray);
    color+=vec3(.40,.48,.64)*storm*cloudLight;
    color=pow(max(color,vec3(0)),vec3(2.2))*brightness;
    if(hdrOutput){FragColor=vec4(color,1.0);return;}
    color=filmic(color);
    FragColor=vec4(pow(color,vec3(1.0/2.2)),1.0);
}
