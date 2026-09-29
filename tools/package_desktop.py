#!/usr/bin/env python3
"""Create a compressed, ready-to-extract Windows game archive."""

import argparse
import hashlib
import os
import tempfile
import zipfile
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
GAME_NAME = 'Frogger Remake'
EXTENSION_NAME = 'libfrogger_arcade.windows.template_release.x86_64.dll'


def digest(stream):
    checksum = hashlib.sha256()
    for chunk in iter(lambda: stream.read(1024 * 1024), b''):
        checksum.update(chunk)
    return checksum.digest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--exe', type=Path, required=True, help='Fresh Godot release export')
    parser.add_argument('--output', type=Path, default=Path('builds/windows'))
    args = parser.parse_args()

    output = (ROOT / args.output).resolve()
    executable = (ROOT / args.exe).resolve()
    if executable.parent != output or not executable.is_file():
        parser.error('The exported executable must exist directly in the output directory.')

    files = {
        f'{GAME_NAME}.exe': executable,
        EXTENSION_NAME: output / EXTENSION_NAME,
    }
    license_dir = output / 'licenses'
    files.update({f'licenses/{name}': license_dir / name for name in ('LICENSE', 'THIRD_PARTY.md')})
    files.update({f'licenses/{font.name}': font for font in sorted(license_dir.glob('*-OFL.txt'))})
    if not list(license_dir.glob('*-OFL.txt')):
        raise RuntimeError('Font license files are missing from the export.')
    for name, path in files.items():
        if not path.is_file():
            raise RuntimeError(f'Missing desktop export file: {name} ({path})')

    archive_path = output / f'{GAME_NAME}.zip'
    raw_size = sum(path.stat().st_size for path in files.values())
    with tempfile.NamedTemporaryFile(prefix='.frogger-desktop-', suffix='.zip', dir=output, delete=False) as temporary:
        temporary_path = Path(temporary.name)
    try:
        with zipfile.ZipFile(temporary_path, 'w', compression=zipfile.ZIP_DEFLATED, compresslevel=9, allowZip64=True) as archive:
            for name, path in files.items():
                archive.write(path, name)
        with zipfile.ZipFile(temporary_path) as archive:
            if set(archive.namelist()) != set(files) or archive.testzip() is not None:
                raise RuntimeError('Desktop archive failed integrity validation.')
            for name, path in files.items():
                with path.open('rb') as source, archive.open(name) as packed:
                    if digest(source) != digest(packed):
                        raise RuntimeError(f'Desktop archive does not match {name}.')
        os.replace(temporary_path, archive_path)
    finally:
        temporary_path.unlink(missing_ok=True)

    archive_size = archive_path.stat().st_size
    print(f'Compressed desktop build: {raw_size:,} -> {archive_size:,} bytes ({archive_size / raw_size:.1%})')
    print(f'Archive: {archive_path}')


if __name__ == '__main__':
    main()
