from pathlib import Path

HEADER = """/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

"""

EXTENSIONS = {".h", ".hpp", ".cpp", ".cxx"}

EXCLUDED_DIRS = {
    "ThirdParty",
    "thirdparty"
    "build",
    ".git",
    ".vs"
}

for path in Path(".").rglob("*"):

    if not path.is_file():
        continue

    if path.suffix not in EXTENSIONS:
        continue

    if any(part in EXCLUDED_DIRS for part in path.parts):
        continue

    try:
        text = path.read_text(encoding="utf-8")
    except UnicodeDecodeError:
        print(f"[SKIP ENCODING] {path}")
        continue
    except Exception as e:
        print(f"[ERROR READ] {path}: {e}")
        continue

    if "Licensed under the MIT License" in text:
#        print(f"[SKIP HEADER] {path}")
        continue

    try:
        path.write_text(HEADER + text, encoding="utf-8")
        print(f"[UPDATED] {path}")
    except Exception as e:
        print(f"[ERROR WRITE] {path}: {e}")