from pathlib import Path
from datetime import datetime,timezone
import shutil,subprocess,hashlib,json,zipfile,sys,re
root=Path(__file__).resolve().parents[1];app=root/'BaseAppOGL';sdk=root/'OpenGL_SDK'
dist=root/'dist';dist.mkdir(exist_ok=True)
stamp=datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%S%fZ')
final=dist/'O-Ultimo-Sinal-Investigacao-v3'
dest=dist/(final.name+'.staging-'+stamp)
logs=root/'logs';logs.mkdir(exist_ok=True)
def sha(path):return hashlib.sha256(path.read_bytes()).hexdigest()
def included(path):return not any(part.casefold()=='evidencias' for part in path.parts)
def checked_dist(path):
 resolved=path.resolve()
 if not resolved.is_relative_to(dist.resolve()) or resolved==dist.resolve():raise RuntimeError('Destino fora de dist: '+str(path))
 return path
cenario_names=('ilha','costa','costalod','caminho','cais','carga','caisbox','anexo','piso','torre','porta','cadeira','caneca','armario','armporta','botas','escada','radio','quadro','botao','mapa','papel','relogio','arquivo','lumin','lampada','diario','alicate','marcas','chave','foto','agua','janvid','vidfar','led','fragment','radioluz','escrivan','decor')
cenario=app/'Scenes/Models/Cenario'
for name in cenario_names:
 required=cenario/(name+'.3ds')
 if not required.is_file() or required.stat().st_size==0:raise RuntimeError('Producao Max ainda incompleta; falta '+str(required))
source_assets={p.relative_to(app).as_posix():p for p in cenario.rglob('*') if p.is_file() and included(p.relative_to(cenario)) and p.suffix.lower() in ('.3ds','.jpg','.jpeg')}
for name in ('mesa.3ds','wood.jpg'):source_assets['Scenes/Models/Mesa/'+name]=app/'Scenes/Models/Mesa'/name
for name,source in source_assets.items():
 if not source.is_file() or source.stat().st_size==0:raise RuntimeError('Recurso obrigatorio ausente: '+str(source))
asset_hashes={name:sha(source) for name,source in source_assets.items()}
model_hashes={name:asset_hashes['Scenes/Models/Mesa/'+name] for name in ('mesa.3ds','wood.jpg')}
bake_file=cenario/'evidencias/bake_verificacao.json'
if not bake_file.is_file():raise RuntimeError('Evidencia do bake nativo do piso ausente.')
bake=json.loads(bake_file.read_text(encoding='utf-8-sig'))
if bake.get('element')!='CompleteMap' or bake.get('uv_channel')!=1 or bake.get('size')!=[1024,1024]:raise RuntimeError('Formato de bake do piso nao corresponde ao verificado.')
if sha(cenario/'piso.3ds')!=bake.get('floor_sha256') or sha(cenario/'p_bake.jpg')!=bake.get('bitmap_sha256'):raise RuntimeError('Piso ou CompleteMap mudou desde a verificacao nativa do bake.')
if not (app/'bin/InvestigacaoRelease/BaseAppOGL.exe').is_file():raise RuntimeError('Compile Release antes de empacotar.')
source_exe_hash=sha(app/'bin/InvestigacaoRelease/BaseAppOGL.exe')
dest.mkdir()
for p in (app/'bin/InvestigacaoRelease').glob('*'):
 if p.suffix.lower() in ('.exe','.dll'):shutil.copy2(p,dest/p.name)
for folder in ['Scenes','Fonts']:
 for p in (app/folder).rglob('*'):
  if p.is_file() and included(p.relative_to(app)) and p.suffix.lower() in ('.vert','.frag','.geom','.bmp','.jpg','.jpeg','.png','.tga','.obj','.mtl','.3ds','.ttf'):
   out=dest/p.relative_to(app);out.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(p,out)
for name,expected in asset_hashes.items():
 copied=dest/name
 if not copied.is_file() or sha(copied)!=expected:raise RuntimeError('Modelo/textura do pacote difere da origem: '+name)
