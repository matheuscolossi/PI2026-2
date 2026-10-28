"""Archive the real, completed Max sources separately from the playable build.

This script does not run Max, synthesize models, or change authoring sources.
Run only after native production/conference of all 40 exports has finished.
"""
from pathlib import Path
from datetime import datetime, timezone
import argparse
import hashlib
import json
import shutil
import zipfile


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def collect_sources(root, inventory):
    models = root / 'BaseAppOGL/Scenes/Models'
    cenario = models / 'Cenario'
    mesa = models / 'Mesa'
    contract = json.loads(inventory.read_text(encoding='utf-8-sig'))
    assets = contract['assets']
    if len(assets) != 40 or len({asset['id'] for asset in assets}) != 40:
        raise RuntimeError('O contrato deve declarar 40 modelos distintos.')
    for asset in assets:
        path = (cenario / asset['file']).resolve()
        if not path.is_relative_to(models.resolve()) or path.suffix.lower() != '.3ds':
            raise RuntimeError('Caminho de modelo fora da producao: ' + str(path))
        if not path.is_file() or path.stat().st_size == 0:
            raise RuntimeError('Producao ainda incompleta: ' + str(path))
    for path in (cenario / 'cenario.max', mesa / 'mesa.max'):
        if not path.is_file() or path.stat().st_size == 0:
            raise RuntimeError('Fonte editavel obrigatorio ausente: ' + str(path))
    for folder in (cenario, mesa):
        if not any(path.is_file() for path in folder.glob('*.ms')):
            raise RuntimeError('Scripts da producao ausentes: ' + str(folder))
        if not any(path.is_file() and path.stat().st_size for path in (folder / 'evidencias').rglob('*')):
            raise RuntimeError('Evidencias de producao/conferencia ausentes: ' + str(folder))
    files = {}
    for folder, master in ((cenario, 'cenario.max'), (mesa, 'mesa.max')):
        for path in folder.rglob('*'):
            if not path.is_file():
                continue
            relative = path.relative_to(folder)
            evidence = any(part.casefold() == 'evidencias' for part in relative.parts)
            if evidence or path.name == master or path.suffix.lower() in ('.ms', '.3ds', '.jpg', '.jpeg', '.png', '.bmp', '.txt', '.json'):
                files[(Path(folder.name) / relative).as_posix()] = path
    files['Contrato/world_max_inventory_2026-10-22.json'] = inventory
    files['CREDITOS.md'] = root / 'docs/CREDITOS.md'
    if not files['CREDITOS.md'].is_file():
        raise RuntimeError('CREDITOS.md ausente.')
    return files


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root', type=Path, default=Path(r'C:\Users\mlc17\Desktop\O Último Sinal'))
    parser.add_argument('--inventory', type=Path, default=Path(__file__).resolve().parents[1] / 'BaseAppOGL/Scenes/Models/Cenario/evidencias/world_max_inventory_2026-10-22.json')
    args = parser.parse_args()
    root = args.root.resolve()
    files = collect_sources(root, args.inventory.resolve())
    manifest = {name: sha(path) for name, path in files.items()}
    stamp = datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%S%fZ')
    dist = root / 'dist'
    dist.mkdir(exist_ok=True)
    archive = dist / 'Cenario-3dsMax-Fontes.zip'
    candidate = dist / ('Cenario-3dsMax-Fontes.staging-' + stamp + '.zip')
    backup = None
    if archive.exists():
        backups = dist / 'backups'
        backups.mkdir(exist_ok=True)
        backup = backups / ('Cenario-3dsMax-Fontes-' + stamp + '.zip')
        if not backup.resolve().is_relative_to(dist):
            raise RuntimeError('Destino de backup fora de dist.')
        shutil.copy2(archive, backup)
        if sha(archive) != sha(backup):
            raise RuntimeError('Backup anterior nao confere.')
    prefix = 'Cenario-3dsMax-Fontes/'
    readme = '''O ULTIMO SINAL — fontes de producao no 3ds Max

Cenario/cenario.max: cena mestre editavel do conjunto.
Mesa/mesa.max: mesa original, preservada separadamente.
39 modelos do Cenario + mesa.3ds: exportacoes usadas pelo jogo.
Pastas evidencias: registros nativos, conferencias e imagens disponiveis.
Scripts .ms: producao assistida por Codex, executada no 3ds Max 2027.
Nao representa modelagem manual integral do aluno.

Os mapas Noise foram renderizados em bitmaps pelo proprio Max.
wood.jpg e photo.jpg foram geradas pela ferramenta de imagens integrada.
Prompts e registros da producao assistida em Cenario/evidencias/spatial_2026-10-22.
P1.jpg da aula permanece preservada no jogo e nos backups anteriores.
Converter Noise em bitmap NAO e bake de iluminacao/sombras.
O piso possui CompleteMap nativo Scanline/Render To Texture 1024x1024, UV1,
com verificacao shadows on/off em Cenario/evidencias/bake_verificacao.json.
Somente o piso recebeu bake. Os demais objetos mantem sombras analiticas
no jogo. No piso baked nao ha relighting Point/Spot durante a execucao.

Contrato: inventario de coordenadas, funcoes e nomes dos 40 modelos.
manifest.json: SHA-256 de cada fonte preservado neste arquivo.
Leia CREDITOS.md para distinguir producao do projeto e material da aula.
O pacote jogavel com executavel e DLLs e entregue separadamente.
'''
    with zipfile.ZipFile(candidate, 'w', zipfile.ZIP_DEFLATED) as out:
        for name, path in sorted(files.items()):
            out.write(path, prefix + name)
        out.writestr(prefix + 'manifest.json', json.dumps(manifest, indent=2))
        out.writestr(prefix + 'LEIA-ME-FONTES.txt', readme)
    with zipfile.ZipFile(candidate) as packed:
        if packed.testzip() is not None:
            raise RuntimeError('ZIP de fontes corrompido.')
        for name, path in files.items():
            if sha(path) != manifest[name] or hashlib.sha256(packed.read(prefix + name)).hexdigest() != manifest[name]:
                raise RuntimeError('Fonte alterado durante a copia: ' + name)
    candidate.replace(archive)
    summary = {'model_files': 40, 'master_files': ['Cenario/cenario.max', 'Mesa/mesa.max'], 'file_sha256': manifest,
               'zip_sha256': sha(archive), 'zip_bytes': archive.stat().st_size, 'previous_zip_backup': str(backup) if backup else None,
               'static_bake_scope': 'floor only; see Cenario/evidencias/bake_verificacao.json', 'all_assets_baked': False,
               'production': 'Native 3ds Max with disclosed MAXScript/Codex assistance'}
    (root / 'logs').mkdir(exist_ok=True)
    (root / 'logs/cenario_fontes_package_verificacao.json').write_text(json.dumps(summary, indent=2), encoding='utf-8')
    print('Fontes:', archive, 'bytes=', archive.stat().st_size)


if __name__ == '__main__':
    main()
