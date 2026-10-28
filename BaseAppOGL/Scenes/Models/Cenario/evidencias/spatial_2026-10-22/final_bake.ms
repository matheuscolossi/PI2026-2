global USFolder,USScratch,USMats,USG,USBakeFloor,USBakeShadow,USBakeClear
fn SRFinalBake =
(
 if not doesFileExist (USScratch+"post_complete.txt") do throw "Complete finishing first."
 local log=openFile (USScratch+"producao.txt") mode:"a"
 local chair=getNodeByName "US_cadeira"
 if getUserProp chair "placed20261022"!="yes" do
 (
  move chair (USG [4.3,.095,2]);setUserProp chair "placed20261022" "yes"
  local second=for n in chair.children collect (instance n)
  for n in second do move n (USG [0,0,1.2])
  group second name:"SR_ChairLetter"
 )
 local light=getNodeByName "US_BakeLight2";light.pos=USG [2.4,2.70,-1.8]
 -- New fine-grain timber is generated art. Original lesson texture is preserved
 -- in the pre-revision backup and in the original P1.jpg resource.
 local wood=standardMaterial name:"finewood" diffuse:(color 255 255 255) diffuseMap:(bitmapTexture filename:(USScratch+"woodfine.jpg")) specularLevel:8 glossiness:20
 wood.diffuseMap.coordinates.uTiling=4;wood.diffuseMap.coordinates.vTiling=4
 USBakeFloor=(getNodeByName "US_piso").children[1];USBakeFloor.material=wood
 USBakeFloor.INodeBakeProperties.removeAllBakeElements();USBakeShadow=CompleteMap();USBakeClear=CompleteMap()
 for pair in #(#(USBakeShadow,"sp_on.jpg",true),#(USBakeClear,"sp_off.jpg",false)) do
 (
  local el=pair[1];el.outputSzX=1024;el.outputSzY=1024;el.fileType=USScratch+pair[2];el.fileName=pair[2]
  el.filterOn=true;el.shadowsOn=pair[3];el.enabled=true;USBakeFloor.INodeBakeProperties.addBakeElement el
 )
 USBakeFloor.INodeBakeProperties.bakeEnabled=true;USBakeFloor.INodeBakeProperties.bakeChannel=1;USBakeFloor.INodeBakeProperties.nDilations=3
 select USBakeFloor;format "FINAL WOOD BAKE begin\n" to:log;flush log
 render rendertype:#bakeSelected vfb:off progressBar:true outputSize:[1024,1024]
 USBakeFloor.material=standardMaterial name:"pbake" diffuse:(color 255 255 255) selfIllumAmount:100 diffuseMap:(bitmapTexture filename:(USFolder+"p_bake.jpg"))
 select USBakeFloor;exportFile (USScratch+"piso.3ds") #noPrompt selectedOnly:true
 -- Store portable production paths in the editable master.
 for tex in getClassInstances BitmapTexture do
 (
  local name=toLower (filenameFromPath tex.filename)
  if name=="wood.jpg" or name=="woodfine.jpg" do tex.filename=USFolder+"wood.jpg"
  if name=="photo.jpg" do tex.filename=USFolder+"photo.jpg"
 )
 saveMaxFile (USScratch+"cenario_revisado.max") quiet:true
 format "FINAL WOOD BAKE complete\n" to:log;close log
 local done=createFile (USScratch+"final_complete.txt");format "Fine timber, corrected lamp and matched floor bake saved.\n" to:done;close done
)
try(with redraw off(SRFinalBake()))catch(messageBox(getCurrentException()) title:"Final bake paused safely")
