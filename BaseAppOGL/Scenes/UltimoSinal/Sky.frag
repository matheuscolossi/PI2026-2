#version 330 core
in vec3 TexCoords;
out vec4 FragColor;
uniform samplerCube skyboxTextures;
uniform float brightness,time;
vec3 filmic(vec3 c){
    c=max(c,vec3(0));
    return clamp((c*(2.51*c+.03))/(c*(2.43*c+.59)+.14),0.0,1.0);
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
    vec3 cloud=mix(vec3(gray),source,.22)*vec3(.52,.63,.70);
    float h=max(direction.y,0.0);
    vec3 color=mix(vec3(.39,.46,.49),cloud,smoothstep(0.0,.55,h));
    vec3 horizontal=vec3(direction.x,0,direction.z);
    horizontal/=max(length(horizontal),.00001);
    float dawn=pow(max(dot(horizontal,normalize(vec3(-.62,0,.65))),0.0),5.0);
    color+=vec3(.075,.033,.005)*dawn*exp(-h*7.0);
    color=pow(color,vec3(2.2))*brightness;
    color=filmic(color);
    FragColor=vec4(pow(color,vec3(1.0/2.2)),1.0);
}
