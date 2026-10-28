-- Native 3ds Max polish, retaining the professor's .3DS/Standard/UVW pipeline.
global USFolder,USScratch,USG,USNote,USUnwrap,USMakeMaterials
global USPolishLog
fn USPolishNode n =
(
    local primitive=n.baseObject
    local changed=false
    if classOf primitive==ChamferBox then
    (
        local small=amin #(primitive.width,primitive.length,primitive.height)
        if small>.008 do
        (
            primitive.fillet=amin #(.022,small*.22)
            primitive.Fillet_Segments=3
            changed=true
        )
    )
    else if classOf primitive==Box then
    (
        local small=amin #(primitive.width,primitive.length,primitive.height)
        if small>.018 do
        (
            local c=Chamfer amount:(amin #(.014,small*.16)) segments:3 tension:.55
            c.chamfertype=0;c.miteringType=0;c.amountType=2
            c.selectionoption=5;c.useminangle=true;c.minangle=35
            c.smooth=true;c.smoothtype=1;c.smoothtoadjacent=true;c.limiteffect=true
            addModifier n c
            changed=true
        )
    )
    else if classOf primitive==Cylinder then
    (
        if primitive.radius>.02 and primitive.sides<32 do
        (
            primitive.sides=32
            changed=true
        )
    )
    if changed do
    (
        USUnwrap n
        local msh=snapshotAsMesh n
        format "% | % | faces=% | uv=%\n" n.name (classOf n.baseObject) msh.numfaces msh.numtverts to:USPolishLog
    )
    changed
)
fn USPolishExport key anchor =
(
    local root=getNodeByName ("US_"+key)
    if root==undefined do throw ("Missing group "+key)
    local nodes=for n in root.children collect n
    local changed=0
    USNote ("POLISH begin "+key)
    for n in nodes do if USPolishNode n do changed+=1
    if changed>0 do
    (
        local transforms=for n in nodes collect n.transform
        for n in nodes do n.transform=n.transform*(transMatrix (-(USG anchor)))
        select nodes
        exportFile (USScratch+key+".3ds") #noPrompt selectedOnly:true
        for i=1 to nodes.count do nodes[i].transform=transforms[i]
        if not doesFileExist (USScratch+key+".3ds") do throw ("Missing native export "+key)
        format "EXPORT % changed=% components=%\n" key changed nodes.count to:USPolishLog
    )
    saveMaxFile (USFolder+"cenario.max") quiet:true
    USNote ("POLISH complete "+key+" changed="+(changed as string))
)
fn USNativePolish =
(
    USFolder=@"C:\Users\mlc17\Desktop\O Último Sinal\BaseAppOGL\Scenes\Models\Cenario\"
    USScratch=@"C:\Users\mlc17\Documents\Codex\2026-09-08\https-drive-google-com-drive-u\work\pro_upgrade_2026-10-20\native\"
    makeDir USScratch all:true
    if objects.count>0 and maxFileName!="cenario.max" do throw "Open only the saved Master or an empty startup scene."
    if objects.count>0 and getSaveRequired() do saveMaxFile (USFolder+"evidencias\\antes_polimento_ui.max") quiet:true
    if doesFileExist (USScratch+"polish_complete.txt") do throw "Production already complete; original sources were preserved."
    loadMaxFile (USFolder+"cenario.max") quiet:true useFileUnits:true
    fileIn (USFolder+"common.ms")
    USPolishLog=createFile (USScratch+"polish_native.txt")
    local jobs=#(
      #("anexo",[0,0,0]),#("torre",[0,0,0]),#("cais",[0,0,0]),#("caisbox",[0,0,0]),
      #("escada",[-4.26,.09,.65]),#("porta",[-.97,1.35,-.5]),#("cadeira",[4.3,.095,2]),
      #("caneca",[3.52,1.186,3.08]),#("armario",[-3.7,0,4.15]),#("armporta",[-4.32,1.05,3.55]),
      #("botas",[-1.685,.095,4.3]),#("radio",[3,1.29,2]),#("quadro",[4.67,1.55,3.6]),
      #("arquivo",[4.6,.095,-4.65]),#("lumin",[0,3.14,2.5]),#("diario",[-3,1.151,3]),
      #("alicate",[-3,1.1235,1]),#("chave",[-3.7,1.25,3.8]),#("relogio",[-2.3,2.27,-5.73])
    )
    for job in jobs do USPolishExport job[1] job[2]
    for name in #("US_MesaDiario","US_MesaManut","US_MesaReg","US_MesaFoto","US_MesaRadio") do
    (
        local root=getNodeByName name
        if root==undefined do throw ("Missing Master table "+name)
        USNote ("POLISH table "+name)
        for n in root.children do USPolishNode n
    )
    saveMaxFile (USFolder+"cenario.max") quiet:true
    -- The source table remains five editable Boxes with native Chamfer and UVW.
    local tableFolder=@"C:\Users\mlc17\Desktop\O Último Sinal\BaseAppOGL\Scenes\Models\Mesa\"
    loadMaxFile (tableFolder+"mesa.max") quiet:true useFileUnits:true
    if objects.count!=5 do throw "Expected the original five-piece table source."
    for n in objects do USPolishNode n
    select objects
    exportFile (USScratch+"mesa.3ds") #noPrompt selectedOnly:true
    saveMaxFile (tableFolder+"mesa.max") quiet:true
    close USPolishLog
    loadMaxFile (USFolder+"cenario.max") quiet:true useFileUnits:true
    local done=createFile (USScratch+"polish_complete.txt")
    format "Native Chamfer/ChamferBox3/cylinder32 and Unwrap applied; table retains 5 editable Boxes. Master saved and reloaded.\n" to:done
    close done
    USNote "POLISH all native exports ready for acceptance checks; originals retained in pre-polish ZIP."
    format "NATIVE POLISH OK\n"
)
try(USNativePolish())catch(try(close USPolishLog)catch();try(USNote(getCurrentExceptionStackTrace()))catch();messageBox(getCurrentException()) title:"Polish stopped safely")