if sha(dest/'BaseAppOGL.exe')!=source_exe_hash:raise RuntimeError('Executavel distribuido difere do Release atual.')
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
readme_text='''O ULTIMO SINAL — investigacao v3 — conjunto de modelos do Max

Abra BaseAppOGL.exe. Mantenha Scenes, Fonts e DLLs junto ao executavel.
Windows x64, driver OpenGL 4.6 e Visual C++ Runtime x64 compativel sao necessarios.
Nao ha instalacao automatica. Este pacote foi testado localmente; conferir no PC de apresentacao.

WASD: andar. Mouse: olhar. E: examinar. J: caderno. Tab/Esc: pausa/voltar. F1: ajuda sob pedido. F2: opcoes.
Enter: iniciar ou apresentar relatorio apos fundamentar a investigacao. Sair: menu de pausa.
Escolha uma hipotese, duas evidencias e escreva sua justificativa antes do resultado.
Nova investigacao: menu, com confirmacao. Ctrl+0/1/5/6: aulas. Ctrl+2: recriar cena.

Este pacote integra 39 arquivos do cenario produzidos no 3ds Max, mais mesa.3ds reutilizada em quatro mesas; a bancada do radio usa escrivan.3ds.
Terreno, arquitetura, mobiliario, pistas, agua e vidro usam geometria importada com UVs.
Os bitmaps de variacao foram renderizados por mapas Noise nativos do Max; wood.jpg e photo.jpg foram geradas com a ferramenta de imagens integrada; P1.jpg da aula foi preservada.
A producao usou MAXScript assistido por Codex executado no Max; nao representa modelagem manual integral do aluno.
O piso usa CompleteMap 1024x1024, UV1, produzido por Render To Texture/Scanline no Max com sombras estaticas.
Somente o piso recebeu esse bake. Nele nao ha nova iluminacao Point/Spot em tempo de execucao; os outros objetos continuam com iluminacao e sombras analiticas do jogo.
O bake foi comparado com sombras ligadas/desligadas. Isso nao significa que todos os objetos receberam bake nem que a entrega academica esteja concluida.
Acabamento, testes humanos, piloto e desempenho no computador de apresentacao continuam pendentes.
O conjunto editavel, scripts, texturas e evidencias sao entregues separadamente em Cenario-3dsMax-Fontes.zip.
Tres areas fisicas conectadas: sala de servico, arquivo e sala optica. A escada em espiral tem 36 degraus transitaveis.
A investigacao inclui energia, radio e ensaio local da lente no alto da torre. O radio tem sintonia sintetizada e transcricao; o ensaio local tem tres pulsos longos, sem nova transmissao para o mar. Diario, alicate, chave e foto oferecem Ver objeto, com rotacao/inclinacao/zoom. O caderno agrupa evidencias e horarios conhecidos. Nao ha dublagem. Progresso/opcoes: pasta UltimoSinal em LOCALAPPDATA. Confirme antes de substituir uma investigacao salva. Leia CREDITOS.md antes de redistribuicao externa.
'''
run=subprocess.run([str(dest/'BaseAppOGL.exe'),'--verify-game'],cwd=Path(__file__).parent,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=90)
(logs/'mesa_package_runtime.log').write_bytes(run.stdout)
text=run.stdout.decode('utf-8',errors='replace')
if run.returncode or 'ERROR' in text or 'FAIL' in text:raise RuntimeError(text)
mesa_lines=[line for line in text.splitlines() if 'Mesa 3DS:' in line]
if not mesa_lines or not all(re.search(r'\baccepted=1\b',line) for line in mesa_lines):raise RuntimeError('O runtime nao confirmou a mesa CAsset: '+str(mesa_lines))
instance_lines=[line for line in text.splitlines() if 'Mobiliario Max aplicado:' in line]
if not instance_lines or not all(re.search(r'\bmesa_instances=4 escrivan_instances=1\b',line) for line in instance_lines):raise RuntimeError('O runtime nao confirmou as quatro mesas e a escrivaninha: '+str(instance_lines))
runtime=re.search(r'Runtime: (\d+) checks; failures=0',text)
if not runtime:raise RuntimeError('O percurso executavel nao confirmou failures=0.')
accepted=set(re.findall(r'Max world asset accepted: ([a-z0-9_]+) triangles=\d+',text))
if accepted!=set(cenario_names) or 'Max world asset rejected' in text:raise RuntimeError('Conjunto Max aceito pelo runtime difere dos 39 esperados: '+str(sorted(accepted)))
libraries=re.findall(r'Max world library: (\d+)/39 assets',text)
if not libraries or any(int(count)!=39 for count in libraries):raise RuntimeError('Biblioteca Max incompleta no executavel distribuido.')
coverage=re.findall(r'Max native coverage: accepted=(\d+) fallback=(\d+)',text)
if not coverage or any(int(count)!=39 or int(fallback)!=0 for count,fallback in coverage):raise RuntimeError('O runtime nao confirmou cobertura nativa accepted=39 fallback=0.')
bake_lines=re.findall(r'Max floor bake: CompleteMap UV1 applied=(\d+)',text)
if not bake_lines or any(int(applied)!=1 for applied in bake_lines):raise RuntimeError('O executavel distribuido nao confirmou o CompleteMap do piso.')
for scene in (0,1,5,6,2):
 if f'Scene {scene} load GL error=0' not in text or f'Scene {scene} rendered; GL error=0' not in text:raise RuntimeError('Regressao grafica nao confirmada na cena '+str(scene))
