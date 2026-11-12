#version 330 core
in vec3 worldPos,worldNormal;
in vec2 uv;
out vec4 FragColor;
uniform vec4 Color;
uniform vec3 eye,look;
uniform sampler2D grassTex,stoneTex,normalTex;
uniform sampler2D uDiffuseMap;
uniform sampler2D uNormalMap,uLightMap;
uniform bool uImportedMaterial,uHasDiffuse,uHasNormal,uHasLightmap;
uniform vec4 uMaterialTint;
uniform samplerCube environment;
uniform int material,quality,blockerCount,surfaceClass;
uniform vec3 blockerMin[32],blockerMax[32];
uniform vec3 shadowBoundsMin,shadowBoundsMax;
uniform bool useNormal,useFog,usePoint,useSpot,useEnvironment,useMulti,powered;
uniform float time,brightness,powerPulse,resolutionWarmth;
uniform bool rainMotion;
// Presentation uniforms. All default to zero: the direct path below then
// reproduces the previous single-pass image when post-processing is off.
uniform bool hdrOutput;
uniform float emissiveBoost,storm,lensFlash,dawn,lampSurge;
uniform vec4 focusPoint; // xyz: aimed interaction in world space, w: strength
uniform float focusRadius;

float hash(vec2 p){return fract(sin(dot(p,vec2(127.1,311.7)))*43758.5453);}
float noise(vec2 p){
    vec2 i=floor(p),f=fract(p);f=f*f*(3.0-2.0*f);
    return mix(mix(hash(i),hash(i+vec2(1,0)),f.x),mix(hash(i+vec2(0,1)),hash(i+vec2(1,1)),f.x),f.y);
}
vec3 linearColor(vec3 c){return pow(max(c,vec3(0)),vec3(2.2));}
vec3 horizonColor(){return vec3(.25,.32,.385);}
vec3 safeUnit(vec3 v){return v/max(length(v),.00001);}
vec3 filmic(vec3 c){
    // A restrained display curve, applied once after the linear lighting pass.
    c=max(c,vec3(0));
    return clamp((c*(2.51*c+.03))/(c*(2.43*c+.59)+.14),0.0,1.0);
}
// With post-processing the scene target keeps linear HDR radiance; exposure,
// bloom, the display curve and gamma are then applied once in Post.frag.
vec4 present(vec3 radiance,float opacity){
    if(hdrOutput)return vec4(max(radiance,vec3(0)),opacity);
    return vec4(pow(filmic(radiance),vec3(1.0/2.2)),opacity);
}
vec2 coordinates(vec3 n){
    vec3 a=abs(n);
    return a.y>max(a.x,a.z)?worldPos.xz:a.x>a.z?worldPos.zy:worldPos.xy;
}
// Visibility against explicit architectural/furniture volumes. This is an
// analytical approximation, not a shadow map or baked 3ds Max light texture.
float visibility(vec3 normal,vec3 direction,float limit){
    if(blockerCount<=0)return 1.0;
    vec3 origin=worldPos+normal*.036;
    vec3 inv=(step(vec3(0),direction)*2.0-1.0)/max(abs(direction),vec3(.0001));
    // Reject rays missing the CPU-computed union before visiting each blocker.
    vec3 outer0=(shadowBoundsMin-origin)*inv,outer1=(shadowBoundsMax-origin)*inv;
    vec3 outerLow=min(outer0,outer1),outerHigh=max(outer0,outer1);
    float outerEnter=max(max(outerLow.x,outerLow.y),outerLow.z);
    float outerLeave=min(min(outerHigh.x,outerHigh.y),outerHigh.z);
    if(outerLeave<=max(outerEnter,.001)||outerEnter>=limit||outerLeave<=.001)return 1.0;
    for(int i=0;i<32;++i){
        if(i>=blockerCount)break;
        vec3 t0=(blockerMin[i]-origin)*inv,t1=(blockerMax[i]-origin)*inv;
        vec3 low=min(t0,t1),high=max(t0,t1);
        float enter=max(max(low.x,low.y),low.z),leave=min(min(high.x,high.y),high.z);
        if(leave>max(enter,.001)&&enter<limit&&leave>.001)return .10;
    }
    return 1.0;
}
vec3 pointLight(vec3 n,vec3 location,vec3 tint,vec3 albedo,float roughness){
    vec3 delta=location-worldPos;float dist=length(delta);vec3 l=delta/max(dist,.001);
    if(dist>13.5)return vec3(0);
    float diffuse=max(dot(n,l),0.0);
    if(diffuse<=0.0)return vec3(0);
    float shadow=visibility(n,l,dist-.05);
    if(quality==2)shadow=(shadow+visibility(n,normalize(delta+vec3(.24,.12,.18)),dist-.05)+visibility(n,normalize(delta+vec3(-.21,.15,-.18)),dist-.05))/3.0;
    vec3 halfVector=safeUnit(l+safeUnit(eye-worldPos));
    float specular=pow(max(dot(n,halfVector),0.0),mix(120.0,14.0,roughness))*(1.0-roughness)*.32;
    return (albedo*diffuse+vec3(specular))*tint*shadow/(1.0+.10*dist+.052*dist*dist);
}
// Pendant and desk lamps sit under a shade: most of their light leaves
// downward, so ceilings and upper walls fall away from the lit pool.
float lampHood(vec3 location,float low,float high){
    vec3 fromLamp=safeUnit(worldPos-location);
    return smoothstep(low,high,-fromLamp.y);
}
vec3 mappedNormal(vec3 n,vec2 tc,sampler2D normalSampler){
    vec3 p1=dFdx(worldPos),p2=dFdy(worldPos);vec2 t1=dFdx(tc),t2=dFdy(tc);
    float det=t1.x*t2.y-t1.y*t2.x;
    if(abs(det)<.0000001)return n;
    vec3 tangent=(p1*t2.y-p2*t1.y)/det;
    tangent=normalize(tangent-n*dot(n,tangent));
    vec3 bitangent=normalize(cross(n,tangent))*sign(det);
    vec3 sampleN=texture(normalSampler,tc).xyz*2.0-1.0;sampleN.xy*=.35;
    return normalize(mat3(tangent,bitangent,n)*normalize(sampleN));
}
void main(){
    // Small camera-facing atmospheric particles and soft beam cards. Their
    // depth is tested against actual scenery; they never replace evidence art.
    if(material==14||material==15||material==16){
        vec2 p=uv*2.0-1.0;
        float spread=max(.025,uv.y);
        float mask=material==15?exp(-dot(p,p)*5.5):exp(-p.x*p.x/(spread*spread)*4.5)*(1.0-smoothstep(.08,1.0,uv.y));
        float edge=material==15?1.0-smoothstep(.72,1.0,length(p)):smoothstep(0.0,.025,uv.y);
        if(material==16){mask=exp(-p.x*p.x*7.0)*(1.0-smoothstep(.25,1.0,abs(p.y)));edge=1.0;}
        float opacity=Color.a*mask*edge;
        if(opacity<.002)discard;
        // Scattering cards carry a little extra energy for the bloom pass only.
        float glow=hdrOutput?(material==14?2.4:material==15?1.7:1.0):1.0;
        if(material==16)opacity*=1.0-.92*dawn;
        FragColor=present(linearColor(Color.rgb)*brightness*glow*(1.0+.9*storm),opacity);
        return;
    }
    vec3 geometricNormal=normalize(worldNormal),n=geometricNormal;
    if(!gl_FrontFacing)n=-n;
    vec4 surface=Color;
    if(uImportedMaterial)surface*=uMaterialTint;
    vec2 tc=uImportedMaterial?uv:coordinates(n);vec3 albedo=surface.rgb;float roughness=.85;
    float alpha=surface.a;
    vec4 importedBitmap=uImportedMaterial&&uHasDiffuse?texture(uDiffuseMap,uv):vec4(1.0);
    float broad=noise(tc*.34),fine=noise(tc*6.0);
    float wetness=0.0;
    if(material==1){
        // A sixth-power contour keeps every rectangular walking corner on land.
        vec2 q=abs(worldPos.xz)/vec2(15.35,23.85);
        float boundary=pow(q.x,6.0)+pow(q.y,6.0);
        float erosion=.035*sin(worldPos.x*1.8+worldPos.z*.72)+.025*noise(worldPos.xz*1.3);
        if(boundary>1.0+erosion)discard;
        float shore=smoothstep(.59,1.02,boundary);
        float path=(1.0-smoothstep(1.0,2.15,abs(worldPos.x)))*smoothstep(4.5,6.0,worldPos.z);
        // Flatten's island UV chart has a 56m longest edge. Restore its
        // authored physical texel scale without projecting from world position.
        vec2 terrainUV=uImportedMaterial?uv*56.0:tc;
        vec3 grass=texture(grassTex,terrainUV*.38).rgb*vec3(.58,.63,.51);
        vec3 stone=texture(stoneTex,terrainUV*.45).rgb*vec3(.76,.80,.77);
        albedo*=useMulti?mix(grass,stone,max(shore,path)*.87):grass;
        wetness=shore*.66+path*(.15+.15*noise(worldPos.xz*.9));
        albedo*=.86+.18*broad;roughness=mix(.95,.30,wetness);
    }else if(material==2){
        if(uImportedMaterial){
            albedo*=importedBitmap.rgb;alpha*=importedBitmap.a;
            if(useNormal){
                if(uHasNormal)n=mappedNormal(n,uv,uNormalMap);
                else n=mappedNormal(n,uv,normalTex);
            }
        }else{
            vec3 stone=texture(stoneTex,tc*.55).rgb;
            albedo*=mix(vec3(.80),stone,.50)*(.90+.16*broad);
            if(useNormal)n=mappedNormal(n,tc*.55,normalTex);
        }
        wetness=clamp((.7-worldPos.y)*.9,0.0,.6);roughness=mix(.87,.34,wetness);
    }else if(material==3){
        // Procedural grain is deliberately labelled as such, not a Max UV bake.
        float grain=noise(vec2(tc.x*2.0,tc.y*31.0));
        float seam=1.0-smoothstep(.015,.035,abs(fract(tc.x*2.2)-.5));
        albedo*=.80+.23*grain;albedo*=1.0-.055*seam;roughness=.72;
    }else if(material==7){
        float damp=(1.0-smoothstep(.0,.8,worldPos.y))*(.12+.08*broad);
        albedo*=.91+.09*broad+.025*fine-damp;
    }else if(material==8){
        float seam=smoothstep(.90,.97,fract(worldPos.z*3.7));
        albedo*=.83+.14*broad-.12*seam;roughness=.52;
    }else if(material==9){albedo*=.94+.06*fine;roughness=1.0;}
    else if(material==10){albedo*=.90+.10*broad;roughness=.98;}
    else if(material==11){albedo*=.85+.15*broad;roughness=.42;}
    else if(material==12||material==13){
        // Author-exported bitmap sampled with the model's Unwrap UVW channel.
        // No world-space wood grain or procedural overlay is applied here.
        vec4 bitmap=uImportedMaterial?importedBitmap:texture(uDiffuseMap,uv);
        // CompleteMap is already shaded. An optional imported lightmap slot can
        // supply that complete atlas; otherwise the diffuse bitmap is the atlas.
        if(material==13&&uImportedMaterial&&uHasLightmap)bitmap=texture(uLightMap,uv);
        albedo*=bitmap.rgb;alpha*=bitmap.a;roughness=.76;
        if(material==12&&uImportedMaterial&&uHasNormal&&useNormal)n=mappedNormal(n,uv,uNormalMap);
        if(material==12){
            // Per-object response preserves UV charts. Surface wear below is a
            // restrained procedural finish over the bitmap, not new Max baking.
            // It is a material approximation, not measured PBR texture data.
            if(surfaceClass==1)roughness=.68; // timber
            if(surfaceClass==2)roughness=.38; // painted/rusted metal
            if(surfaceClass==3)roughness=.92; // plaster
            if(surfaceClass==4)roughness=.98; // paper
            if(surfaceClass==5)roughness=.82; // masonry
            bool exterior=abs(worldPos.x)>5.1||worldPos.z>5.15||worldPos.z< -6.15;
            if(exterior&&surfaceClass!=4){wetness=.28*(1.0-smoothstep(.1,1.3,worldPos.y));roughness=mix(roughness,.30,wetness);}
            if(quality>0){
                float age=noise(worldPos.xz*2.1+worldPos.y*.71);
                float vertical=1.0-smoothstep(.40,.78,abs(n.y));
                if(surfaceClass==3){
                    float risingDamp=exp(-max(worldPos.y-.10,0.0)*2.1)*vertical;
                    float salt=smoothstep(.66,.91,noise(vec2(worldPos.x+worldPos.z,worldPos.y)*8.0));
                    albedo*=mix(vec3(1),vec3(.62,.69,.66),risingDamp*(.10+.15*age));
                    albedo*=.985+.015*age;albedo+=vec3(.006,.006,.004)*salt*vertical;
                }
                if(surfaceClass==2){float patina=smoothstep(.60,.86,age);albedo*=1.0-patina*.08;roughness=clamp(roughness+patina*.17,.2,.95);}
                if(surfaceClass==1){albedo*=.98+.02*age;roughness=clamp(roughness+(age-.5)*.12,.3,.9);}
            }
        }
    }

    albedo=linearColor(albedo);
    bool indoors=abs(worldPos.x)<5.15&&worldPos.z>-6.1&&worldPos.z<5.1&&worldPos.y<3.22;
    // Upward faces receive the overcast sky; down-facing ceilings retain the
    // weaker bounced light. A normal-dependent term gives rounded edges depth.
    float skyFacing=clamp(n.y*.5+.5,0.0,1.0);
    // 05h40 under a retreating storm: a dim, cold sky fill. The rooms rely on
    // their own lamps, so walls fall off toward the corners instead of reading
    // as evenly lit planes.
    vec3 ambient=indoors?mix(vec3(.046,.060,.088),vec3(.105,.140,.190),skyFacing)
                         :mix(vec3(.080,.100,.128),vec3(.215,.285,.365),skyFacing);
    if(indoors){
        // A low bounce off the lit floor keeps shaded ceilings readable: warm
        // in the service room, cold in the archive. It is an ambient estimate.
        float coldRoom=1.0-smoothstep(-.95,-.45,worldPos.z);
        ambient+=mix(vec3(.060,.043,.026),vec3(.030,.044,.062),coldRoom)*(powered?1.0:.45)*(1.0+lampSurge);
    }
    // The epilogue clears the overcast; a lightning strike is a short cold fill.
    ambient*=mix(vec3(1.0),indoors?vec3(1.10,1.03,.95):vec3(1.22,1.07,.90),dawn);
    ambient+=(indoors?vec3(.085,.11,.16):vec3(.50,.62,.86)*(.35+.65*skyFacing))*storm;
    // The floor atlas is already shaded by 3ds Max. It only receives the same
    // exposure as the room around it; no Point/Spot term is added to the bake.
    vec3 lighting=material==13?albedo*((powered?.84:.58)+.16*storm):albedo*ambient;
    vec3 sun=normalize(vec3(-.62,.43,.65));
    if(material!=4&&material!=5&&material!=13){
      float ndl=max(dot(n,sun),0.0);
      if(ndl>0.0){
        float shade=visibility(n,sun,70.0);
        // Stable, two-direction penumbra only in the high preset. No random
        // temporal dithering, so pausing and reduced motion remain stable.
        if(quality==2&&shade<.99)shade=mix(shade,visibility(n,normalize(sun+vec3(.055,.035,-.025)),70.0),.45);
        lighting+=albedo*mix(vec3(.34,.33,.33),vec3(1.30,.88,.50),dawn)*ndl*shade;
      }
    }
    if(usePoint&&material!=4&&material!=5&&material!=13){
        // The short settle belongs to the actual power-restoration event;
        // regular exposure does not flicker throughout the investigation.
        float energy=max(0.0,((powered?1.0:.48)+powerPulse)*(1.0+lampSurge));
        lighting+=pointLight(n,vec3(0,2.70,2.5),vec3(2.9,1.62,.68)*energy*lampHood(vec3(0,2.70,2.5),-.22,.50),albedo,roughness);
        lighting+=pointLight(n,vec3(2.4,2.70,-1.8),vec3(.98,1.36,1.90)*energy*lampHood(vec3(2.4,2.70,-1.8),-.22,.50),albedo,roughness);
        lighting+=pointLight(n,vec3(3.48,1.69,3.02),vec3(3.6,1.52,.46)*energy*(.30+.70*lampHood(vec3(3.48,1.69,3.02),-.05,.55)),albedo,roughness);
        lighting+=pointLight(n,vec3(0,8.6,-3),mix(vec3(1.65,1.06,.38),vec3(2.0,1.42,.65),resolutionWarmth)*energy*(1.0+2.4*lensFlash),albedo,roughness);
        lighting+=pointLight(n,vec3(-4.65,1.95,2),vec3(.24,.48,.88),albedo,roughness);
        lighting+=pointLight(n,vec3(4.70,2.10,-3.5),vec3(.26,.50,.82),albedo,roughness);
    }
    if(useSpot&&material!=4&&material!=5&&material!=13){
        // Hand torch: a soft outer cone with a tighter, warmer core, so the
        // Spot reads as a beam on the wall instead of an even room fill.
        vec3 fromEye=worldPos-eye;float axis=dot(normalize(fromEye),normalize(look));
        float cone=smoothstep(.875,.972,axis),core=smoothstep(.958,.996,axis);
        if(cone>.002)lighting+=pointLight(n,eye+vec3(0,-.10,0),vec3(1.0,.98,.90)*(cone*.62+core*.50),albedo,roughness);
    }
    if(material==4||material==5){
        vec3 I=safeUnit(worldPos-eye);
        float rain=0.0;
        if(material==5&&quality>0){
            // World-anchored moisture never swims with the camera. Only the
            // droplet clock is frozen by reduced motion, not the sky horizon.
            vec2 glass=vec2(worldPos.z*.73+worldPos.x*.51,worldPos.y);
            float lane=floor(glass.x*18.0),seed=hash(vec2(lane,3.4));
            float across=abs(fract(glass.x*18.0)-.5);
            float drop=fract(glass.y*.62+(rainMotion?time*.13:0.0)+seed*7.0);
            rain=pow(max(0.0,1.0-across*13.0),2.0)*smoothstep(.46,.70,drop)*(1.0-smoothstep(.78,1.0,drop));
            n=normalize(n+vec3(rain*.025,-rain*.035,rain*.018));
            alpha=clamp(alpha+rain*.18,0.0,.47);
        }
        if(material==4){
            // Two swell directions and a short wind ripple. Mesh and camera
            // stay fixed; reduced motion freezes this supplied simulation time.
            float swellX=sin(worldPos.x*.52+worldPos.z*.23+time*.51)+.42*sin(worldPos.z*1.7-time*.61);
            float swellZ=cos(worldPos.z*.47-worldPos.x*.18+time*.41)+.31*cos(worldPos.x*1.9+time*.74);
            float ripple=sin(worldPos.x*5.4+worldPos.z*3.7+time*1.3);
            n=normalize(vec3(.039*swellX+.007*ripple,1,.033*swellZ));
            lighting=albedo*vec3(.47,.69,.78);
        }
        if(useEnvironment){
            vec3 reflectionSample=texture(environment,reflect(I,n)).rgb;
            float gray=dot(reflectionSample,vec3(.2126,.7152,.0722));
            vec3 reflection=linearColor(mix(vec3(gray),reflectionSample,.35)*vec3(.56,.67,.72));
            vec3 refraction=linearColor(texture(environment,refract(I,n,1.0/1.5)).rgb)*vec3(.40,.56,.61);
            float fresnel=.04+.96*pow(1.0-clamp(dot(-I,n),0.0,1.0),5.0);
            lighting=material==4?mix(lighting,reflection,.40+.45*fresnel):mix(refraction,reflection,fresnel)*surface.rgb;
            if(material==5)lighting+=vec3(.055,.078,.085)*rain;
            if(material==4){
                float glint=pow(max(dot(reflect(-sun,n),normalize(eye-worldPos)),0.0),110.0);
                lighting+=vec3(.30,.29,.23)*glint;
                // Shore foam follows the same sixth-power island boundary.
                // This is a shader approximation, not simulated water depth.
                vec2 shoreQ=abs(worldPos.xz)/vec2(16.1,24.7);
                float contour=pow(shoreQ.x,6.0)+pow(shoreQ.y,6.0);
                float breakup=noise(worldPos.xz*1.65+vec2(time*.14,-time*.09));
                float band=exp(-abs(contour-1.0-sin(time*.48+worldPos.x*.2)*.025)*30.0);
                float foam=band*smoothstep(.36,.77,breakup)*.38;
                lighting=mix(lighting,vec3(.38,.46,.45),foam);
                if(powered){
                    vec3 fromLens=worldPos-vec3(0,8.10,-3);
                    vec3 sweepDirection=normalize(vec3(cos(time*.18+.47),-.14,sin(time*.18+.47)));
                    float sweep=smoothstep(.985,.998,dot(safeUnit(fromLens),sweepDirection));
                    lighting+=vec3(.62,.39,.13)*sweep/(1.0+length(fromLens)*.025);
                }
            }
        }
    }
    if(wetness>.001&&useEnvironment&&material!=13){
        vec3 v=safeUnit(eye-worldPos);
        float fresnel=.04+.96*pow(1.0-clamp(dot(n,v),0.0,1.0),5.0);
        vec3 reflected=texture(environment,reflect(-v,n)).rgb;
        float lum=dot(reflected,vec3(.2126,.7152,.0722));
        lighting+=linearColor(mix(vec3(lum),reflected,.25)*vec3(.55,.64,.67))*wetness*fresnel*.46;
    }
    if(material==6){
        // Emissive lamps exceed display white only in the HDR target, where the
        // bloom pass turns that excess into a soft halo around the fixture.
        float emission=hdrOutput?emissiveBoost:1.6;
        emission*=max(.15,1.0+lampSurge);
        if(distance(worldPos,vec3(0,8.10,-3))<1.25)emission*=1.0+2.2*lensFlash;
        lighting=linearColor(surface.rgb)*emission;
    }
    // Aim feedback: a warm pool around the interaction the player is facing.
    // It is an interface affordance, so the baked floor atlas is left untouched.
    if(focusPoint.w>.001&&material!=4&&material!=5&&material!=6&&material!=13){
        vec3 toFocus=worldPos-focusPoint.xyz;
        float pool=focusPoint.w*exp(-dot(toFocus,toFocus)/max(focusRadius*focusRadius,.0004));
        vec3 toEye=safeUnit(eye-worldPos);
        float rim=pow(1.0-clamp(dot(n,toEye),0.0,1.0),2.0);
        lighting+=(albedo*vec3(1.0,.72,.34)*.95+vec3(.90,.60,.22)*rim*.22)*pool;
    }
    // Exposure applies to scenery and sky, but never makes text illegible.
    // Room grading is a color treatment over both baked floor and real-time
    // objects; it does not claim to change the baked light/shadow solution.
    if(indoors){
        float archive=1.0-smoothstep(-.95,-.45,worldPos.z);
        lighting*=mix(vec3(1.025,.965,.86),vec3(.80,.93,1.08),archive);
    }else if(worldPos.y>7.25&&length(worldPos.xz-vec2(0,-3))<2.5){
        lighting*=mix(vec3(.86,.96,1.05),vec3(1.0,.98,.91),resolutionWarmth*.48);
    }
    lighting*=brightness*(indoors?.78:1.0);
    float distanceToEye=length(eye-worldPos);
    float lowMist=1.0-smoothstep(.0,7.5,(eye.y+worldPos.y)*.5);
    float fog=useFog?1.0-exp(-distanceToEye*(.0050+.0125*lowMist)*(1.0-.42*dawn)):0.0;
    if(indoors)fog*=.25;
    vec3 fogColor=linearColor(mix(horizonColor(),vec3(.66,.58,.48),dawn*.85))*(1.0+1.4*storm);
    vec3 result=mix(lighting,fogColor*brightness,fog);
    FragColor=present(result,alpha);
}
