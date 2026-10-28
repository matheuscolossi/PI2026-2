-- O Ultimo Sinal: execute through Scripting > Run Script in an EMPTY scene.
-- Native editable Box primitives and Unwrap UVW modifiers, not an imported mesh.
(
    local folder = getFilenamePath (getSourceFileName())
    local audit = folder + "evidencias\\producao_max.txt"
    fn note path value =
    (
        local stream = openFile path mode:"a"
        format "%\n" value to:stream
        close stream
    )
    try
    (
        if objects.count != 0 do throw "Scene must be empty; no existing objects will be removed."
        if doesFileExist (folder + "mesa.max") do throw "mesa.max already exists; refusing to overwrite."
        if doesFileExist (folder + "mesa.3ds") do throw "mesa.3ds already exists; refusing to overwrite."
        if not doesFileExist (folder + "wood.jpg") do throw "wood.jpg is missing."
        makeDir (folder + "evidencias") all:true
        local stream = createFile audit
        close stream
        note audit ("3ds Max version: " + (maxVersion() as string))
        note audit ("Language: " + (sysInfo.getMaxLanguage() as string))
        units.DisplayType = #Metric
        units.MetricType = #Meters
        units.SystemType = #Meters
        units.SystemScale = 1.0
        note audit "Units: display meters; 1 system unit = 1 meter."
        local wood = bitmapTexture filename:(folder + "wood.jpg") name:"WoodBitmap"
        local mat = standardMaterial name:"Wood" diffuse:(color 190 170 140) glossiness:12 specularLevel:10
        mat.diffuseMap = wood
        local pieces = #()
        append pieces (box name:"Tampo" width:1.60 length:0.90 height:0.12 pos:[0,0,0.88] mapCoords:true)
        local positions = #([-0.65,-0.30,0], [0.65,-0.30,0], [-0.65,0.30,0], [0.65,0.30,0])
        for i = 1 to 4 do append pieces (box name:("Perna" + i as string) width:0.10 length:0.10 height:0.88 pos:positions[i] mapCoords:true)
        note audit "Five editable Box primitives created."
        for obj in pieces do
        (
            obj.material = mat
            obj.wirecolor = color 160 110 65
            select obj
            max modify mode
            local uv = Unwrap_UVW()
            addModifier obj uv
            modPanel.setCurrentObject uv
            uv.unwrap2.setTVSubObjectMode 3
            local faces = uv.unwrap.numberPolygons()
            local allFaces = #{}
            for i = 1 to faces do allFaces[i] = true
            uv.unwrap2.selectPolygonsUpdate allFaces true
            uv.unwrap2.flattenMap 45.0 #() 0.02 true 0 true true
            subObjectLevel = 0
            local meshCopy = snapshotAsMesh obj
            note audit (obj.name + ": Box " + obj.width as string + " x " + obj.length as string + " x " + obj.height as string + "; triangles=" + meshCopy.numfaces as string + "; UVvertices=" + meshCopy.numtverts as string + "; Unwrap_UVW retained")
        )
        showTextureMap mat wood on
        select pieces
        viewport.setType #view_persp_user
        max zoomext sel all
        redrawViews()
        saveMaxFile (folder + "mesa.max") quiet:true
        note audit "Saved native mesa.max."
        exportFile (folder + "mesa.3ds") #noPrompt selectedOnly:true
        note audit "Exported mesa.3ds with native 3D Studio exporter."
        completeRedraw()
        local dib = gw.getViewportDib()
        dib.filename = folder + "evidencias\\modelo.png"
        save dib
        close dib
        select pieces[1]
        max modify mode
        local uv = pieces[1].modifiers[1]
        modPanel.setCurrentObject uv
        try
        (
            uv.renderuv_width = 1024
            uv.renderuv_height = 1024
            uv.renderuv_force2sided = true
            uv.renderuv_visibleedges = true
            uv.renderuv_seamedges = true
            uv.renderuv_showframebuffer = false
            uv.unwrap5.renderUV (folder + "evidencias\\uv.png")
            if doesFileExist (folder + "evidencias\\uv.png") then
                note audit "Native UV template present at evidencias/uv.png."
            else
                note audit "UV template not written: inspect the UV Editor; export to an ASCII path and verify the file before copying it into evidencias/uv.png."
        ) catch (note audit ("UV template: " + getCurrentException()))
        note audit "SUCCESS: geometry, editable UVs, Standard bitmap, MAX and 3DS saved."
        messageBox "Mesa created: five Boxes, Unwrap UVW, wood.jpg, mesa.max and mesa.3ds.\nContinue with UV inspection and game import validation." title:"O Ultimo Sinal"
    )
    catch
    (
        try (note audit ("ERROR: " + getCurrentException())) catch()
        messageBox (getCurrentException()) title:"Mesa - stopped safely"
    )
)
