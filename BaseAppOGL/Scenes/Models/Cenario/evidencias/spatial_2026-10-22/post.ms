-- Final native pass: chair, evidence artwork, clear optical landing and new bake.
global USFolder,USScratch,USMats,USG,USUnwrap,USMakeMaterials,SRExport,SRBox,SRText,USBeam,USCylinder,USName
global USBakeFloor,USBakeShadow,USBakeClear,SRLog
fn SRPost =
(
 if not doesFileExist (USScratch+"complete.txt") do throw "Wait for the complete native build."
 SRLog=openFile (USScratch+"producao.txt") mode:"a"
 -- Keep all subsequent authoring off the display redraw path.
 local decor=getNodeByName "US_decor"
 local moved=0
 for n in decor.children where n.min.z>7 and n.max.x>1.0 do (move n (USG [-1.92,0,-1.85]);moved+=1)
 if moved>0 do
 (
  select decor.children;exportFile (USScratch+"decor.3ds") #noPrompt selectedOnly:true
  SRNote ("OPTICAL CONSOLE moved clear of landing: "+(moved as string)+" nodes")
 )
 local a=#(),leather=standardMaterial name:"seat" diffuse:(color 68 42 34) glossiness:22 specularLevel:12
 local enamel=standardMaterial name:"teal" diffuse:(color 31 65 68) glossiness:28 specularLevel:20
 SRBox a [0,.475,0] [.55,.10,.55] leather r:.03
 for x in #(-.215,.215) do for z in #(-.215,.215) do
 (
  append a (USBeam [x*1.12,.015,z*1.12] [x,.45,z] .022 USMats.Metal)
  append a (USBeam [x*1.12,.015,z*1.12] [x*1.10,.055,z*1.10] .026 USMats.Brass)
 )
 -- Curved back, individual rounded slats and rolled arms.
 for j= -4 to 4 do
 (
  local z=j*.060;local x= -.235-.045*cos(j*16)
  SRBox a [x,.81,z] [.058,.39,.070] enamel r:.013
 )
 for z in #(-.27,.27) do
 (
  append a (USBeam [-.21,.49,z] [-.21,.69,z] .017 USMats.Brass)
  append a (USBeam [-.21,.69,z] [.21,.65,z] .022 USMats.Brass)
  append a (USBeam [.21,.65,z] [.21,.48,z] .017 USMats.Brass)
 )
 SRExport "cadeira" a
 -- A complete photo is mapped with explicit UVs on its top face; the border
 -- and back remain separate paper geometry. The handwritten solution stays text.
 if doesFileExist (USScratch+"photo.jpg") then
 (
  a=#();local art=standardMaterial name:"aurora" diffuse:(color 255 255 255) diffuseMap:(bitmapTexture filename:(USScratch+"photo.jpg")) specularLevel:0
  local paper=USMats.Paper
  SRBox a [0,.004,0] [.32,.008,.47] paper r:.002
  local n=mesh name:(USName "Ph") vertices:#(USG [-.15,.009,.225],USG [.15,.009,.225],USG [.15,.009,-.225],USG [-.15,.009,-.225]) faces:#([1,2,3],[1,3,4])
  n.material=art
  setNumTVerts n 4;setTVert n 1 [0,0,0];setTVert n 2 [1,0,0];setTVert n 3 [1,1,0];setTVert n 4 [0,1,0]
  buildTVFaces n;setTVFace n 1 [1,2,3];setTVFace n 2 [1,3,4]
  local uv=Unwrap_UVW();addModifier n uv;append a n
  -- Export without re-flattening the authored photographic plane's full UV.
  USUnwrap a[1];select a;exportFile (USScratch+"foto.3ds") #noPrompt selectedOnly:true
  local old=getNodeByName "US_foto";if old!=undefined do(old.name="OLD_US_foto";hide old;for p in old.children do(p.renderable=false;p.castShadows=false))
  local root=group a name:"US_foto";move root (USG [3,1.096,-3]);SRNote "PHOTOGRAPH: explicit full image UV, paper back, native source saved."
 )
 -- Update the source master used by the lighting bake, including the actual
 -- writing desk instead of a duplicate dining-table shadow caster.
 local oldDesk=getNodeByName "US_MesaRadio"
 if oldDesk!=undefined do(hide oldDesk;for n in oldDesk.children do(n.renderable=false;n.castShadows=false))
 local desk=getNodeByName "US_escrivan";move desk (USG [3,.095,2.5])
 local floorRoot=getNodeByName "US_piso";USBakeFloor=floorRoot.children[1]
 local wood=copy USMats.Wood;wood.name="floorpre";wood.diffuseMap=copy USMats.Wood.diffuseMap
 wood.diffuseMap.coordinates.uTiling=8;wood.diffuseMap.coordinates.vTiling=8;USBakeFloor.material=wood
 for name in #("US_porta","US_armporta","US_chave","US_agua","US_janvid","US_vidfar","US_led","US_lampada") do
 (
  local root=getNodeByName name;if root!=undefined do for n in root.children do n.castShadows=false
 )
 renderers.current=Default_Scanline_Renderer();ambientColor=color 45 49 55
 for name in #("US_BakeLight1","US_BakeLight2") do
 (
  local lamp=getNodeByName name;lamp.baseObject.castShadows=true;lamp.multiplier=.43
 )
 local lamp=omniLight name:"SR_DeskLight" pos:(USG [3.48,1.69,3.02]) rgb:(color 255 190 110) multiplier:.23
 lamp.castShadows=true
 USBakeFloor.INodeBakeProperties.removeAllBakeElements();USBakeShadow=CompleteMap();USBakeClear=CompleteMap()
 for pair in #(#(USBakeShadow,"sp_on.jpg",true),#(USBakeClear,"sp_off.jpg",false)) do
 (
  local el=pair[1];el.outputSzX=1024;el.outputSzY=1024;el.fileType=USScratch+pair[2];el.fileName=pair[2]
  el.filterOn=true;el.shadowsOn=pair[3];el.enabled=true;USBakeFloor.INodeBakeProperties.addBakeElement el
 )
 USBakeFloor.INodeBakeProperties.bakeEnabled=true;USBakeFloor.INodeBakeProperties.bakeChannel=1;USBakeFloor.INodeBakeProperties.nDilations=3
 select USBakeFloor;SRNote "BAKE begin for corrected furniture and stair positions"
 render rendertype:#bakeSelected vfb:off progressBar:true outputSize:[1024,1024]
 for name in #("sp_on.jpg","sp_off.jpg") do if not doesFileExist (USScratch+name) do throw ("Missing bake "+name)
 local mat=standardMaterial name:"pbake" diffuse:(color 255 255 255) selfIllumAmount:100 diffuseMap:(bitmapTexture filename:(USFolder+"p_bake.jpg"))
 USBakeFloor.material=mat;showTextureMap mat mat.diffuseMap on;select USBakeFloor;exportFile (USScratch+"piso.3ds") #noPrompt selectedOnly:true
 saveMaxFile (USScratch+"cenario_revisado.max") quiet:true
 local done=createFile (USScratch+"post_complete.txt");format "Native chair, photo UV, optical clearance and floor CompleteMap updated.\n" to:done;close done
 SRNote "POST COMPLETE";close SRLog
)
try(with redraw off(SRPost()))catch(try(SRNote(getCurrentExceptionStackTrace());close SRLog)catch();messageBox(getCurrentException()) title:"Spatial finishing paused safely")
