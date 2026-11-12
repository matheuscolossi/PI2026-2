#version 330 core
// Screen-space finish for scene 2: ambient occlusion, bloom and one display
// transform over the linear HDR target. These passes approximate contact
// shadow and glare from the image itself; they are not ray tracing, a shadow
// map or a 3ds Max bake, and the game still renders without them.
in vec2 uv;
out vec4 FragColor;
uniform int mode;               // 0 bloom down, 1 bloom up, 2 occlusion, 3 composite
uniform sampler2D source;       // scene colour or the previous bloom level
uniform sampler2D depthMap;     // resolved scene depth
uniform sampler2D occlusionMap; // raw occlusion from mode 2
uniform sampler2D bloomMap;     // accumulated bloom
uniform vec2 texel;             // size of one source texel in UV
uniform vec2 projection;        // projection[0][0], projection[1][1]
uniform vec2 depthRange;        // near, far
uniform float prefilter,aspect;
uniform int occlusionSamples;
uniform float occlusionStrength,bloomStrength,exposure,saturation,contrast,vignette,grain,grainSeed;
uniform vec3 shadowTint,highlightTint,veil;

float luminance(vec3 c){return dot(c,vec3(.2126,.7152,.0722));}
float hash(vec2 p){return fract(sin(dot(p,vec2(127.1,311.7)))*43758.5453);}
vec3 filmic(vec3 c){
    c=max(c,vec3(0));
    return clamp((c*(2.51*c+.03))/(c*(2.43*c+.59)+.14),0.0,1.0);
}
float linearDepth(float depth){
    float z=depth*2.0-1.0;
    return 2.0*depthRange.x*depthRange.y/(depthRange.y+depthRange.x-z*(depthRange.y-depthRange.x));
}
vec3 viewPosition(vec2 coordinate){
    float distanceAlongView=linearDepth(texture(depthMap,coordinate).r);
    vec2 ndc=coordinate*2.0-1.0;
    return vec3(ndc.x*distanceAlongView/projection.x,ndc.y*distanceAlongView/projection.y,-distanceAlongView);
}
vec3 bloomDown(){
    vec2 o=texel;
    vec3 sum=texture(source,uv).rgb*4.0;
    sum+=texture(source,uv+vec2(-o.x,-o.y)).rgb;
    sum+=texture(source,uv+vec2(o.x,-o.y)).rgb;
    sum+=texture(source,uv+vec2(-o.x,o.y)).rgb;
    sum+=texture(source,uv+vec2(o.x,o.y)).rgb;
    sum/=8.0;
    if(prefilter>.5){
        // Only radiance above display white feeds the glare; a soft knee and
        // a firefly clamp keep small bright texels from flickering.
        sum=min(sum,vec3(24.0));
        float peak=max(max(sum.r,sum.g),sum.b);
        float knee=clamp(peak-.82,0.0,.50);
        float soft=knee*knee;
        sum*=max(soft,peak-1.07)/max(peak,.0001);
    }
    return max(sum,vec3(0));
}
vec3 bloomUp(){
    vec2 o=texel;
    vec3 sum=texture(source,uv+vec2(-o.x*2.0,0)).rgb+texture(source,uv+vec2(o.x*2.0,0)).rgb;
    sum+=texture(source,uv+vec2(0,-o.y*2.0)).rgb+texture(source,uv+vec2(0,o.y*2.0)).rgb;
    sum+=(texture(source,uv+vec2(-o.x,o.y)).rgb+texture(source,uv+vec2(o.x,o.y)).rgb
         +texture(source,uv+vec2(-o.x,-o.y)).rgb+texture(source,uv+vec2(o.x,-o.y)).rgb)*2.0;
    return sum/12.0;
}
float occlusion(){
    float depth=texture(depthMap,uv).r;
    if(depth>=.99995)return 1.0;
    vec3 p=viewPosition(uv);
    // Geometric normal from the nearer depth neighbour on each axis, which
    // avoids borrowing a slope from the far side of a silhouette.
    vec3 right=viewPosition(uv+vec2(texel.x,0))-p,left=p-viewPosition(uv-vec2(texel.x,0));
    vec3 up=viewPosition(uv+vec2(0,texel.y))-p,down=p-viewPosition(uv-vec2(0,texel.y));
    vec3 dx=abs(right.z)<abs(left.z)?right:left;
    vec3 dy=abs(up.z)<abs(down.z)?up:down;
    vec3 normal=normalize(cross(dx,dy));
    float reach=.85; // metres
    float radius=clamp(reach*projection.y*.5/max(-p.z,.05),texel.y*3.0,.14);
    float spin=fract(52.9829189*fract(dot(gl_FragCoord.xy,vec2(.06711056,.00583715))))*6.2831853;
    float total=0.0;
    for(int i=0;i<16;++i){
        if(i>=occlusionSamples)break;
        float ring=sqrt((float(i)+.5)/float(occlusionSamples));
        float angle=spin+float(i)*2.3999632;
        vec2 offset=vec2(cos(angle)/max(aspect,.0001),sin(angle))*radius*ring;
        vec3 v=viewPosition(uv+offset)-p;
        float d2=dot(v,v);
        float facing=max(dot(normal,v)-.012*(-p.z)-.01,0.0);
        total+=facing/(d2+.012)*(1.0-smoothstep(reach*reach*.55,reach*reach*1.6,d2));
    }
    float amount=total/float(max(occlusionSamples,1))*1.15;
    return pow(clamp(1.0-amount,0.0,1.0),1.35);
}
float smoothOcclusion(){
    // Depth-aware 3x3 tent hides the per-pixel sample rotation.
    float centre=linearDepth(texture(depthMap,uv).r);
    float sum=0.0,weight=0.0;
    for(int y=-1;y<=1;++y)for(int x=-1;x<=1;++x){
        vec2 at=uv+vec2(float(x),float(y))*texel*1.5;
        float d=linearDepth(texture(depthMap,at).r);
        float w=(2.0-abs(float(x)))*(2.0-abs(float(y)))/(1.0+abs(d-centre)*(40.0/max(centre,.2)));
        sum+=texture(occlusionMap,at).r*w;weight+=w;
    }
    return sum/max(weight,.0001);
}
void main(){
    if(mode==0){FragColor=vec4(bloomDown(),1.0);return;}
    if(mode==1){FragColor=vec4(bloomUp(),1.0);return;}
    if(mode==2){FragColor=vec4(vec3(occlusion()),1.0);return;}
    vec3 color=texture(source,uv).rgb;
    if(occlusionStrength>.001){
        // Lamps and the sky keep their energy; only lit surfaces are grounded.
        float open=smoothOcclusion();
        float lit=1.0-smoothstep(.85,2.4,luminance(color));
        color*=mix(1.0,open,occlusionStrength*lit);
    }
    color+=texture(bloomMap,uv).rgb*bloomStrength;
    color=color*exposure+veil;
    color=filmic(color);
    float value=luminance(color);
    color=mix(vec3(value),color,saturation);
    color=mix(color,color*color*(3.0-2.0*color),contrast);
    color*=mix(shadowTint,highlightTint,smoothstep(.05,.75,value));
    vec2 fromCentre=(uv-.5)*vec2(min(aspect,1.9)*.82,1.0);
    color*=1.0-vignette*smoothstep(.34,.92,length(fromCentre));
    color=pow(clamp(color,0.0,1.0),vec3(1.0/2.2));
    // Fine static-or-animated grain also dithers dark gradients.
    color+=(hash(gl_FragCoord.xy+vec2(grainSeed*37.0,grainSeed*17.0))-.5)*grain;
    FragColor=vec4(clamp(color,0.0,1.0),1.0);
}
