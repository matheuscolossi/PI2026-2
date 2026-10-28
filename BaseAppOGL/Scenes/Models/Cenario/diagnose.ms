select USProbe
USNote ("PREEXPORT "+(isValidNode USProbe as string)+" count="+(objects.count as string))
exportFile (USScratch+"probe.3ds") #noPrompt selectedOnly:true
USNote ("POSTEXPORT "+(isValidNode USProbe as string)+" count="+(objects.count as string))
global USProbeGroup=group #(USProbe) name:"US_Probe"
USNote ("POSTGROUP "+(isValidNode USProbe as string)+" count="+(objects.count as string))
saveMaxFile (USFolder+"diagnostic.max") quiet:true
USNote ("POSTSAVE "+(isValidNode USProbe as string)+" count="+(objects.count as string))
