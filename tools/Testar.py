from pathlib import Path
import os,subprocess,sys
root=Path(__file__).resolve().parents[1]
env={k.upper():v for k,v in os.environ.items()}
vswhere=Path(env['PROGRAMFILES(X86)'])/'Microsoft Visual Studio/Installer/vswhere.exe'
found=subprocess.check_output([str(vswhere),'-latest','-products','*','-requires','Microsoft.Component.MSBuild','-find',r'MSBuild\**\Bin\MSBuild.exe'],env=env,text=True).splitlines()
if not found:raise SystemExit('MSBuild do Visual Studio não encontrado.')
for name in ['Testes','ExperienceTests','AudioPcmTests','SpatialTests']:
 command=[found[0],str(root/'tests'/(name+'.vcxproj')),'/t:Build','/p:Configuration=Debug','/p:Platform=x64','/nologo','/v:minimal','/p:GameSources='+str(root/'BaseAppOGL/Scenes/UltimoSinal')]
 subprocess.run(command,cwd=root,env=env,check=True)
 subprocess.run([str(root/'tests/bin'/(name+'.exe'))],cwd=root,env=env,check=True)
print('Lógica, apresentação, áudio e suporte espacial aprovados.')
