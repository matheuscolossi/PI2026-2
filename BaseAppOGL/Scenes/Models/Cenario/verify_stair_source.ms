global USWriteAudit
(
    loadMaxFile (USFolder+"cenario.max") quiet:true
    local root=getNodeByName "US_escada"
    if root==undefined or getUserProp root "repair20261020"!="complete" do throw "Saved source repair marker absent."
    local out=createFile (USFolder+"evidencias\\escada_visibilidade.txt")
    for n in root.children do format "% hidden=% renderable=% castShadows=%\n" n.name n.isHidden n.renderable n.castShadows to:out
    close out
    USWriteAudit()
    USNote "STAIR SOURCE reopened and native audit completed."
    format "STAIR SOURCE OK\n"
)
