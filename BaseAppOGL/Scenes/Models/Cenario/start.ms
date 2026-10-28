global USFolder, USScratch, USNote, USG, USMakeMaterials
(
    try
    (
        global USFolder=getFilenamePath (getSourceFileName())
        global USScratch=@"C:\Users\mlc17\Documents\Codex\2026-09-08\https-drive-google-com-drive-u\work\cenario_2026-10-20\bake_temp\"
        makeDir USScratch all:true
        makeDir (USFolder+"evidencias") all:true
        fileIn (USFolder+"common.ms")
        if doesFileExist (USFolder+"cenario.max") do throw "cenario.max already exists; resume using a separate guarded stage."
        if maxFileName!="mesa.max" or objects.count!=5 do throw "Expected the saved five-piece Mesa scene. Existing work was not reset."
        if getSaveRequired() do saveMaxFile (USFolder+"mesa_snapshot.max") quiet:true
        local names=#("Tampo","Perna1","Perna2","Perna3","Perna4")
        local nodes=for name in names collect getNodeByName name
        if findItem nodes undefined>0 do throw "The original table pieces are missing."
        units.DisplayType=#Metric;units.MetricType=#Meters;units.SystemType=#Meters;units.SystemScale=1.0
        for n in nodes do move n (USG [-3,.095,3])
        group nodes name:"US_MesaDiario"
        saveMaxFile (USFolder+"cenario.max") quiet:true
        USNote "Started separate cenario.max from saved Mesa; source mesa.max preserved. Units 1m; original table moved to diary station."
        USMakeMaterials()
        saveMaxFile (USFolder+"cenario.max") quiet:true
        messageBox "Master scene and native bitmap materials prepared." title:"O Ultimo Sinal"
    )
    catch (format "%\n" (getCurrentExceptionStackTrace());try (USNote ("ERROR: "+getCurrentException())) catch();messageBox (getCurrentException()) title:"Cenario - stopped safely")
)
