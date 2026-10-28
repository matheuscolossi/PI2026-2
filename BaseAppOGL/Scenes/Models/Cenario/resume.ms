fileIn (USFolder+"common.ms")
fileIn (USFolder+"PropsHelpers.ms")
if isValidNode USProbe do (USProbe.renderable=false;hide USProbe)
if isValidNode USProbeGroup do hide USProbeGroup
fileIn (USFolder+"architecture.ms")
