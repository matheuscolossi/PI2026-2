-- Native scene construction. World coordinates preserved, metres, no reset.
global USPending=#(), USStageKey=""
fn USAdd arr n = (if n!=undefined do append arr n;n)
fn USArch key =
(
    local a=#()
    local m=USMats
    case key of
    (
        "ilha":
        (
            local vv=#([0,0,-.025]);local ff=#();local count=96
            for i=0 to count-1 do
            (
                local t=i*360.0/count;local c=cos t;local s=sin t
                local x=15.35*(if c<0 then -1 else 1)*(abs c)^(1.0/3)
                local z=23.85*(if s<0 then -1 else 1)*(abs s)^(1.0/3)
                append vv (USG [x,-.025,z])
            )
            for i=1 to count do append ff [1,(if i==count then 2 else i+2),i+1]
            local n=mesh name:(USName "Is") vertices:vv faces:ff
            n.material=m.Grass;append a n
        )
        "costa":
        (
            for i=0 to 47 do
            (
                local t=i*360.0/48;local c=cos t;local s=sin t
                local v=.8+.22*sin(i*7.12*180/pi)
                local x=15.5*(if c<0 then -1 else 1)*(abs c)^(1.0/3)
                local z=24*(if s<0 then -1 else 1)*(abs s)^(1.0/3)
                USAdd a (USRot (USCylinder [x,-.48,z] [1.5*v,.92,1.9*v] m.Stone sides:12) (i*43) [.3,1,.5])
                USAdd a (USRot (USCone [x+.4,-.22,z+.3] [1.1*v,.9,1.2] m.Stone) 18 [1,0,1])
            )
        )
        "costalod":
        (
            for i=0 to 27 do
            (
                local t=i*360.0/28;local c=cos t;local s=sin t;local v=.8+.22*sin(i*7.12*180/pi)
                local x=15.5*(if c<0 then -1 else 1)*(abs c)^(1.0/3)
                local z=24*(if s<0 then -1 else 1)*(abs s)^(1.0/3)
                USAdd a (USRot (USCylinder [x,-.48,z] [1.5*v,.92,1.9*v] m.Stone sides:8) (i*43) [.3,1,.5])
            )
        )
        "caminho":
        (
            for j=0 to 12 do USAdd a (USBox [.07*sin(j*2*180/pi),.009,5.6+j*.9] [1.6+.13*sin(j*180/pi),.06,.78] m.Stone)
        )
        "cais":
        (
            for j=0 to 9 do USAdd a (USBox [0,.07,17.5+j*.35] [2.15,.14,.31] m.Wood)
            for x in #(-.83,.83) do USAdd a (USBox [x,-.08,19] [.16,.28,3.55] m.Wood)
            for x in #(-1.2,1.2) do for z in #(17.45,20.6) do
            (
                USAdd a (USCylinder [x,.28,z] [.13,1.45,.13] m.Wood sides:12)
                USAdd a (USCylinder [x,.89,z] [.16,.08,.16] m.Metal sides:12)
                for k=0 to 2 do USAdd a (USCylinder [x,.65+k*.06,z] [.16,.035,.16] m.Brass sides:12)
            )
        )
        "caisbox":
        (
            local blue=USPropBuild "caixazul" USMats
            for n in blue do (move n (USG [-2.65,0,9]);append a n)
            local lid=USPropBuild "caixatam" USMats
            for n in lid do (move n (USG [-2.65,.47,9.20]);append a n)
            USAdd a (USBeam [-2.65,.06,8.75] [-2.4,.06,5.5] .018 m.Blue)
            USAdd a (USBeam [-2.4,.06,5.5] [4.85,.06,5.5] .018 m.Blue)
            USAdd a (USBeam [4.85,.06,5.5] [5.22,1.45,3.6] .018 m.Blue)
        )
        "piso": (USAdd a (USBox [0,.075,-.5] [9.7,.04,10.7] m.Wood))
        "anexo":
        (
            USAdd a (USBox [0,0,-.5] [10.4,.16,11.4] m.Stone)
            for x in #(-5.0,5.0) do
            (
                USAdd a (USBox [x,.59,-.5] [.3,1.18,11] m.Paint)
                USAdd a (USBox [x,2.9,-.5] [.3,.6,11] m.Chalk)
                for pair in #([-5.175,1.65],[-.75,3.8],[3.925,2.15]) do USAdd a (USBox [x,1.9,pair.x] [.3,1.4,pair.y] m.Chalk)
                for z in #(-3.5,2.0) do
                (
                    for zz in #(z-.88,z+.88) do USAdd a (USBox [x,1.9,zz] [.40,1.6,.1] m.Metal)
                    for y in #(1.18,2.62) do USAdd a (USBox [x,y,z] [.4,.1,1.85] m.Metal)
                    USAdd a (USBox [x,1.9,z] [.35,.06,1.75] m.Metal)
                    USAdd a (USBox [x,1.1,z] [.60,.1,1.95] m.Stone)
                )
                USAdd a (USBox [x,.12,-.5] [.46,.24,11.25] m.Stone)
            )
            for z in #(-6.0,-.5,5.0) do
            (
                if z== -6 then
                (
                    USAdd a (USBox [0,1.6,z] [10,3.2,.3] m.Chalk)
                    USAdd a (USBox [0,.58,z+.17] [9.8,1.16,.025] m.Paint)
                )
                else
                (
                    for x in #(-3.0,3.0) do
                    (
                        USAdd a (USBox [x,1.6,z] [4,3.2,.3] m.Chalk)
                        USAdd a (USBox [x,.58,z+(if z>0 then -.17 else .17)] [4,1.16,.025] m.Paint)
                    )
                    USAdd a (USBox [0,2.95,z] [2,.5,.3] m.Chalk)
                    for x in #(-1.06,1.06) do USAdd a (USBox [x,1.37,z] [.14,2.75,.44] m.Wood)
                    USAdd a (USBox [0,2.76,z] [2.28,.14,.44] m.Wood)
                )
            )
            for x in #(-3.375,3.375) do USAdd a (USBox [x,3.15,-.5] [3.65,.12,11.4] m.Wood)
            USAdd a (USBox [0,3.15,1.875] [3.1,.12,6.65] m.Wood)
            USAdd a (USBox [0,3.15,-5.375] [3.1,.12,1.65] m.Wood)
            USAdd a (SRRoof())
            USAdd a (USBox [0,4.63,2.1] [.22,.15,6.6] m.Metal)
            USAdd a (USBox [0,4.63,-5.55] [.22,.15,1.5] m.Metal)
            for z in #(-6.33,5.32) do for x in #(-5.25,5.25) do
            (
                USAdd a (USBox [x,3.17,z] [.14,.15,.6] m.Metal)
                USAdd a (USCylinder [x,1.6,z] [.045,3.2,.045] m.Metal sides:12)
            )
            USAdd a (USBeam [4.72,2,3.6] [4.72,2.88,3.6] .022 m.Metal)
            USAdd a (USBeam [4.72,2.88,3.6] [.1,2.88,3.6] .022 m.Metal)
        )
        "torre":
        (
            USAdd a (USCylinder [0,3.65,-3] [2.38,.8,2.38] m.Stone sides:32)
            USAdd a (USCylinder [0,5.48,-3] [1.93,3.1,1.93] m.Chalk sides:32)
            USAdd a (USCylinder [0,5.88,-3] [1.95,.72,1.95] m.Rust sides:32)
            USAdd a (USCylinder [0,7.10,-3] [2.48,.22,2.48] m.Metal sides:32)
            for i=0 to 15 do
            (
                local t=i*360.0/16;local u=(i+1)*360.0/16
                local p=[2.28*cos t,7.62,-3+2.28*sin t];local q=[2.28*cos u,7.62,-3+2.28*sin u]
                USAdd a (USBeam (p-[0,.42,0]) (p+[0,.42,0]) .028 m.Metal)
                USAdd a (USBeam (p+[0,.35,0]) (q+[0,.35,0]) .025 m.Metal)
                USAdd a (USBeam (p-[0,.12,0]) (q-[0,.12,0]) .018 m.Metal)
            )
            USAdd a (USCylinder [0,7.35,-3] [1.62,.22,1.62] m.Metal sides:32)
            for i=0 to 7 do USAdd a (USCylinder [1.53*cos(i*45),8.1,-3+1.53*sin(i*45)] [.045,1.45,.045] m.Metal sides:12)
            for j=0 to 6 do USAdd a (USCylinder [0,7.68+j*.13,-3] [.65,.035,.65] m.Brass sides:32)
            USAdd a (USCylinder [0,8.85,-3] [1.75,.18,1.75] m.Metal sides:32)
            USAdd a (USCone [0,9.38,-3] [2.02,.94,2.02] m.Rust)
            USAdd a (USCylinder [0,10.03,-3] [.035,.55,.035] m.Metal sides:12)
        )
        "escada":
        (
            -- Uniform 20 cm risers, every step starts at the same local floor.
            for j=0 to 3 do USAdd a (USBox [0,.105+j*.10,.50-j*.33] [.88,.20+j*.20,.30] m.Stone)
            USAdd a (USBeam [-.51,.55,.75] [-.51,1.9,-.35] .035 m.Metal)
            USAdd a (USBeam [.51,.55,.75] [.30,1.58,-.08] .035 m.Metal)
            USAdd a (USBeam [-.51,1.35,.75] [-.51,2.15,-.35] .02 m.Metal)
            for z in #(.55,-.05,-.65) do USAdd a (USRot (USBox [.56,.8,z] [.04,.16,.54] m.Brass) -18 [0,1,0])
            -- Face into the room and fit between the west wall and maintenance desk.
            for n in a do n.transform=n.transform*(scaleMatrix [.8,1,1])*(rotateZMatrix 180)
            -- Keep the three warning strips on the visible, room-facing side.
            for i=8 to 10 do move a[i] [.768,0,0]
        )
        default:throw ("Unknown architecture: "+key)
    )
    a
)
