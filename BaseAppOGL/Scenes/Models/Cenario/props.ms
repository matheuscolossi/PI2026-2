global USPending, USStageKey, USPropBuild
fileIn (USFolder+"PropsHelpers.ms")
fn USPropPlace root p = (move root (USG p))
fn USPropsStage =
(
    local jobs=#(
        #("carga",[-3.7,.35,12.3]),#("porta",[-.97,1.35,-.5]),
        #("cadeira",[4.3,.095,2]),#("caneca",[3.52,1.186,3.08]),
        #("armario",[-3.7,0,4.15]),#("armporta",[-4.32,1.05,3.55]),
        #("botas",[-1.685,.095,4.3]),#("radio",[3,1.29,2]),
        #("quadro",[4.67,1.55,3.6]),#("botao",[4.50,1.4,3.5]),
        #("mapa",[1.7,1.53,4.78]),#("papel",[-2.5,1.106,1]),
        #("relogio",[-2.3,2.27,-5.73]),#("arquivo",[4.6,.095,-4.65]),
        #("lumin",[0,3.14,2.5]),#("lampada",[0,2.73,2.5]),
        #("diario",[-3,1.151,3]),#("alicate",[-3,1.1235,1]),
        #("marcas",[-2,.035,9]),#("chave",[-3.7,1.25,3.8]),
        #("foto",[3,1.102,-3]),#("agua",[0,-.4,0]),
        #("janvid",[-5,1.9,-3.5]),#("vidfar",[0,8.1,-3]),
        #("led",[-3.27,1.45,3.51]),#("fragment",[-2.76,1.1135,.88]),
        #("radioluz",[2.83,1.357,2.216])
    )
    for job in jobs do
    (
        local key=job[1]
        if getNodeByName ("US_"+key)==undefined do
        (
            USStageKey=key
            USPending=USPropBuild key USMats
            local root=USFinish key USPending
            case key of
            (
                "carga":(scale root [1.5,1.15,.7])
                "botao":(scale root [.04,.20,.08])
                "lampada":(scale root [.16,.16,.04])
                "agua":(scale root [220,220,1])
                "janvid":(scale root [1.38,1.68,1];rotate root (angleaxis 90 [0,-1,0]))
                "vidfar":(scale root [1.52,1.52,1.4])
                "led":(scale root [.07,.025,.07])
            )
            USPropPlace root job[2]
            saveMaxFile (USFolder+"cenario.max") quiet:true
        )
    )
)
try (with redraw off (USPropsStage());USNote "Props stage complete";messageBox "Prop exports complete." title:"O Ultimo Sinal")
catch (USNote ("ERROR prop "+USStageKey+": "+getCurrentException());format "%\n" (getCurrentExceptionStackTrace());messageBox (getCurrentException()) title:"Props stopped safely")
