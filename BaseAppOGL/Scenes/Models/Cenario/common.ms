-- Native 3ds Max production helpers for O Ultimo Sinal.
-- Game coordinates are Y-up; Max coordinates are Z-up: [x,-z,y].
global USFolder, USScratch, USMats, USCounter = 0
struct USMaterials (Wood,Metal,Paint,Paper,Brass,Glass,Shoe,Chalk,Rust,Stone,Grass,Black,Luminous,Blue)
fn USG p = [p.x,-p.z,p.y]
fn USNote msg =
(
    local s=openFile (USFolder+"evidencias\\producao.txt") mode:"a"
    format "%\n" msg to:s
    close s
)
fn USName prefix = (USCounter+=1; prefix+formattedPrint USCounter format:"05d")
fn USBox p sz mat prefix:"Bx" =
(
    local n=box name:(USName prefix) width:sz.x length:sz.z height:sz.y pos:(USG (p-[0,sz.y*.5,0])) mapCoords:true
    n.pivot=USG p
    n.material=mat
    n
)
fn USCylinder p sz mat sides:20 prefix:"Cy" =
(
    local n=cylinder name:(USName prefix) radius:sz.x height:sz.y sides:sides heightsegs:1 capsegs:1 pos:(USG (p-[0,sz.y*.5,0])) mapCoords:true
    n.scale=[1,sz.z/sz.x,1]
    n.pivot=USG p
    n.material=mat
    n
)
fn USCone p sz mat prefix:"Co" =
(
    -- A tiny real top avoids zero-area apex faces in the legacy 3DS exporter.
    local n=cone name:(USName prefix) radius1:sz.x radius2:.003 height:sz.y sides:24 heightsegs:1 capsegs:1 pos:(USG (p-[0,sz.y*.5,0])) mapCoords:true
    n.scale=[1,sz.z/sz.x,1]
    n.pivot=USG p
    n.material=mat
    n
)
fn USPlane p sx sz mat prefix:"Pl" =
(
    local n=plane name:(USName prefix) width:sx length:sz widthsegs:1 lengthsegs:1 pos:(USG p)
    n.material=mat
    n
)
fn USRot n degrees axis = (rotate n (angleaxis degrees (normalize (USG axis))); n)
fn USBeam a b r mat =
(
    local d=b-a
    local lengthD=length d
    if lengthD<0.00001 then return undefined
    local n=USCylinder ((a+b)*.5) [r,lengthD,r] mat sides:12
    local direction=normalize (USG d)
    local angle=acos (amax -1.0 (amin 1.0 (dot [0,0,1] direction)))
    local axis=cross [0,0,1] direction
    if length axis<0.0001 do axis=[1,0,0]
    rotate n (angleaxis angle (normalize axis))
    n
)
fn USUnwrap n =
(
    select n
    max modify mode
    -- Reset XForm leaves an editable transform modifier below the UV modifier.
    -- UVs therefore see physical dimensions, including non-uniform scale.
    resetXForm n
    local uv=Unwrap_UVW()
    addModifier n uv
    modPanel.setCurrentObject uv
    uv.unwrap2.setTVSubObjectMode 3
    local faces=#{}
    for i=1 to uv.unwrap.numberPolygons() do faces[i]=true
    uv.unwrap2.selectPolygonsUpdate faces true
    uv.unwrap2.flattenMap 45.0 #() 0.018 true 0 true true
    subObjectLevel=0
    uv
)
fn USFinish key nodes unwrap:true =
(
    if nodes.count==0 do throw ("Empty asset: "+key)
    if unwrap do for i=1 to nodes.count do
    (
        if mod i 10==1 do USNote ("UV "+key+" "+(i as string)+"/"+(nodes.count as string))
        USUnwrap nodes[i]
    )
    select nodes
    local exported=USScratch+key+".3ds"
    exportFile exported #noPrompt selectedOnly:true
    if not doesFileExist exported do throw ("3DS export failed: "+key)
    if doesFileExist (USFolder+key+".3ds") do throw ("Existing asset protected: "+key)
    if not copyFile exported (USFolder+key+".3ds") do throw ("Could not copy export: "+key)
    local triCount=0
    for n in nodes do (local meshCopy=snapshotAsMesh n; triCount+=meshCopy.numfaces)
    local root=group nodes name:("US_"+key)
    USNote ("EXPORTED "+key+" nodes="+nodes.count as string+" triangles="+triCount as string)
    saveMaxFile (USFolder+"cenario.max") quiet:true
    root
)
fn USNoiseMaterial key c rough:0.12 =
(
    local destination=USFolder+key+".jpg"
    if not doesFileExist destination do
    (
        local procedural=noise name:(key+"_native_noise") color1:(c*(1.0-rough)) color2:c size:12 type:1 levels:3
        local temp=USScratch+key+".jpg"
        local b=renderMap procedural size:[256,256] filename:temp scale:64 display:false
        save b
        close b
        if not doesFileExist temp do throw ("Texture not rendered: "+key)
        copyFile temp destination
    )
    local tex=bitmapTexture filename:destination name:(key+"_bitmap")
    local mat=standardMaterial name:key diffuse:(color 255 255 255) diffuseMap:tex specularLevel:12 glossiness:18
    showTextureMap mat tex on
    mat
)
fn USMakeMaterials =
(
    USMats=USMaterials()
    USMats.Wood=standardMaterial name:"wood" diffuse:(color 255 255 255) diffuseMap:(bitmapTexture filename:(USFolder+"wood.jpg")) glossiness:12 specularLevel:8
    showTextureMap USMats.Wood USMats.Wood.diffuseMap on
    USMats.Metal=USNoiseMaterial "iron" (color 49 60 62) rough:.10
    USMats.Paint=USNoiseMaterial "paint" (color 65 91 85) rough:.12
    USMats.Paper=USNoiseMaterial "paper" (color 207 198 167) rough:.035
    USMats.Brass=USNoiseMaterial "brass" (color 150 124 73) rough:.08
    USMats.Shoe=USNoiseMaterial "shoe" (color 40 33 27) rough:.10
    USMats.Chalk=USNoiseMaterial "chalk" (color 198 199 178) rough:.055
    USMats.Rust=USNoiseMaterial "rust" (color 120 70 43) rough:.25
    USMats.Stone=USNoiseMaterial "stone" (color 117 123 112) rough:.18
    USMats.Grass=USNoiseMaterial "grass" (color 135 153 117) rough:.08
    USMats.Black=USNoiseMaterial "black" (color 23 26 24) rough:.06
    USMats.Blue=USNoiseMaterial "blue" (color 41 77 120) rough:.10
    USMats.Glass=standardMaterial name:"glass" diffuse:(color 255 255 255) opacity:100 glossiness:55 specularLevel:55
    USMats.Luminous=standardMaterial name:"light" diffuse:(color 255 255 255) selfIllumAmount:100
    USNote "Standard materials prepared; procedural maps rendered by native Max to portable bitmap files."
)