print('Pacote: percurso e aulas passaram com pasta de trabalho externa.')
missing=dest/'Scenes/Skybox/right.bmp';held=dest/'Scenes/Skybox/right.bmp.verificacao'
assert not held.exists()
missing.rename(held)
try:
 negative=subprocess.run([str(dest/'BaseAppOGL.exe'),'--verify-game'],cwd=Path(__file__).parent,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=20)
 (logs/'mesa_package_missing_resource.log').write_bytes(negative.stdout)
 assert negative.returncode==2 and b'Recurso ausente' in negative.stdout,negative.stdout
 print('Recurso ausente: diagnostico correto e codigo 2.')
finally:held.rename(missing)
for name,expected in asset_hashes.items():
 if sha(source_assets[name])!=expected or sha(dest/name)!=expected:raise RuntimeError('Arte alterada durante verificacao: '+name)
if sha(app/'bin/InvestigacaoRelease/BaseAppOGL.exe')!=source_exe_hash or sha(dest/'BaseAppOGL.exe')!=source_exe_hash:raise RuntimeError('Release mudou durante a verificacao; empacote novamente.')
(dest/'LEIA-ME.txt').write_text(readme_text,encoding='utf-8')
if any(p.is_dir() and p.name.casefold()=='evidencias' for p in dest.rglob('*')):raise RuntimeError('Pasta de evidencias indevida no runtime novo.')
if any(p.is_file() and p.suffix.lower() in ('.max','.ms') for p in dest.rglob('*')):raise RuntimeError('Fontes de autoria indevidos no runtime; use o pacote separado de fontes.')
manifest={str(p.relative_to(dest)):sha(p) for p in dest.rglob('*') if p.is_file() and included(p.relative_to(dest)) and p.name not in ['manifest.json','imgui.ini','OpenGL_Info.txt']}
(dest/'manifest.json').write_text(json.dumps(manifest,indent=2),encoding='utf-8')
archive=root/'dist/O-Ultimo-Sinal-Investigacao-v3.zip'
backups=dist/'backups';backups.mkdir(exist_ok=True)
backup_zip=None;backup_runtime=None
if archive.exists():
 backup_zip=checked_dist(backups/(archive.stem+'-'+stamp+'.zip'))
 shutil.copy2(archive,backup_zip)
 if sha(archive)!=sha(backup_zip):raise RuntimeError('Backup do ZIP anterior nao confere.')
candidate=checked_dist(dist/(archive.stem+'.staging-'+stamp+'.zip'))
with zipfile.ZipFile(candidate,'w',zipfile.ZIP_DEFLATED) as out:
 for p in dest.rglob('*'):
  if p.is_file() and included(p.relative_to(dest)) and p.name not in ['imgui.ini','OpenGL_Info.txt']:out.write(p,Path(final.name)/p.relative_to(dest))
with zipfile.ZipFile(candidate) as packed:
 if packed.testzip() is not None:raise RuntimeError('ZIP novo corrompido.')
 if any(not included(Path(name)) for name in packed.namelist()):raise RuntimeError('ZIP contem pasta evidencias.')
 for name,expected in asset_hashes.items():
  data=packed.read(final.name+'/'+name)
  if hashlib.sha256(data).hexdigest()!=expected:raise RuntimeError('Modelo/textura no ZIP nao confere: '+name)
if final.exists():
 backup_runtime=checked_dist(backups/(final.name+'-'+stamp))
 checked_dist(final).rename(backup_runtime)
try:checked_dist(dest).rename(checked_dist(final))
except Exception:
 if backup_runtime is not None and not final.exists():backup_runtime.rename(final)
 raise
candidate.replace(archive)
summary={'model_sha256':model_hashes,'native_asset_sha256':asset_hashes,'release_sha256':source_exe_hash,'casset_accepted':True,'table_instances':4,'writing_desk_instances':1,'native_models':39,'total_model_files':40,'native_fallbacks':0,'floor_complete_map_applied':True,'static_bake_scope':'floor only; other meshes keep analytic shadows','floor_bake_verification':bake,'runtime_checks':int(runtime.group(1)),'runtime_failures':0,'evidence_folders_excluded':True,'authoring_files_excluded':True,'zip_sha256':sha(archive),'zip_bytes':archive.stat().st_size,'previous_zip_backup':str(backup_zip) if backup_zip else None,'previous_runtime_backup':str(backup_runtime) if backup_runtime else None,'runtime_log':'mesa_package_runtime.log','missing_resource_log':'mesa_package_missing_resource.log'}
(logs/'mesa_package_verificacao.json').write_text(json.dumps(summary,indent=2),encoding='utf-8')
print('ZIP:',archive,'bytes=',archive.stat().st_size)
print('40 modelos + bitmaps: hashes identicos na origem, runtime e ZIP; Cenario39/39, fallback=0, Mesa4 e uma escrivaninha. Fontes/evidencias excluidos do runtime.')
