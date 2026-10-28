fileIn (USFolder+"common.ms")
try
(
    with redraw off
    (
        for key in #("costa","torre") do
        (
            local root=getNodeByName ("US_"+key)
            if root==undefined do throw ("Missing native group "+key)
            local nodes=for n in root.children collect n
            local fixed=0
            for n in nodes where classOf n.baseObject==Cone do
            (
                n.radius2=.003
                USUnwrap n
                fixed+=1
            )
            select nodes
            exportFile (USScratch+key+"_fix.3ds") #noPrompt selectedOnly:true
            USNote ("REPAIRED cone apex "+key+" nodes="+(fixed as string)+" export="+USScratch+key+"_fix.3ds")
        )
        saveMaxFile (USFolder+"cenario.max") quiet:true
    )
    fileIn (USFolder+"props.ms")
)
catch(USNote(getCurrentExceptionStackTrace());messageBox(getCurrentException()) title:"Repair stopped safely")
