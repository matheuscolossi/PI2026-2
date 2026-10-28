-- Authentic native Scanline Render To Texture, CompleteMap on non-overlapping UV1.
-- Dynamic leaves/key do not cast frozen shadows into the floor atlas.
global USBakeFloor, USBakeShadow, USBakeClear
fn USExtraTable key p depth =
(
    if getNodeByName key!=undefined do return false
    local a=#()
    USAdd a (USBox (p+[0,.94,0]) [1.6,.12,depth] USMats.Wood)
    for x in #(-.67,.67) do for z in #(-depth*.36,depth*.36) do USAdd a (USBox (p+[x,.44,z]) [.1,.88,.1] USMats.Wood)
    for n in a do USUnwrap n
    group a name:key
    true
)
fn USNativeBake =
(
    if doesFileExist (USFolder+"p_bake.jpg") do throw "Existing bake protected; review before replacing it."
    USExtraTable "US_MesaManut" [-3,.095,1] .9
    USExtraTable "US_MesaReg" [-3,.095,-3] .9
    USExtraTable "US_MesaFoto" [3,.095,-3] .9
    USExtraTable "US_MesaRadio" [3,.095,2.5] 1.95
    local floorRoot=getNodeByName "US_piso"
    if floorRoot==undefined do throw "Native floor is missing."
    USBakeFloor=floorRoot.children[1]
    local floorWood=copy USMats.Wood
    floorWood.name="floorpre"
    floorWood.diffuseMap=copy USMats.Wood.diffuseMap
    floorWood.diffuseMap.coordinates.uTiling=8
    floorWood.diffuseMap.coordinates.vTiling=8
    USBakeFloor.material=floorWood
    for name in #("US_porta","US_armporta","US_chave","US_agua","US_janvid","US_vidfar","US_led","US_lampada") do
    (
        local root=getNodeByName name
        if root!=undefined do for n in root.children do n.castShadows=false
    )
    renderers.current=Default_Scanline_Renderer()
    ambientColor=color 38 43 46
    for entry in #(#("US_BakeLight1",[0,2.70,2.5]),#("US_BakeLight2",[0,2.70,-3])) do
    (
        local lamp=getNodeByName entry[1]
        if lamp==undefined do lamp=omniLight name:entry[1] pos:(USG entry[2]) rgb:(color 255 220 177) multiplier:.52
        lamp.baseObject.castShadows=true
    )
    USBakeFloor.INodeBakeProperties.removeAllBakeElements()
    USBakeShadow=CompleteMap()
    USBakeClear=CompleteMap()
    for pair in #(#(USBakeShadow,"p_bake.jpg",true),#(USBakeClear,"p_nosha.jpg",false)) do
    (
        local element=pair[1]
        element.outputSzX=1024;element.outputSzY=1024
        element.fileType=USScratch+pair[2]
        element.fileName=pair[2]
        element.filterOn=true;element.shadowsOn=pair[3];element.enabled=true
        USBakeFloor.INodeBakeProperties.addBakeElement element
    )
    USBakeFloor.INodeBakeProperties.bakeEnabled=true
    USBakeFloor.INodeBakeProperties.bakeChannel=1
    USBakeFloor.INodeBakeProperties.nDilations=3
    saveMaxFile (USFolder+"cenario.max") quiet:true
    select USBakeFloor
    USNote "BAKE begin native Scanline CompleteMap 1024 UV1, paired shadows on/off, static room lights"
    render rendertype:#bakeSelected vfb:off progressBar:true outputSize:[1024,1024]
    for name in #("p_bake.jpg","p_nosha.jpg") do if not doesFileExist (USScratch+name) do throw ("Bake output absent "+name)
    copyFile (USScratch+"p_bake.jpg") (USFolder+"p_bake.jpg")
    copyFile (USScratch+"p_nosha.jpg") (USFolder+"evidencias\\p_nosha.jpg")
    local mat=standardMaterial name:"pbake" diffuse:(color 255 255 255) selfIllumAmount:100 diffuseMap:(bitmapTexture filename:(USFolder+"p_bake.jpg"))
    USBakeFloor.material=mat
    showTextureMap mat mat.diffuseMap on
    select USBakeFloor
    exportFile (USScratch+"piso_baked.3ds") #noPrompt selectedOnly:true
    saveMaxFile (USFolder+"cenario.max") quiet:true
    USNote "BAKE complete native outputs and piso_baked.3ds; original floor export is preserved until image/runtime verification"
)
try (USNativeBake();messageBox "Native floor bake complete. Verify maps before runtime promotion." title:"O Ultimo Sinal")
catch (USNote(getCurrentExceptionStackTrace());messageBox(getCurrentException()) title:"Bake stopped safely")
