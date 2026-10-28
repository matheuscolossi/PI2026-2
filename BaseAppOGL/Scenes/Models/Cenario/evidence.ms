-- Reopen the saved editable scene and render its actual floor UV template.
try
(
    saveMaxFile (USFolder+"cenario.max") quiet:true
    if not loadMaxFile (USFolder+"cenario.max") quiet:true useFileUnits:true do throw "Master did not reopen."
    USWriteAudit()
    local root=getNodeByName "US_piso"
    setGroupOpen root true
    local node=root.children[1]
    select node
    max modify mode
    local uv=(for m in node.modifiers where classOf m==Unwrap_UVW collect m)[1]
    modPanel.setCurrentObject uv
    uv.unwrap2.setTVSubObjectMode 3
    local faces=#{}
    for i=1 to uv.unwrap.numberPolygons() do faces[i]=true
    uv.unwrap2.selectPolygonsUpdate faces true
    subObjectLevel=3
    uv.unwrap.edit()
    uv.renderuv_width=1024;uv.renderuv_height=1024
    uv.renderuv_force2sided=true;uv.renderuv_showframebuffer=false
    uv.renderuv_fillmode=1
    uv.unwrap5.renderUV (USScratch+"piso_uv.png")
    if not doesFileExist (USScratch+"piso_uv.png") do throw "Native UV template missing."
    copyFile (USScratch+"piso_uv.png") (USFolder+"evidencias\\piso_uv.png")
    subObjectLevel=0
    USNote "REOPEN verified cenario.max; native piso UV template exported; UV1 atlas supports CompleteMap."
)
catch (USNote(getCurrentExceptionStackTrace());messageBox(getCurrentException()) title:"Evidence stopped safely")
