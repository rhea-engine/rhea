#!/usr/bin/env python3
"""
Type Registry Scanner for Rhea Engine
Scans C++ header files for RECLASS macros and generates type registry cache.

Usage:
    python scan_types.py <project_root> [output_json]

Example:
    python scan_types.py C:/Users/baume/source/repos/Meworla .rhea/cache/type_registry.json
"""

import re
import json
import os
import sys
from pathlib import Path
from typing import List, Dict, Optional
from dataclasses import dataclass, asdict

@dataclass
class TypeInfo:
    typeId: str
    cppClass: str
    baseClass: str
    headerPath: str
    isVirtual: bool = False
class TypeScanner:
    """Scans header files for RECLASS and MECLASS macro definitions"""

    # RECLASS with explicit URI: RECLASS(ClassName, BaseClass, "//uri")
    RECLASS_PATTERN = re.compile(
        r'RECLASS\s*\(\s*(\w+)\s*,\s*([\w:]+)\s*,\s*"([^"]+)"\s*\)'
    )
    RECLASS_VIRTUAL_PATTERN = re.compile(
        r'RECLASS_VIRTUAL\s*\(\s*(\w+)\s*,\s*([\w:]+)\s*,\s*"([^"]+)"\s*\)'
    )

    # MECLASS without URI: MECLASS(ClassName, BaseClass)
    # Uses stringified class name as type ID: #_MeClass → "ClassName"
    MECLASS_PATTERN = re.compile(
        r'MECLASS\s*\(\s*(\w+)\s*,\s*([\w:]+)\s*\)'
    )
    MECLASS_VIRTUAL_PATTERN = re.compile(
        r'MECLASS_VIRTUAL\s*\(\s*(\w+)\s*,\s*([\w:]+)\s*\)'
    )

    def __init__(self):
        self.types: List[TypeInfo] = []

    def scan_file(self, file_path: Path) -> None:
        """Scan a single header file for RECLASS and MECLASS macros"""
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()

            for match in self.RECLASS_PATTERN.finditer(content):
                class_name = match.group(1)
                base_class = match.group(2)
                resource_id = match.group(3)

                type_info = TypeInfo(
                    typeId=resource_id,
                    cppClass=class_name,
                    baseClass=base_class,
                    headerPath=str(file_path)
                )

                self.types.append(type_info)
                print(f"  Found: {class_name} -> {resource_id}")

            for match in self.RECLASS_VIRTUAL_PATTERN.finditer(content):
                class_name = match.group(1)
                base_class = match.group(2)
                resource_id = match.group(3)
               
                type_info = TypeInfo(
                    typeId=resource_id,
                    cppClass=class_name,
                    baseClass=base_class,
                    headerPath=str(file_path),
                    isVirtual=True
                )
                self.types.append(type_info)
                print(f"  Found (virtual): {class_name} -> {resource_id}")

            # Scan for MECLASS (without explicit URI)
            for match in self.MECLASS_PATTERN.finditer(content):
                class_name = match.group(1)
                base_class = match.group(2)
                # MECLASS uses class name as type ID (stringified via #_MeClass)
                resource_id = class_name

                type_info = TypeInfo(
                    typeId=resource_id,
                    cppClass=class_name,
                    baseClass=base_class,
                    headerPath=str(file_path)
                )

                self.types.append(type_info)
                print(f"  Found (MECLASS): {class_name} -> {resource_id}")

            # Scan for MECLASS_VIRTUAL
            for match in self.MECLASS_VIRTUAL_PATTERN.finditer(content):
                class_name = match.group(1)
                base_class = match.group(2)
                resource_id = class_name

                type_info = TypeInfo(
                    typeId=resource_id,
                    cppClass=class_name,
                    baseClass=base_class,
                    headerPath=str(file_path),
                    isVirtual=True
                )

                self.types.append(type_info)
                print(f"  Found (MECLASS_VIRTUAL): {class_name} -> {resource_id}")

        except Exception as e:
            print(f"Error scanning {file_path}: {e}", file=sys.stderr)

    def scan_directory(self, root: Path) -> None:
        """Recursively scan directory for header files"""
        header_extensions = {'.h', '.hpp'}

        for root_dir, _, files in os.walk(root):
            for file in files:
                file_path = Path(root_dir) / file
                if file_path.suffix in header_extensions:
                    self.scan_file(file_path)

    def save_to_json(self, output_path: Path) -> None:
        """Save scanned types to JSON cache file"""
        output_path.parent.mkdir(parents=True, exist_ok=True)

        data = {
            'version': 1,
            'types': [asdict(t) for t in self.types]
        }

        with open(output_path, 'w', encoding='utf-8') as f:
            json.dump(data, f, indent=2)

        print(f"\nSaved {len(self.types)} types to {output_path}")

def main():
    if len(sys.argv) < 2:
        print("Usage: python scan_types.py <project_root> [output_json]")
        sys.exit(1)

    project_root = Path(sys.argv[1])
    output_file = Path(sys.argv[2]) if len(sys.argv) > 2 else project_root / '.rhea' / 'cache' / 'type_registry.json'

    if not project_root.exists():
        print(f"Error: Project root '{project_root}' does not exist")
        sys.exit(1)

    print(f"Scanning project: {project_root}")
    print(f"Output file: {output_file}")
    print()

    scanner = TypeScanner()
    scanner.scan_directory(project_root)
    scanner.save_to_json(output_file)

if __name__ == '__main__':
    main()
