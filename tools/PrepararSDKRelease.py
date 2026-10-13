from pathlib import Path
import subprocess,os,sys
root=Path(__file__).resolve().parents[1];sdk=root/'OpenGL_SDK';build=sdk/'release-build';prefix=sdk/'release';build.mkdir(exist_ok=True)
cm=r'C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
(root/'logs').mkdir(exist_ok=True)
env={k.upper():v for k,v in os.environ.items()}
projects=[
 ('glfw',sdk/'glfw',['-DGLFW_BUILD_EXAMPLES=OFF','-DGLFW_BUILD_TESTS=OFF','-DGLFW_BUILD_DOCS=OFF','-DBUILD_SHARED_LIBS=OFF']),
 ('glew',sdk/'glew/build/cmake',['-DBUILD_SHARED_LIBS=ON','-DBUILD_UTILS=OFF']),
 ('freetype',sdk/'freetype',['-DBUILD_SHARED_LIBS=ON','-DFT_DISABLE_ZLIB=ON','-DFT_DISABLE_BZIP2=ON','-DFT_DISABLE_PNG=ON','-DFT_DISABLE_HARFBUZZ=ON','-DFT_DISABLE_BROTLI=ON']),
 ('assimp',sdk/'assimp',['-DBUILD_SHARED_LIBS=ON','-DASSIMP_BUILD_TESTS=OFF','-DASSIMP_BUILD_ASSIMP_TOOLS=OFF','-DASSIMP_BUILD_ZLIB=ON','-DASSIMP_WARNINGS_AS_ERRORS=OFF','-DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT=OFF','-DASSIMP_BUILD_3DS_IMPORTER=ON','-DASSIMP_BUILD_OBJ_IMPORTER=ON','-DASSIMP_BUILD_FBX_IMPORTER=ON','-DASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT=OFF'])]
for name,source,options in projects:
 log=root/'logs'/f'release-sdk-{name}.log'
 with log.open('wb') as out:
  for args in [[cm,'-S',str(source),'-B',str(build/name),'-G','Visual Studio 18 2026','-A','x64','-DCMAKE_INSTALL_PREFIX='+str(prefix),'-DCMAKE_POLICY_VERSION_MINIMUM=3.5']+options,[cm,'--build',str(build/name),'--config','Release','--parallel','4'],[cm,'--install',str(build/name),'--config','Release']]:
   r=subprocess.run(args,env=env,stdout=out,stderr=subprocess.STDOUT)
   if r.returncode:
    print('FAIL',name,log,flush=True);print(log.read_text(errors='replace')[-4000:],flush=True);sys.exit(r.returncode)
 print('OK',name,flush=True)
