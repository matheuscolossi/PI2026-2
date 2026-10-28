-- Run inside the native MAXScript listener. Only the generated table is touched.
(
    local folder = maxFilePath
    local logPath = folder + "evidencias\\conferencia_max.txt"
    local stream = createFile logPath
    local pieces = for n in #("Tampo","Perna1","Perna2","Perna3","Perna4") collect getNodeByName n
    local originalMaterials = for obj in pieces collect (if obj != undefined then obj.material else undefined)
    try
    (
        if objects.count != 5 or findItem pieces undefined > 0 do throw "Expected only the generated five table pieces."
        if maxFileName != "mesa.max" do throw "Expected mesa.max."
        format "Native scene: %\nObjects: %\nUnits: % / %\n" (folder+maxFileName) pieces.count units.SystemType units.SystemScale to:stream
        select pieces[1]
        max modify mode
        local uv = pieces[1].modifiers[1]
        modPanel.setCurrentObject uv
        uv.unwrap2.setTVSubObjectMode 3
        local allFaces = #{}
        for i=1 to uv.unwrap.numberPolygons() do allFaces[i]=true
        uv.unwrap2.selectPolygonsUpdate allFaces true
        subObjectLevel = 3
        uv.unwrap.edit()
        uv.renderuv_width=1024
        uv.renderuv_height=1024
        uv.renderuv_force2sided=true
        uv.renderuv_showframebuffer=true
        uv.renderuv_fillmode=1
        uv.unwrap5.renderUV @"C:\Users\mlc17\Documents\Codex\2026-09-08\https-drive-google-com-drive-u\work\mesa_uv_tampo.png"
        format "UV template (ASCII path) exists: %\n" (doesFileExist @"C:\Users\mlc17\Documents\Codex\2026-09-08\https-drive-google-com-drive-u\work\mesa_uv_tampo.png") to:stream
        subObjectLevel=0
        local checkerMap = checker name:"UV_Check" color1:(color 235 235 235) color2:(color 45 45 45)
        checkerMap.coords.U_Tiling=8
        checkerMap.coords.V_Tiling=8
        local checkerMat=standardMaterial name:"UV_QA_ONLY" diffuseMap:checkerMap
        showTextureMap checkerMat checkerMap on
        for obj in pieces do obj.material=checkerMat
        select pieces
        max zoomext sel all
        completeRedraw()
        local dib=gw.getViewportDib()
        dib.filename=folder+"evidencias\\checker.png"
        save dib
        close dib
        for i=1 to pieces.count do pieces[i].material=originalMaterials[i]
        completeRedraw()
        dib=gw.getViewportDib()
        dib.filename=folder+"evidencias\\modelo.png"
        save dib
        close dib
        saveMaxFile (folder+"mesa.max") quiet:true
        local loaded=loadMaxFile (folder+"mesa.max") quiet:true useFileUnits:true
        format "Reopened saved MAX file: %\n" loaded to:stream
        for obj in objects do format "% | % | % | %\n" obj.name (classOf obj.baseObject) (classOf obj.modifiers[1]) (classOf obj.material) to:stream
        format "Wood restored before saving/reopening. No checker exported to 3DS.\n" to:stream
        close stream
        messageBox "Mesa saved and reopened. Checker and model evidence saved." title:"O Ultimo Sinal"
    ) catch
    (
        for i=1 to pieces.count where pieces[i] != undefined do pieces[i].material=originalMaterials[i]
        format "ERROR: %\n" (getCurrentException()) to:stream
        close stream
        messageBox (getCurrentException()) title:"Mesa verification"
    )
)
