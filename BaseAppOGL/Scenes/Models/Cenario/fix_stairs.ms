-- Execute in the current native Master. A saved pre-fix source is mandatory.
global USFolder, USScratch, USG, USNote, USMakeMaterials, USArch, USUnwrap
fn USRepairStair =
(
    if maxFileName!="cenario.max" do throw "Expected the current saved cenario.max."
    local oldRoot=getNodeByName "US_escada"
    if oldRoot==undefined or oldRoot.children.count!=10 do throw "Expected the ten native staircase components."
    if getUserProp oldRoot "repair20261020"=="complete" do throw "Staircase repair already applied."
    saveMaxFile (USFolder+"evidencias\\cenario_antes_escada.max") quiet:true
    fileIn (USFolder+"common.ms")
    USMakeMaterials()
    -- Edit the original ten components in place, preserving native primitives.
    local nodes=for n in oldRoot.children collect n
    for n in nodes do
    (
        local warning=classOf n.baseObject==Box and n.baseObject.width<.1
        if classOf n.baseObject==Box and n.baseObject.width>.8 do
        (
            local j=(floor ((n.baseObject.height-.23)/.40+.5)) as integer
            if j<0 or j>3 do throw "Unexpected original stair height."
            n.baseObject.height=.20+j*.20
        )
        n.transform=n.transform*(transMatrix (-(USG [-4.35,0,.2])))*(scaleMatrix [.8,1,1])*(rotateZMatrix 180)
        if warning do n.transform=n.transform*(transMatrix [.768,0,0])
    )
    for n in nodes do USUnwrap n
    select nodes
    exportFile (USScratch+"escada_corrigida.3ds") #noPrompt selectedOnly:true
    if not doesFileExist (USScratch+"escada_corrigida.3ds") do throw "No repaired export."
    for n in nodes do n.transform=n.transform*(transMatrix (USG [-4.26,.09,.65]))
    local newRoot=oldRoot
    newRoot.pivot=USG [-4.26,.09,.65]
    setUserProp newRoot "repair20261020" "complete"
    local report=createFile (USFolder+"evidencias\\escada_reparo.txt")
    format "Native staircase repair: 10 components, yaw 180, width factor .8, anchor (-4.26,.09,.65), risers .20m.\n" to:report
    for n in newRoot.children do
    (
        local b=nodeGetBoundingBox n (matrix3 1)
        format "% bounds_max_xyz % %\n" n.name b[1] b[2] to:report
    )
    local tops=for n in nodes where classOf n.baseObject==Box and n.baseObject.width>.8 collect (nodeGetBoundingBox n (matrix3 1))[2].z
    sort tops
    if tops.count!=4 do throw "Expected four native stone steps."
    for i=2 to 4 do if abs(tops[i]-tops[i-1]-.20)>.002 do throw "Native riser audit failed."
    format "Evaluated tread tops gameY: %; three rises .20m verified.\n" tops to:report
    close report
    saveMaxFile (USFolder+"cenario.max") quiet:true
    USNote "STAIR REPAIR complete: local export pending validation, Master updated; floor rebake required."
)
try (USRepairStair();format "STAIR REPAIR OK\n")
catch (try(USNote(getCurrentExceptionStackTrace()))catch();messageBox(getCurrentException()) title:"Stair repair stopped safely")
