-- Source evidence: the actual scene nodes, primitive classes and UV modifiers.
global USSourceAudit
fn USWriteAudit =
(
    local keys=#("ilha","costa","costalod","caminho","cais","carga","caisbox","anexo","piso","torre","porta","cadeira","caneca","armario","armporta","botas","escada","radio","quadro","botao","mapa","papel","relogio","arquivo","lumin","lampada","diario","alicate","marcas","chave","foto","agua","janvid","vidfar","led","fragment","radioluz")
    local output=createFile (USFolder+"evidencias\\source_nodes.tsv")
    format "asset\tnode\tprimitive\ttriangles\tuv_vertices\tUnwrap_modifiers\n" to:output
    local bad=0;local total=0
    for key in keys do
    (
        local root=getNodeByName ("US_"+key)
        if root==undefined do throw ("Missing source group "+key)
        for n in root.children do
        (
            local uvCount=(for modifier in n.modifiers where classOf modifier==Unwrap_UVW collect modifier).count
            local msh=snapshotAsMesh n
            if uvCount<1 or msh.numtverts<1 do bad+=1
            format "%\t%\t%\t%\t%\t%\n" key n.name (classOf n.baseObject) msh.numfaces msh.numtverts uvCount to:output
            total+=1
        )
    )
    close output
    USNote ("SOURCE AUDIT groups=37 nodes="+(total as string)+" nodes_missing_UV="+(bad as string))
    if bad>0 do throw "Native source UV audit failed."
    saveMaxFile (USFolder+"cenario.max") quiet:true
)
try (USWriteAudit();messageBox "37 native source groups and their Unwrap modifiers audited." title:"O Ultimo Sinal")
catch (USNote(getCurrentExceptionStackTrace());messageBox(getCurrentException()) title:"Source audit stopped safely")
