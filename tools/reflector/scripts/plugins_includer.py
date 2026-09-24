#!/usr/bin/env python3
"""
Made by Cloude AI
Regenerates reflector_plugins.inc by scanning for generator plugin
headers. This used to be a separate compiled native executable
(plugins_includer.cpp); it's now a plain script run by meson as a
custom_target, so the project only ever compiles one binary
(reflect_tool) instead of two.
"""
import os
import sys


def collect(root: str, subdir: str, suffix: str) -> list[str]:
    d = os.path.join(root, subdir)
    if not os.path.isdir(d):
        return []
    out = []
    for dirpath, _dirnames, filenames in os.walk(d):
        for fn in filenames:
            if fn.endswith(suffix):
                out.append(os.path.join(dirpath, fn))
    return out


def main() -> int:
    if len(sys.argv) < 3:
        print("usage: plugins_includer.py <source_root> <output_file>", file=sys.stderr)
        return 1

    source_root, output_file = sys.argv[1], sys.argv[2]

    matches = collect(source_root, "scene2", "_plugin.h")
    matches += collect(source_root, os.path.join("tools", "reflector", "plugins"), ".h")

    rel = sorted({os.path.relpath(m, source_root).replace(os.sep, "/") for m in matches})

    lines = [
        "// Auto-generated -- do not edit.",
        "// Regenerated on every build; add a new plugin file and it",
        "// will be picked up automatically, no manual #include needed.",
        "",
    ]
    lines += [f'#include "{path}"' for path in rel]
    content = "\n".join(lines) + "\n"

    # Avoid rewriting (and busting downstream compile caches) if unchanged.
    if os.path.exists(output_file):
        with open(output_file, "r", encoding="utf-8") as f:
            if f.read() == content:
                return 0

    os.makedirs(os.path.dirname(output_file) or ".", exist_ok=True)
    with open(output_file, "w", encoding="utf-8") as f:
        f.write(content)
    return 0


if __name__ == "__main__":
    sys.exit(main())
