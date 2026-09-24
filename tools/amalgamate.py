#!/usr/bin/env python3
"""Regroupe un fichier source et ses en-têtes locaux en un seul fichier.

- Les includes avec guillemets ("...") sont remplacés par le contenu du fichier,
  une seule fois chacun (équivalent de #pragma once / include guards).
- Les includes système (<...>) sont conservés tels quels.

Usage : python3 amalgamate.py src/main.cpp -I src -o ncorps_single.cpp
"""
import argparse
import re
from pathlib import Path

INCLUDE_RE = re.compile(r'^\s*#\s*include\s*"([^"]+)"')
PRAGMA_ONCE_RE = re.compile(r"^\s*#\s*pragma\s+once\s*$")


def resolve(name, current_dir, include_dirs):
    # Même ordre que le compilateur : d'abord le dossier du fichier courant,
    # puis les dossiers -I dans l'ordre donné.
    for d in [current_dir, *include_dirs]:
        candidate = (d / name).resolve()
        if candidate.is_file():
            return candidate
    return None


def expand(path, include_dirs, seen, out):
    path = path.resolve()
    if path in seen:
        return
    seen.add(path)
    out.append(f"// ===== début : {path.name} =====\n")
    for line in path.read_text().splitlines(keepends=True):
        if PRAGMA_ONCE_RE.match(line):
            continue
        m = INCLUDE_RE.match(line)
        if m:
            target = resolve(m.group(1), path.parent, include_dirs)
            if target is None:
                out.append(line)  # introuvable : on laisse l'include tel quel
            else:
                expand(target, include_dirs, seen, out)
            continue
        out.append(line)
    if not out[-1].endswith("\n"):
        out.append("\n")
    out.append(f"// ===== fin : {path.name} =====\n")


def main():
    p = argparse.ArgumentParser()
    p.add_argument("source")
    p.add_argument("-I", dest="include_dirs", action="append", default=[])
    p.add_argument("-o", dest="output", required=True)
    args = p.parse_args()

    out = []
    expand(Path(args.source), [Path(d) for d in args.include_dirs], set(), out)
    Path(args.output).write_text("".join(out))


if __name__ == "__main__":
    main()
