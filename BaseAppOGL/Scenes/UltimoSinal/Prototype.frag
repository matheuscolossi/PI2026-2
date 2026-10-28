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
uniform float time,brightness,powerPulse;

float hash(vec2 p){return fract(sin(dot(p,vec2(127.1,311.7)))*43758.5453);}
float noise(vec2 p){
    vec2 i=floor(p),f=fract(p);f=f*f*(3.0-2.0*f);
    return mix(mix(hash(i),hash(i+vec2(1,0)),f.x),mix(hash(i+vec2(0,1)),hash(i+vec2(1,1)),f.x),f.y);
}
vec3 linearColor(vec3 c){return pow(max(c,vec3(0)),vec3(2.2));}
vec3 horizonColor(){return vec3(.39,.46,.49);}
vec3 safeUnit(vec3 v){return v/max(length(v),.00001);}
vec3 filmic(vec3 c){
    // A restrained display curve, applied once after the linear lighting pass.
    c=max(c,vec3(0));
    return clamp((c*(2.51*c+.03))/(c*(2.43*c+.59)+.14),0.0,1.0);
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
    if(material==14||material==15){
        vec2 p=uv*2.0-1.0;
        float spread=max(.025,uv.y);
        float mask=material==15?exp(-dot(p,p)*5.5):exp(-p.x*p.x/(spread*spread)*4.5)*(1.0-smoothstep(.08,1.0,uv.y));
        float edge=material==15?1.0-smoothstep(.72,1.0,length(p)):smoothstep(0.0,.025,uv.y);
        float opacity=Color.a*mask*edge;
        if(opacity<.002)discard;
        FragColor=vec4(pow(filmic(linearColor(Color.rgb)*brightness),vec3(1.0/2.2)),opacity);
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
            // Per-object response preserves the authored bitmap and UV chart.
            // It is a material approximation, not measured PBR texture data.
            if(surfaceClass==1)roughness=.68; // timber
            if(surfaceClass==2)roughness=.38; // painted/rusted metal
            if(surfaceClass==3)roughness=.92; // plaster
            if(surfaceClass==4)roughness=.98; // paper
            if(surfaceClass==5)roughness=.82; // masonry
            bool exterior=abs(worldPos.x)>5.1||worldPos.z>5.15||worldPos.z< -6.15;
            if(exterior&&surfaceClass!=4){wetness=.28*(1.0-smoothstep(.1,1.3,worldPos.y));roughness=mix(roughness,.30,wetness);}
        }
    }

    albedo=linearColor(albedo);
    bool indoors=abs(worldPos.x)<5.15&&worldPos.z>-6.1&&worldPos.z<5.1&&worldPos.y<3.22;
    // Upward faces receive the overcast sky; down-facing ceilings retain the
    // weaker bounced light. A normal-dependent term gives rounded edges depth.
    float skyFacing=clamp(n.y*.5+.5,0.0,1.0);
    vec3 ambient=indoors?mix(vec3(.10,.12,.15),vec3(.25,.30,.35),skyFacing)
                         :mix(vec3(.18,.205,.21),vec3(.46,.57,.63),skyFacing);
    vec3 lighting=material==13?albedo:albedo*ambient;
    vec3 sun=normalize(vec3(-.62,.43,.65));
    if(material!=4&&material!=5&&material!=13){
      float ndl=max(dot(n,sun),0.0);
      if(ndl>0.0){
        float shade=visibility(n,sun,70.0);
        // Stable, two-direction penumbra only in the high preset. No random
        // temporal dithering, so pausing and reduced motion remain stable.
        if(quality==2&&shade<.99)shade=mix(shade,visibility(n,normalize(sun+vec3(.055,.035,-.025)),70.0),.45);
        lighting+=albedo*vec3(.66,.61,.48)*ndl*shade;
      }
    }
    if(usePoint&&material!=4&&material!=5&&material!=13){
        // The short settle belongs to the actual power-restoration event;
        // regular exposure does not flicker throughout the investigation.
        float energy=(powered?1.0:.48)+powerPulse;
        lighting+=pointLight(n,vec3(0,2.70,2.5),vec3(2.5,1.60,.82)*energy,albedo,roughness);
        lighting+=pointLight(n,vec3(2.4,2.70,-1.8),vec3(1.9,1.42,.96)*energy,albedo,roughness);
        lighting+=pointLight(n,vec3(3.48,1.69,3.02),vec3(2.5,1.21,.42)*energy,albedo,roughness);
        lighting+=pointLight(n,vec3(0,8.6,-3),vec3(1.85,1.36,.65)*energy,albedo,roughness);
        lighting+=pointLight(n,vec3(-4.65,1.95,2),vec3(.30,.62,1.1),albedo,roughness);
    }
    if(useSpot&&material!=4&&material!=5&&material!=13){
        vec3 fromEye=worldPos-eye;float cone=smoothstep(.83,.97,dot(normalize(fromEye),normalize(look)));
        // Flashlight is deliberately broad and weak enough to preserve room light.
        if(cone>.002)lighting+=pointLight(n,eye+vec3(0,-.10,0),vec3(1.0,1.13,1.14)*cone,albedo,roughness);
    }
    if(material==4||material==5){
        vec3 I=safeUnit(worldPos-eye);
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
    if(material==6)lighting=linearColor(surface.rgb)*1.6;
    // Exposure applies to scenery and sky, but never makes text illegible.
    lighting*=brightness*(indoors?.78:1.0);
    float distanceToEye=length(eye-worldPos);
    float lowMist=1.0-smoothstep(.0,7.5,(eye.y+worldPos.y)*.5);
    float fog=useFog?1.0-exp(-distanceToEye*(.0038+.009*lowMist)):0.0;
    if(indoors)fog*=.25;
    vec3 fogColor=linearColor(horizonColor());
    vec3 result=mix(lighting,fogColor*brightness,fog);
    result=filmic(result);
    FragColor=vec4(pow(max(result,vec3(0)),vec3(1.0/2.2)),alpha);
}
