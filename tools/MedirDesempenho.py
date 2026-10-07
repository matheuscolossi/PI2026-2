from pathlib import Path
import subprocess,hashlib,json,shutil
root=Path(__file__).resolve().parents[1];app=root/'BaseAppOGL';exe=app/'bin/Release/BaseAppOGL.exe'
data={'executable_sha256':hashlib.sha256(exe.read_bytes()).hexdigest(),'runs':[]}
for i in range(1,4):
 print(f'Iniciando medicao {i}/3: 30s aquecimento + 120s coleta.',flush=True)
 result=subprocess.run([str(exe),'--benchmark'],cwd=root/'tests',stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=210)
 text=result.stdout.decode('utf-8',errors='replace');(root/'logs'/f'benchmark_{i}.log').write_bytes(result.stdout)
 if result.returncode:raise RuntimeError(text)
 shutil.copy2(app/'benchmark_frames.csv',root/'logs'/f'benchmark_{i}_frames.csv')
 summary=[line for line in text.splitlines() if line.startswith(('BENCHMARK','GPU='))]
 data['runs'].append(summary);(root/'logs/benchmark_summary.json').write_text(json.dumps(data,ensure_ascii=False,indent=2),encoding='utf-8');print('\n'.join(summary),flush=True)
