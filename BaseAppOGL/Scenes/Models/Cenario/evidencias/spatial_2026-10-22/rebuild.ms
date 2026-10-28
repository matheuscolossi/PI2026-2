-- Native production for the spatial revision. Originals remain in the backup
-- and hidden OLD_ groups; only new selected geometry is exported to staging.
global USFolder,USScratch,USMats,USCounter, USG,USBox,USCylinder,USBeam,USUnwrap
global USName,USRot,USMakeMaterials,USArch,USPropBuild,USPropMaterials
global SRLog,SRAdded=#(),SRMaterial
fn SRNote s=(format "%\n" s to:SRLog;flush SRLog)
fn SRBox a p s m r:.01 =
(
 local n=chamferBox name:(USName "SR") width:s.x length:s.z height:s.y fillet:(amin #(r,s.x*.18,s.y*.18,s.z*.18)) Fillet_Segments:3 mapCoords:true pos:(USG (p-[0,s.y/2,0]))
 n.pivot=USG p;n.material=m;append a n;n
)
fn SRText a word p scale m yaw:0 =
(
 local n=text name:(USName "Tx") text:word size:scale font:"Arial" alignment:2 pos:(USG p)
 n.material=m;rotate n (angleaxis 90 [1,0,0]);if yaw!=0 do rotate n (angleaxis yaw [0,0,1])
 addModifier n (Extrude amount:.0015);append a n;n
)
fn SRTube a bottom top outer inner m =
(
 local v=#(),f=#(),count=64
 for pair in #([outer,bottom],[outer,top],[inner,bottom],[inner,top]) do for i=0 to count-1 do append v (USG [pair.x*cos(i*360.0/count),pair.y,-3+pair.x*sin(i*360.0/count)])
 for i=1 to count do
 (
  local j=if i==count then 1 else i+1
  for q in #([i,j,count+j,count+i],[2*count+j,2*count+i,3*count+i,3*count+j],[count+i,count+j,3*count+j,3*count+i],[j,i,2*count+i,2*count+j]) do
  (append f [q.x,q.y,q.z];append f [q.x,q.z,q.w])
 )
 local n=mesh name:(USName "Tu") vertices:v faces:f;n.material=m;append a n;n
)
fn SRWedge a angle height =
(
 local v=#(),f=#();local count=5
 for y in #(height-.055,height) do for r in #(.43,1.37) do for i=0 to count do
  append v (USG [r*cos(angle+i*4),y,-3+r*sin(angle+i*4)])
 local band=count+1
 for i=1 to count do
 (
  local q=#([i,i+1,band+i+1,band+i],[2*band+i,3*band+i,3*band+i+1,2*band+i+1],[i,2*band+i,2*band+i+1,i+1],[band+i+1,3*band+i+1,3*band+i,band+i])
  for x in q do (append f [x.x,x.y,x.z];append f [x.x,x.z,x.w])
 )
 for q in #([1,band+1,3*band+1,2*band+1],[band,3*band,4*band,2*band]) do (append f [q.x,q.y,q.z];append f [q.x,q.z,q.w])
 local n=mesh name:(USName "St") vertices:v faces:f;n.material=USMats.Metal;append a n;n
)
fn SRRoof =
(
 local v=#(),f=#();local count=96
 for r in #(0,1) do for i=0 to count-1 do
 (
  local t=i*360.0/count;local c=cos t;local s=sin t
  local extent=amin #(5.36/(amax .00001 (abs c)),(if s>=0 then 8.4 else 3.35)/(amax .00001 (abs s)))
  local rad=if r==0 then 1.72 else extent;local x=rad*c;local z= -3+rad*s
  append v (USG [x,4.63-.273*(abs x),z])
 )
 for i=1 to count do(local j=if i==count then 1 else i+1;append f [i,j,count+j];append f [i,count+j,count+i])
 local n=mesh name:(USName "Rf") vertices:v faces:f;n.material=USMats.Rust;n
)
fn SRExport key a anchor:[0,0,0] =
(
 SRNote ("UV "+key+" nodes="+(a.count as string))
 for i=1 to a.count do(USUnwrap a[i];if mod i 20==0 do SRNote ("  "+(i as string)))
 local transforms=for n in a collect n.transform
 for n in a do n.transform=n.transform*(transMatrix (-(USG anchor)))
 select a;exportFile (USScratch+key+".3ds") #noPrompt selectedOnly:true
 for i=1 to a.count do a[i].transform=transforms[i]
 if not doesFileExist (USScratch+key+".3ds") do throw ("Missing export "+key)
 local old=getNodeByName ("US_"+key)
 if old!=undefined do (old.name="OLD_"+old.name;hide old;for n in old.children do(n.renderable=false;n.castShadows=false))
 group a name:("US_"+key);append SRAdded key
 SRNote ("EXPORTED "+key)
 saveMaxFile (USScratch+"cenario_revisado.max") quiet:true
)
fn SRBuild =
(
 USFolder=@"C:\Users\mlc17\Desktop\O Último Sinal\BaseAppOGL\Scenes\Models\Cenario\"
 USScratch=@"C:\Users\mlc17\Documents\Codex\2026-09-08\https-drive-google-com-drive-u\work\spatial_2026-10-22\native\"
 SRLog=createFile (USScratch+"producao.txt")
 loadMaxFile (USFolder+"cenario.max") quiet:true useFileUnits:true
 fileIn (USFolder+"common.ms");USMakeMaterials();fileIn (USFolder+"PropsHelpers.ms")
 fileIn @"C:\Users\mlc17\Documents\Codex\2026-09-08\https-drive-google-com-drive-u\work\spatial_2026-10-22\arch_functions.ms"
 local leather=standardMaterial name:"leather" diffuse:(color 57 35 33) glossiness:25 specularLevel:18
 local enamel=standardMaterial name:"enamel" diffuse:(color 32 68 70) glossiness:35 specularLevel:28
 local cream=standardMaterial name:"cream" diffuse:(color 207 193 150) glossiness:12 specularLevel:8
 local a=#()
 -- Full two-room shell with an actual roof/ceiling aperture for the tower.
 a=USArch "anexo";SRExport "anexo" a
 a=#();SRTube a 3.15 7.30 1.73 1.50 USMats.Chalk
 SRTube a 5.65 6.15 1.75 1.50 USMats.Rust
 SRTube a 7.20 7.295 2.65 1.47 USMats.Metal
 -- A landing joins the last stair to the annular floor without capping the shaft.
 SRBox a [1.89,7.245,-3] [1.25,.10,.9] USMats.Metal
 SRTube a 7.30 7.48 2.70 2.46 USMats.Metal
 for i=0 to 15 do
 (
  local t=i*22.5;local u=(i+1)*22.5
  local p=[2.52*cos t,7.33,-3+2.52*sin t],q=[2.52*cos u,7.33,-3+2.52*sin u]
  append a (USBeam p (p+[0,2.25,0]) .026 USMats.Brass)
  append a (USBeam (p+[0,2.25,0]) (q+[0,2.25,0]) .028 USMats.Brass)
 )
 append a (USCylinder [0,9.65,-3] [2.82,.18,2.82] USMats.Metal sides:64)
 local roof=cone radius1:2.95 radius2:.10 height:.95 sides:64 mapCoords:true pos:(USG [0,9.74,-3]);roof.material=USMats.Rust;append a roof
 -- Optical assembly: rings, struts, base and focusing cradle.
 append a (USCylinder [0,7.63,-3] [.61,.67,.61] USMats.Metal sides:48)
 for j=0 to 9 do
 (
  local ring=torus radius1:(.58+.12*sin(j*18)) radius2:.035 segs:64 sides:8 pos:(USG [0,7.91+j*.10,-3]);ring.material=USMats.Brass;append a ring
 )
 for t in #(45,135,225,315) do append a (USBeam [.69*cos t,7.8,-3+.69*sin t] [.69*cos t,9.05,-3+.69*sin t] .025 USMats.Brass)
 SRExport "torre" a
 a=#()
 for j=0 to 35 do
 (
  local angle=j*20.;local top=.095+(j+1)*.20;SRWedge a angle top
  local p=[1.40*cos(angle+10),top+.02,-3+1.40*sin(angle+10)]
  if mod j 2==0 do append a (USBeam p (p+[0,.92,0]) .018 USMats.Brass)
  local q=[1.40*cos(angle+30),top+.22,-3+1.40*sin(angle+30)]
  if j<35 do append a (USBeam (p+[0,.92,0]) (q+[0,.92,0]) .022 USMats.Brass)
 )
 append a (USCylinder [0,3.7,-3] [.10,7.21,.10] USMats.Metal sides:32)
 SRExport "escada" a
 -- Legacy photo/window glass stays in a separate pass, including the optical room.
 a=#()
 for i=0 to 15 do
 (
  local t=i*22.5+11.25;local p=[2.53*cos t,8.48,-3+2.53*sin t]
  local n=SRBox a p [.025,2.12,.975] USMats.Glass r:.002
  USRot n (-t) [0,1,0]
 )
 SRExport "vidfar" a
 -- Writing desk, not a stretched dining table. Local origin is at floor centre.
 a=#();SRBox a [0,.94,0] [1.60,.11,1.95] USMats.Wood r:.025
 for x in #(-.68,.68) do for z in #(-.82,.82) do
 (
  append a (USCylinder [x,.47,z] [.052,.94,.052] USMats.Wood sides:24)
  append a (USCylinder [x,.06,z] [.063,.10,.063] USMats.Brass sides:24)
 )
 for z in #(-.56,0,.56) do
 (
  SRBox a [.62,.76,z] [.28,.25,.51] USMats.Wood
  SRBox a [.775,.76,z] [.025,.21,.45] enamel
  append a (USBeam [.80,.78,z-.08] [.80,.78,z+.08] .012 USMats.Brass)
 )
 SRBox a [-.02,.83,-.85] [1.32,.14,.10] enamel
 SRExport "escrivan" a anchor:[0,0,0]
 -- Radio: stronger material separation and a recessed real grille.
 a=#();SRBox a [0,0,0] [.80,.39,.36] enamel r:.035
 SRBox a [0,.0,.181] [.72,.31,.023] USMats.Brass r:.018
 SRBox a [-.14,-.01,.199] [.40,.23,.016] USMats.Black r:.012
 for i=0 to 17 do append a (USBeam [-.33+i*.023,-.10,.213] [-.33+i*.023,.09,.213] .003 USMats.Metal)
 SRBox a [.20,.09,.205] [.19,.062,.013] cream r:.004
 for x in #(.13,.27) do
 (
  append a (USBeam [x,-.065,.20] [x,-.065,.255] .039 USMats.Black)
  append a (USBeam [x,-.065,.255] [x,-.065,.262] .020 USMats.Brass)
 )
 for x in #(-.29,.29) do SRBox a [x,-.222,0] [.095,.055,.20] USMats.Black
 append a (USBeam [-.32,.20,-.1] [-.48,.92,-.1] .007 USMats.Brass)
 SRExport "radio" a
 -- Diary: leather covers, page block and proper spine instead of a wood bitmap.
 a=#();SRBox a [0,0,0] [.49,.065,.35] USMats.Paper r:.004
 for y in #(-.047,.047) do SRBox a [0,y,0] [.54,.022,.39] leather r:.009
 append a (USCylinder [-.255,0,0] [.048,.39,.048] leather sides:32)
 -- Cylinder is rotated to lie along the binding.
 USRot a[a.count] 90 [1,0,0]
 for j=0 to 8 do SRBox a [.249,-.027+j*.007,0] [.001,.001,.32] cream r:0
 SRBox a [0,.060,-.015] [.30,.002,.17] USMats.Brass r:0
 SRBox a [0,.062,-.015] [.28,.002,.15] leather r:0
 SRExport "diario" a
 -- Tool handles are dark rubber, not pale timber.
 local palette=USPropMaterials Wood:USMats.Shoe Metal:USMats.Metal Paint:enamel Paper:USMats.Paper Brass:USMats.Brass Glass:USMats.Glass Shoe:USMats.Shoe Blue:USMats.Blue
 a=USPropBuild "alicate" palette;for n in a where classOf n.baseObject==ChamferBox do n.baseObject.Fillet_Segments=3
 SRExport "alicate" a
 -- More defined cabinet casing; all dimensions and hinge pivots remain stable.
 a=#()
 SRBox a [0,1.05,.56] [1.25,2.1,.06] enamel
 for x in #(-.60,.60) do SRBox a [x,1.05,0] [.055,2.1,1.15] enamel
 for y in #(.06,1.20,2.07) do SRBox a [0,y,0] [1.25,.06,1.15] USMats.Wood
 for x in #(-.49,.49) do SRBox a [x,.16,.36] [.075,.27,.12] USMats.Brass
 SRExport "armario" a
 a=#();SRBox a [.62,0,0] [1.24,2.1,.065] enamel r:.018
 for y in #(-.62,.52) do SRBox a [.62,y,-.040] [1.08,.77,.015] USMats.Paint
 append a (USBeam [1.06,-.10,-.085] [1.06,.12,-.085] .018 USMats.Brass)
 SRBox a [.62,.74,-.053] [.55,.15,.014] cream
 SRExport "armporta" a
 -- Decorative objects are built in world space with surfaces and clearances.
 a=#()
 for j=0 to 5 do
 (
  local mat=if mod j 2==0 then leather else enamel
  SRBox a [-3.48+j*.115,1.155,.02] [.095,.11+j*.018,.33] mat r:.004
 )
 -- Desk light with turned stem and shade, motivated by a warm point light.
 append a (USCylinder [3.48,1.12,3.02] [.13,.045,.13] USMats.Brass sides:32)
 append a (USBeam [3.48,1.14,3.02] [3.48,1.65,3.02] .016 USMats.Brass)
 local shade=cone radius1:.20 radius2:.10 height:.21 sides:48 pos:(USG [3.48,1.60,3.02]);shade.material=cream;append a shade
 SRText a "BRUMAS / SERVICO" [-1.5,2.45,4.78] .09 cream
 SRText a "ARQUIVO DO FAROL" [2.7,2.55,-5.81] .095 USMats.Brass
 -- Optical controls on a cast pedestal, away from the stair landing.
 append a (USCylinder [1.92,7.76,-3.05] [.18,.93,.18] USMats.Metal sides:32)
 SRBox a [1.92,8.24,-3.05] [.64,.12,.57] enamel
 SRText a "COMANDO LOCAL" [1.92,8.31,-2.80] .052 cream
 SRExport "decor" a
 -- Align the source master with the new diary/maintenance furniture layout.
 for job in #(#("US_diario",[-2.9,1.151,1.8]),#("US_alicate",[-3,1.1235,.25]),#("US_armario",[-3.7,0,4.15]),#("US_armporta",[-4.32,1.05,3.55]),#("US_radio",[3,1.29,2]),#("US_MesaDiario",[.1,0,-1.2]),#("US_MesaManut",[0,0,-.75])) do
 (
  local n=getNodeByName job[1]
  if n!=undefined do move n (USG job[2])
 )
 -- vidfar/torre/escada are authored directly in world space, no extra anchor.
 saveMaxFile (USScratch+"cenario_revisado.max") quiet:true
 local done=createFile (USScratch+"complete.txt");format "%\n" SRAdded to:done;close done
 SRNote "COMPLETE";close SRLog;format "SPATIAL BUILD COMPLETE\n"
)
try(SRBuild())catch(try(SRNote(getCurrentExceptionStackTrace());close SRLog)catch();messageBox(getCurrentException()) title:"Spatial build paused safely")
