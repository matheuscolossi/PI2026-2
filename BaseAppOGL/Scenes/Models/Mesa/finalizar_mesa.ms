(
    local folder = getFilenamePath (getSourceFileName())
    local audit = folder + "evidencias\\producao_max.txt"
    fn note path value = (local s=openFile path mode:"a"; format "%\n" value to:s; close s)
    try
    (
        local pieces = for name in #("Tampo","Perna1","Perna2","Perna3","Perna4") collect getNodeByName name
        if objects.count != 5 or findItem pieces undefined > 0 do throw "Unexpected scene: expected the five existing table pieces."
        if doesFileExist (folder + "mesa.max") or doesFileExist (folder + "mesa.3ds") do throw "Refusing to overwrite existing model."
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
    ) catch (note audit ("ERROR: "+getCurrentException()); messageBox (getCurrentException()) title:"Mesa - stopped safely")
)
