-- O Ultimo Sinal: native PROP CONSTRUCTORS ONLY, not an export runner.
-- Loading this file defines functions. It does not reset/save/export a scene.
-- Call from the authorized native Max production runner, in its fresh asset scene.
-- Game-local [X,Y-up,Z] -> Max [X,-Z,Y]. Door/cabinet pivots must NOT be recentered.

struct USPropMaterials (Wood, Metal, Paint, Paper, Brass, Glass, Shoe, Blue)

fn USPropToMax p = [p.x,-p.z,p.y]

fn USPropName prefix nodes =
(
    local shortPrefix=substring prefix 1 (amin #(4,prefix.count))
    for i=1 to nodes.count do nodes[i].name=shortPrefix+"_"+(formattedPrint i format:"03d")
    nodes
)

fn USPropBox outNodes center size mat bevel:0.004 yaw:0 =
(
    if size.x<=0 or size.y<=0 or size.z<=0 do throw "Nonpositive prop box dimension."
    local small=amin #(size.x,size.y,size.z)
    local edge=amin #(bevel,small*0.18)
    local p=USPropToMax (center-[0,size.y*0.5,0])
    local obj=undefined
    if edge>0.00001 then
        obj=chamferBox width:size.x length:size.z height:size.y fillet:edge Fillet_Segments:1 mapCoords:true pos:p
    else
        obj=box width:size.x length:size.z height:size.y mapCoords:true pos:p
    obj.material=mat
    if yaw!=0 do rotate obj (angleAxis yaw [0,0,1])
    append outNodes obj
    obj
)

fn USPropRod outNodes start finish radius mat sides:16 =
(
    local a=USPropToMax start
    local b=USPropToMax finish
    local d=b-a
    if (length d)<0.000001 or radius<=0 do throw "Degenerate prop cylinder."
    local obj=cylinder radius:radius height:(length d) sides:sides capsegs:1 heightsegs:1 mapCoords:true
    obj.transform=matrixFromNormal (normalize d)
    obj.pos=a
    obj.material=mat
    append outNodes obj
    obj
)

fn USPropRing outNodes center major minor mat normal:[0,1,0] =
(
    local obj=torus radius1:major radius2:minor segs:20 sides:6 smooth:2 mapCoords:true
    obj.transform=matrixFromNormal (normalize (USPropToMax normal))
    obj.pos=USPropToMax center
    obj.material=mat
    append outNodes obj
    obj
)

fn USPropPlate outNodes outline thickness mat =
(
    -- An extruded polygon in game X/Z: a shallow, closed torn-paper prism.
    local n=outline.count
    local vv=#()
    for p in outline do append vv (USPropToMax [p.x,0,p.y])
    for p in outline do append vv (USPropToMax [p.x,thickness,p.y])
    local ff=#()
    for i=2 to n-1 do
    (
        append ff [1,i,i+1]
        append ff [n+1,n+i+1,n+i]
    )
    for i=1 to n do
    (
        local j=if i==n then 1 else i+1
        append ff [i,n+j,j]
        append ff [i,n+i,n+j]
    )
    local obj=mesh vertices:vv faces:ff
    obj.material=mat
    append outNodes obj
    obj
)

fn USPropOpenCup outNodes mat =
(
    -- Actual inner/outer walls: no solid cylinder masquerading as an empty cup.
    local vv=#()
    local ff=#()
    local rings=#(#(0.085,0.012),#(0.085,0.18),#(0.073,0.012),#(0.073,0.18))
    local segments=24
    for ring in rings do for i=0 to segments-1 do
    (
        local a=i*360.0/segments
        append vv (USPropToMax [ring[1]*cos a,ring[2],ring[1]*sin a])
    )
    for i=1 to segments do
    (
        local j=if i==segments then 1 else i+1
        -- outer wall, inner wall, top lip, bottom annulus
        for q in #(#(i,j,segments+j,segments+i), #(2*segments+j,2*segments+i,3*segments+i,3*segments+j), #(segments+i,segments+j,3*segments+j,3*segments+i), #(j,i,2*segments+i,2*segments+j)) do
        (
            append ff [q[1],q[3],q[2]]
            append ff [q[1],q[4],q[3]]
        )
    )
    local obj=mesh vertices:vv faces:ff
    obj.material=mat
    append outNodes obj
    USPropRod outNodes [0,0,0] [0,0.014,0] 0.084 mat sides:24
    USPropRing outNodes [0.10,0.098,0] 0.050 0.011 mat normal:[0,0,1]
    obj
)

fn USPropBuild kind materials prefix:undefined =
(
    -- mats = USPropMaterials Wood:woodMat Metal:metalMat Paint:paintMat ...
    -- Every supplied material should be Standard with the runner's real Bitmap.
    -- Accept root USMaterials as well as this helper's smaller palette struct.
    local glassMat=if isProperty materials #Glass then materials.Glass else materials.Luminous
    local m=USPropMaterials Wood:materials.Wood Metal:materials.Metal Paint:materials.Paint Paper:materials.Paper Brass:materials.Brass Glass:glassMat Shoe:materials.Shoe Blue:materials.Blue
    for role in #(#Wood,#Metal,#Paint,#Paper,#Brass,#Glass,#Shoe,#Blue) do
        if (getProperty m role)==undefined do throw ("Missing prop palette material: "+(role as string))
    local nodes=#()
    local k=toLower (kind as string)
    local requestedKind=k
    if k=="marcas" do k="pegadas"
    if k=="quadro" do k="painel"
    if k=="mapa" do k="carta"
    if k=="ficha" or k=="registro" or k=="cartahel" do k="papel"
    case k of
    (
        "diario":
        (
            USPropBox nodes [0,0.008,0] [.56,.016,.40] m.Wood bevel:.006
            USPropBox nodes [0,.098,0] [.56,.016,.40] m.Wood bevel:.006
            USPropBox nodes [.008,.053,0] [.50,.077,.355] m.Paper bevel:.002
            USPropBox nodes [-.267,.053,0] [.025,.094,.385] m.Wood
            for z in #(-.15,.15) do USPropBox nodes [-.268,.053,z] [.032,.09,.024] m.Brass bevel:.002
            for x in #(-.24,.24) do for z in #(-.165,.165) do
                USPropBox nodes [x,.109,z] [.04,.006,.045] m.Brass bevel:.001
            USPropBox nodes [-.19,.108,-.02] [.018,.004,.33] m.Paint bevel:0
        )
        "alicate":
        (
            for sign in #(-1,1) do
            (
                USPropBox nodes [sign*.085,.029,.10] [.065,.055,.32] m.Shoe bevel:.012 yaw:(sign*-12)
                USPropBox nodes [sign*.028,.036,-.055] [.053,.057,.13] m.Metal yaw:(sign*20)
                USPropBox nodes [sign*.050,.043,-.155] [.06,.060,.12] m.Metal yaw:(sign*26)
                for j=0 to 3 do USPropBox nodes [sign*.024,.044,-.19+j*.022] [.017,.063,.009] m.Metal bevel:0
            )
            USPropRod nodes [0,.016,-.028] [0,.074,-.028] .027 m.Brass sides:16
            -- Fragment is a separate export/placement, not added to this asset.
        )
        "fragment":
        (
            USPropBox nodes [0,.0175,0] [.045,.035,.07] m.Metal bevel:.002 yaw:12
        )
        "pegadas":
        (
            for i=0 to 2 do
            (
                local p=[(if (mod i 2)==0 then -.15 else .17),.004,(i-1)*.48]
                USPropBox nodes (p+[0,0,-.062]) [.14,.008,.095] m.Shoe bevel:.001 yaw:(i*17-12)
                USPropBox nodes (p+[0,0,.045]) [.15,.008,.15] m.Shoe bevel:.001 yaw:(i*17-12)
                for j=0 to 3 do for side in #(-1,1) do
                    USPropBox nodes (p+[side*.031,.006,-.03+j*.038]) [.073,.004,.012] m.Metal bevel:0 yaw:(side*28)
            )
        )
        "chave":
        (
            USPropRing nodes [0,.015,0] .052 .011 m.Brass
            USPropBox nodes [.112,.015,0] [.17,.022,.028] m.Brass bevel:.003
            USPropBox nodes [.173,.015,.031] [.026,.022,.075] m.Brass bevel:.002
            USPropBox nodes [.142,.015,.023] [.023,.022,.057] m.Brass bevel:.002
        )
        "foto":
        (
            USPropPlate nodes #([-.29,-.19],[-.017,-.19],[-.03,-.13],[-.012,-.065],[-.027,.0],[-.008,.07],[-.025,.14],[-.017,.19],[-.29,.19]) .008 m.Paper
            USPropPlate nodes #([.018,-.17],[.29,-.17],[.29,.21],[.018,.21],[.010,.16],[.027,.10],[.011,.04],[.027,-.03],[.014,-.10]) .008 m.Paper
            -- Actual image goes in a supplied Bitmap, with anonymous boat/shapes.
            USPropBox nodes [-.155,.010,0] [.23,.004,.32] m.Paint bevel:0
            USPropBox nodes [.155,.010,.02] [.23,.004,.32] m.Paint bevel:0
            for x in #(-.15,.15) do
            (
                USPropBox nodes [x,.014,-.005] [.23,.004,.070] m.Wood bevel:0
                USPropBox nodes [x+.025,.018,-.060] [.085,.004,.07] m.Paper bevel:0
            )
        )
        "papel":
        (
            USPropBox nodes [0,.002,0] [.47,.004,.32] m.Paper bevel:0
            USPropBox nodes [.008,.006,-.005] [.45,.004,.305] m.Paper bevel:0 yaw:1.5
            USPropBox nodes [0,.012,-.145] [.12,.014,.025] m.Metal bevel:.002
            -- Shared by maintenance, log and optional letter; essential text stays UI.
        )
        "carta":
        (
            USPropBox nodes [0,0,-.015] [1.23,.96,.07] m.Wood bevel:.008
            USPropBox nodes [0,0,.028] [1.10,.83,.015] m.Paper bevel:0
            for x in #(-.575,.575) do USPropBox nodes [x,0,.041] [.035,.89,.018] m.Brass bevel:.002
            for y in #(-.425,.425) do USPropBox nodes [0,y,.041] [1.16,.035,.018] m.Brass bevel:.002
            -- No new route, code or location labels invented by geometry.
        )
        "painel":
        (
            USPropBox nodes [0,0,0] [.74,.92,.23] m.Paint bevel:.02
            USPropBox nodes [0,0,.122] [.64,.80,.025] m.Metal bevel:.006
            for x in #(-.295,.295) do for y in #(-.365,.365) do
                USPropRod nodes [x,y,.136] [x,y,.148] .013 m.Brass sides:8
            for y in #(-.18,0,.18) do
            (
                USPropBox nodes [-.09,y,.144] [.22,.10,.014] m.Shoe bevel:.004
                USPropBox nodes [.20,y,.144] [.15,.065,.014] m.Paper bevel:.001
            )
            USPropBox nodes [0,.33,.144] [.43,.055,.015] m.Paper bevel:0
        )
        "alavanca":
        (
            -- Axis passes through local origin. Runtime tilts for each breaker.
            USPropRod nodes [0,-.015,-.008] [0,.015,-.008] .024 m.Brass sides:12
            USPropBox nodes [0,0,.035] [.07,.085,.065] m.Metal bevel:.006
            USPropBox nodes [0,0,.085] [.11,.095,.035] m.Shoe bevel:.007
        )
        "botao":
        (
            -- Root World contract is a unit mesh scaled .04/.08/.20 per circuit.
            USPropBox nodes [0,0,0] [1,1,1] m.Metal bevel:.08
            for z in #(-.28,0,.28) do USPropBox nodes [0,.51,z] [.78,.03,.065] m.Shoe bevel:.008
        )
        "led":
        (
            -- Runtime applies its state color/emission, never baked green/red.
            USPropBox nodes [0,0,0] [1,1,1] m.Glass bevel:.09
        )
        "painled":
        (
            USPropRing nodes [0,0,0] .025 .007 m.Brass normal:[0,0,1]
            USPropRod nodes [0,0,-.008] [0,0,.008] .021 m.Glass sides:16
        )
        "radio":
        (
            USPropBox nodes [0,0,0] [.80,.39,.36] m.Paint bevel:.018
            USPropBox nodes [0,-.008,.188] [.70,.29,.025] m.Metal bevel:.008
            USPropBox nodes [-.17,.067,.206] [.27,.08,.012] m.Glass bevel:.002
            for y in #(-.10,-.075,-.05,-.025) do for x in #(-.285,-.25,-.215,-.18,-.145,-.11,-.075) do
                USPropBox nodes [x,y,.207] [.012,.012,.006] m.Shoe bevel:0
            for x in #(.13,.27) do
            (
                USPropRing nodes [x,-.045,.211] .036 .006 m.Brass normal:[0,0,1]
                USPropRod nodes [x,-.045,.207] [x,-.045,.249] .034 m.Shoe sides:16
            )
            for x in #(-.29,.29) do USPropBox nodes [x,-.212,0] [.10,.035,.23] m.Shoe bevel:.006
            USPropRod nodes [-.35,.20,-.06] [-.54,.85,-.06] .009 m.Metal sides:12
            USPropRod nodes [-.54,.85,-.06] [-.565,.94,-.06] .006 m.Brass sides:10
            for x in #(-.26,.26) do USPropBox nodes [x,.245,0] [.035,.11,.06] m.Metal
            USPropBox nodes [0,.287,0] [.55,.035,.06] m.Metal bevel:.008
        )
        "radioluz":
        (
            USPropBox nodes [0,0,0] [.23,.055,.006] m.Glass bevel:.001
        )
        "dial":
        (
            -- Only a mechanical index line. No channel 6 revealed by this model.
            USPropBox nodes [0,.021,0] [.005,.028,.005] m.Paper bevel:0
        )
        "armario":
        (
            USPropBox nodes [0,0,.58] [1.25,2.10,.08] m.Paint bevel:.008
            for x in #(-.62,.62) do USPropBox nodes [x,0,0] [.08,2.10,1.15] m.Paint bevel:.006
            for y in #(-.97,.15,1.03) do USPropBox nodes [0,y,0] [1.25,.08,1.15] m.Wood
            USPropBox nodes [0,.60,.52] [.95,.56,.025] m.Metal
            for x in #(-.34,-.12,.10,.32) do
            (
                USPropRod nodes [x,.68,.50] [x,.68,.33] .011 m.Brass sides:12
                USPropRod nodes [x,.68,.33] [x,.72,.33] .011 m.Brass sides:12
            )
            for y in #(-.78,-.45,-.12,.30,.72) do for x in #(-.566,.566) do
                USPropRod nodes [x,y,.35] [x,y,.38] .009 m.Metal sides:8
        )
        "armporta":
        (
            -- Hinge origin is local [0,0,0]. Leaf X=0..1.24, Y=-1.05..1.05.
            USPropBox nodes [.62,0,0] [1.24,2.10,.06] m.Paint bevel:.007
            USPropBox nodes [.62,.04,-.035] [1.06,1.83,.025] m.Paint bevel:.01
            for y in #(-.77,.77) do USPropRod nodes [0,y-.09,0] [0,y+.09,0] .022 m.Brass sides:12
            USPropRod nodes [1.06,-.105,-.065] [1.06,.105,-.065] .018 m.Metal sides:12
            USPropBox nodes [.58,.68,-.052] [.60,.13,.018] m.Paper bevel:.001
        )
        "armled":
        (
            USPropRing nodes [0,0,0] .033 .006 m.Brass normal:[0,0,-1]
            USPropRod nodes [0,0,-.01] [0,0,.002] .027 m.Glass sides:16
        )
        "porta":
        (
            -- Exact gameplay hinge contract; never auto-center these vertices.
            USPropBox nodes [.97,0,0] [1.94,2.70,.13] m.Wood bevel:.009
            for y in #(-.65,.40) do
            (
                USPropBox nodes [.97,y,-.074] [1.48,.70,.025] m.Paint bevel:.008
                for x in #(.18,1.76) do USPropBox nodes [x,y,-.092] [.045,.75,.025] m.Wood bevel:.002
                for yy in #(y-.375,y+.375) do USPropBox nodes [.97,yy,-.092] [1.62,.045,.025] m.Wood bevel:.002
            )
            for y in #(-.93,0,.93) do USPropRod nodes [0,y-.11,0] [0,y+.11,0] .023 m.Brass sides:12
            USPropBox nodes [1.73,-.12,.087] [.09,.27,.022] m.Brass bevel:.003
            USPropRod nodes [1.73,-.12,.09] [1.73,-.12,.18] .018 m.Brass sides:12
            USPropRod nodes [1.58,-.12,.18] [1.76,-.12,.18] .018 m.Brass sides:12
        )
        "relogio":
        (
            USPropRod nodes [0,0,-.035] [0,0,.025] .30 m.Metal sides:32
            USPropRod nodes [0,0,.025] [0,0,.033] .26 m.Paper sides:32
            USPropRing nodes [0,0,.036] .278 .012 m.Brass normal:[0,0,1]
            for i=0 to 11 do
            (
                local a=i*30.0
                USPropBox nodes [.225*sin a,.225*cos a,.042] [.015,.032,.004] m.Metal bevel:0 yaw:0
            )
            USPropRod nodes [0,0,.05] [0,.18,.05] .006 m.Metal sides:8
            USPropRod nodes [0,0,.053] [.14,.05,.053] .007 m.Metal sides:8
            USPropRod nodes [0,0,.048] [0,0,.063] .014 m.Brass sides:12
        )
        "arquivo":
        (
            -- Front is game -X, matching the existing east-wall filing cabinet.
            USPropBox nodes [0,.48,0] [.55,.96,1.40] m.Paint bevel:.012
            for y in #(.18,.48,.78) do
            (
                USPropBox nodes [-.294,y,0] [.035,.25,1.29] m.Metal bevel:.003
                USPropRod nodes [-.32,y-.02,-.13] [-.32,y-.02,.13] .018 m.Brass sides:12
                USPropBox nodes [-.316,y+.065,0] [.012,.055,.27] m.Paper bevel:.001
                for z in #(-.585,.585) do USPropBox nodes [-.317,y,z] [.01,.15,.02] m.Brass bevel:.001
            )
        )
        "cadeira":
        (
            USPropBox nodes [0,.48,0] [.55,.10,.55] m.Wood bevel:.012
            for x in #(-.22,.22) do for z in #(-.22,.22) do
                USPropRod nodes [x,0,z] [x,.45,z] .027 m.Metal sides:12
            for z in #(-.22,.22) do USPropRod nodes [-.24,.44,z] [-.24,.98,z] .024 m.Metal sides:12
            USPropBox nodes [-.24,.86,0] [.07,.23,.55] m.Paint bevel:.012
            for z in #(-.22,.22) do USPropRod nodes [-.22,.20,z] [.22,.20,z] .017 m.Metal sides:10
            USPropRod nodes [-.22,.20,-.22] [-.22,.20,.22] .017 m.Metal sides:10
        )
        "caneca":
        (
            USPropOpenCup nodes m.Paper
            -- Cold liquid is a separate shallow insert in the still cup.
            USPropRod nodes [0,.153,0] [0,.158,0] .071 m.Shoe sides:24
        )
        "botas":
        (
            for x in #(-.165,.165) do
            (
                USPropBox nodes [x,.038,-.025] [.23,.075,.44] m.Shoe bevel:.018
                USPropBox nodes [x,.137,-.092] [.22,.16,.31] m.Shoe bevel:.025
                USPropBox nodes [x,.28,.045] [.215,.43,.24] m.Shoe bevel:.018
                USPropRing nodes [x,.495,.045] .084 .011 m.Metal
                for j=0 to 3 do
                (
                    local y=.22+j*.06
                    USPropRod nodes [x-.055,y,-.082] [x+.055,y+.027,-.085] .006 m.Brass sides:8
                    USPropRod nodes [x+.055,y,-.082] [x-.055,y+.027,-.085] .006 m.Brass sides:8
                )
                -- Circular marks differ from the zig-zag visitor footprint.
                for z in #(-.17,-.07,.045,.135) do USPropRing nodes [x,.007,z] .035 .007 m.Metal
            )
        )
        "caixazul":
        (
            USPropBox nodes [0,.15,0] [.55,.30,.55] m.Blue bevel:.012
            USPropBox nodes [0,.307,0] [.46,.018,.45] m.Metal bevel:.004
            for x in #(-.16,0,.16) do
            (
                USPropBox nodes [x,.337,0] [.034,.045,.15] m.Brass bevel:.003
                USPropBox nodes [x,.328,.08] [.085,.015,.04] m.Paper bevel:.001
            )
            for x in #(-.23,.23) do for z in #(-.23,.23) do
                USPropRod nodes [x,.31,z] [x,.325,z] .012 m.Metal sides:8
        )
        "caixatam":
        (
            -- Rear edge hinge at local origin. Local lid extends towards -Z.
            USPropBox nodes [0,0,-.275] [.57,.045,.55] m.Blue bevel:.009
            USPropBox nodes [0,-.025,-.275] [.46,.015,.44] m.Shoe bevel:.002
            for x in #(-.19,.19) do USPropRod nodes [x-.035,0,0] [x+.035,0,0] .02 m.Metal sides:12
        )
        "carga":
        (
            -- Unit central-origin crate. Runtime scales to the two existing bounds.
            USPropBox nodes [0,0,0] [1,1,1] m.Wood bevel:.012
            for x in #(-.31,.31) do
            (
                USPropBox nodes [x,0,.507] [.052,1.02,.018] m.Metal bevel:.003
                USPropBox nodes [x,0,-.507] [.052,1.02,.018] m.Metal bevel:.003
                USPropBox nodes [x,.507,0] [.052,.018,1.02] m.Metal bevel:.003
                USPropBox nodes [x,-.507,0] [.052,.018,1.02] m.Metal bevel:.003
            )
            for y in #(-.28,0,.28) do USPropBox nodes [0,y,.514] [.96,.009,.009] m.Metal bevel:0
            USPropBox nodes [0,.02,.524] [.28,.18,.009] m.Paper bevel:0
        )
        "lumin":
        (
            USPropRod nodes [0,0,0] [0,-.33,0] .015 m.Metal sides:12
            local vv=#()
            local ff=#()
            for ring in #(#(.075,-.22),#(.30,-.40)) do for i=0 to 23 do
                append vv (USPropToMax [ring[1]*cos(i*15),ring[2],ring[1]*sin(i*15)])
            for i=1 to 24 do
            (
                local j=if i==24 then 1 else i+1
                append ff [i,j,j+24]
                append ff [i,j+24,i+24]
            )
            local hood=mesh vertices:vv faces:ff
            hood.material=m.Metal
            append nodes hood
            USPropRing nodes [0,-.40,0] .30 .008 m.Brass
        )
        "lampada":
        (
            USPropRod nodes [0,-.5,0] [0,.5,0] 1 m.Glass sides:24
        )
        "agua":
        (
            local v=for p in #([-.5,0,-.5],[.5,0,-.5],[.5,0,.5],[-.5,0,.5]) collect (USPropToMax p)
            local obj=mesh vertices:v faces:#([1,3,2],[1,4,3])
            obj.material=m.Glass
            append nodes obj
        )
        "janvid":
        (
            local v=for p in #([-.5,0,-.5],[.5,0,-.5],[.5,0,.5],[-.5,0,.5]) collect (USPropToMax p)
            local obj=mesh vertices:v faces:#([1,3,2],[1,4,3])
            obj.material=m.Glass
            append nodes obj
        )
        "vidfar":
        (
            -- Open cylinder shell: no transparent caps crossing the lens.
            local vv=#()
            local ff=#()
            for y in #(-.5,.5) do for i=0 to 31 do
                append vv (USPropToMax [cos(i*11.25),y,sin(i*11.25)])
            for i=1 to 32 do
            (
                local j=if i==32 then 1 else i+1
                append ff [i,j+32,j]
                append ff [i,i+32,j+32]
            )
            local obj=mesh vertices:vv faces:ff
            obj.material=m.Glass
            append nodes obj
        )
        default:throw ("Unknown prop kind: "+k)
    )
    -- Exact origins used by World's existing native inventory contract.
    local lift=case k of ("diario":-.055; "alicate":-.029; "foto":-.006; "papel":-.0075; "caneca":-.09; "armario":1.05; default:0.0)
    if lift!=0 do for obj in nodes do obj.pos += [0,0,lift]
    local yaw=case requestedKind of ("quadro":-90.0; "mapa":180.0; default:0.0)
    if yaw!=0 do for obj in nodes do obj.transform=obj.transform*(rotateZMatrix yaw)
    if prefix==undefined do prefix=requestedKind
    USPropName prefix nodes
)

fn USPropApplyUnwrap nodes =
(
    -- Identical flatten API to the verified Mesa pipeline. No export/save here.
    local audit=#()
    for obj in nodes do
    (
        if not isValidNode obj do throw "Invalid node in prop UV list."
        select obj
        max modify mode
        local uv=Unwrap_UVW()
        addModifier obj uv
        modPanel.setCurrentObject uv
        uv.unwrap2.setTVSubObjectMode 3
        local allFaces=#{}
        local faceCount=uv.unwrap.numberPolygons()
        for face=1 to faceCount do allFaces[face]=true
        uv.unwrap2.selectPolygonsUpdate allFaces true
        uv.unwrap2.flattenMap 45.0 #() 0.02 true 0 true true
        subObjectLevel=0
        local copyMesh=snapshotAsMesh obj
        if copyMesh.numfaces==0 or copyMesh.numtverts==0 do throw ("No UV or faces: "+obj.name)
        append audit #(obj.name,copyMesh.numfaces,copyMesh.numtverts)
    )
    audit
)

fn USPropGroup nodes groupName =
(
    if nodes.count==0 do throw "Cannot group empty prop."
    group nodes name:groupName
)

-- Source references for native primitive interfaces:
-- Autodesk ChamferBox: GUID-62783832-81E3-4150-A977-7FC984411384
-- Autodesk Cylinder: GUID-EBD8F9D0-73EF-4CD4-8F37-D57E76724D80
-- Autodesk Torus: GUID-5A7881FE-32F5-4165-9315-5491420B3E68
-- The runner must validate this helper natively before claiming production.
