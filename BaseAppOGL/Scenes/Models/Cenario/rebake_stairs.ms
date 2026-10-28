-- Re-bake the existing UV1 atlas after moving a static shadow caster.
global USBakeFloor, USBakeShadow, USBakeClear
fn USRebakeStair =
(
    local stair=getNodeByName "US_escada"
    if stair==undefined or getUserProp stair "repair20261020"!="complete" do throw "Repair the native staircase first."
    local floorRoot=getNodeByName "US_piso"
    if floorRoot==undefined do throw "Native floor absent."
    if doesFileExist (USScratch+"stair_on.jpg") do throw "Existing new bake protected."
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
    for name in #("US_BakeLight1","US_BakeLight2") do
    (
        local lamp=getNodeByName name
        if lamp==undefined do throw "Expected the existing native bake lights."
        lamp.baseObject.castShadows=true
    )
    USBakeFloor.INodeBakeProperties.removeAllBakeElements()
    USBakeShadow=CompleteMap();USBakeClear=CompleteMap()
    for pair in #(#(USBakeShadow,"stair_on.jpg",true),#(USBakeClear,"stair_off.jpg",false)) do
    (
        local element=pair[1]
        element.outputSzX=1024;element.outputSzY=1024
        element.fileType=USScratch+pair[2];element.fileName=pair[2]
        element.filterOn=true;element.shadowsOn=pair[3];element.enabled=true
        USBakeFloor.INodeBakeProperties.addBakeElement element
    )
    USBakeFloor.INodeBakeProperties.bakeEnabled=true
    USBakeFloor.INodeBakeProperties.bakeChannel=1
    USBakeFloor.INodeBakeProperties.nDilations=3
    select USBakeFloor
    USNote "STAIR BAKE begin: existing floor UV1, paired native CompleteMap."
    render rendertype:#bakeSelected vfb:off progressBar:true outputSize:[1024,1024]
    for name in #("stair_on.jpg","stair_off.jpg") do if not doesFileExist (USScratch+name) do throw ("Output absent "+name)
    -- Promotion happens outside Max, after image verification. Reference the final
    -- stable short filename so the 3DS material will resolve after promotion.
    local mat=standardMaterial name:"pbake" diffuse:(color 255 255 255) selfIllumAmount:100 diffuseMap:(bitmapTexture filename:(USFolder+"p_bake.jpg"))
    USBakeFloor.material=mat
    showTextureMap mat mat.diffuseMap on
    select USBakeFloor
    exportFile (USScratch+"piso_stair.3ds") #noPrompt selectedOnly:true
    saveMaxFile (USFolder+"cenario.max") quiet:true
    USNote "STAIR BAKE complete: paired images and piso_stair.3ds pending promotion."
)
try(USRebakeStair();format "STAIR BAKE OK\n")
catch(try(USNote(getCurrentExceptionStackTrace()))catch();messageBox(getCurrentException()) title:"Stair bake stopped safely")
