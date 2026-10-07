from pathlib import Path
import shutil,subprocess,hashlib,json,zipfile,sys
root=Path(__file__).resolve().parents[1];app=root/'BaseAppOGL';sdk=root/'OpenGL_SDK'
dest=root/'dist/O-Ultimo-Sinal-Windows-x64';dest.mkdir(parents=True,exist_ok=True)
for p in (app/'bin/Release').glob('*'):
 if p.suffix.lower() in ('.exe','.dll'):shutil.copy2(p,dest/p.name)
for folder in ['Scenes','Fonts']:
 for p in (app/folder).rglob('*'):
  if p.is_file() and p.suffix.lower() in ('.vert','.frag','.geom','.bmp','.jpg','.jpeg','.png','.tga','.obj','.mtl','.3ds','.ttf'):
   out=dest/p.relative_to(app);out.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(p,out)
licenses=root/'LICENSES';licenses.mkdir(exist_ok=True)
for name,src in {
 'Assimp.txt':sdk/'assimp/LICENSE','GLFW.md':sdk/'glfw/LICENSE.md','GLEW.txt':sdk/'glew/LICENSE.txt',
 'GLM.txt':sdk/'glm/copying.txt','FreeType.txt':sdk/'freetype/docs/FTL.TXT','FreeType-README.txt':sdk/'freetype/docs/README',
 'Dear-ImGui.txt':sdk/'imgui/LICENSE.txt','zlib.txt':sdk/'assimp/contrib/zlib/LICENSE',
}.items():
 if src.exists():shutil.copy2(src,licenses/name)
 else:print('Licenca nao localizada:',src)
shutil.copytree(licenses,dest/'LICENSES',dirs_exist_ok=True)
shutil.copy2(root/'docs/CREDITOS.md',dest/'CREDITOS.md')
(dest/'LEIA-ME.txt').write_text('''O ULTIMO SINAL — versao de desenvolvimento 07/10/2026

Abra BaseAppOGL.exe. Mantenha Scenes, Fonts e DLLs junto ao executavel.
Windows x64, driver OpenGL 4.6 e Visual C++ Runtime x64 compativel sao necessarios.
Nao ha instalacao automatica. Este pacote foi testado localmente; conferir no PC de apresentacao.

WASD: andar. Mouse: olhar. E: examinar. J: caderno. Tab: pausa/opcoes.
Enter: iniciar ou concluir apos cinco pistas. Esc: sair.
Escolha uma hipotese, duas evidencias e escreva sua justificativa antes do resultado.
R: reiniciar na pausa ou resultado. 0/1/5/6: aulas. 2: recriar jogo.

Arte provisoria. Ainda faltam modelagem autoral no 3DS Max, UVW e bake.
O encerramento do radio e textual. Leia CREDITOS.md antes de redistribuicao externa.
''',encoding='utf-8')
run=subprocess.run([str(dest/'BaseAppOGL.exe'),'--verify-game'],cwd=Path(__file__).parent,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=90)
(root/'logs/package_runtime.log').write_bytes(run.stdout)
text=run.stdout.decode('utf-8',errors='replace')
if run.returncode or 'ERROR' in text or 'FAIL' in text:raise RuntimeError(text)
print('Pacote: percurso e aulas passaram com pasta de trabalho externa.')
missing=dest/'Scenes/Skybox/right.bmp';held=dest/'Scenes/Skybox/right.bmp.verificacao'
assert not held.exists()
missing.rename(held)
try:
 negative=subprocess.run([str(dest/'BaseAppOGL.exe'),'--verify-game'],cwd=Path(__file__).parent,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=20)
 (root/'logs/package_missing_resource.log').write_bytes(negative.stdout)
 assert negative.returncode==2 and b'Recurso ausente' in negative.stdout,negative.stdout
 print('Recurso ausente: diagnostico correto e codigo 2.')
finally:held.rename(missing)
manifest={str(p.relative_to(dest)):hashlib.sha256(p.read_bytes()).hexdigest() for p in dest.rglob('*') if p.is_file() and p.name not in ['manifest.json','imgui.ini','OpenGL_Info.txt']}
(dest/'manifest.json').write_text(json.dumps(manifest,indent=2),encoding='utf-8')
archive=root/'dist/O-Ultimo-Sinal-Windows-x64.zip'
with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED) as out:
 for p in dest.rglob('*'):
  if p.is_file() and p.name not in ['imgui.ini','OpenGL_Info.txt']:out.write(p,p.relative_to(dest.parent))
print('ZIP:',archive,'bytes=',archive.stat().st_size)
