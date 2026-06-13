#!/usr/bin/env python3
"""
Type Registry Scanner for Rhea Engine
Scans C++ header files for RECLASS and REPROPERTY macros and generates type registry cache.

Usage:
    python scan_types.py <project_root> [output_json]

Example:
    python scan_types.py Rhea .rhea/type_registry.json
"""

import re
import json
import os
import sys
from pathlib import Path
from typing import List, Dict, Optional
from dataclasses import dataclass, asdict, field

@dataclass
class PropertyInfo:
    name: str
    propertyType: str
    baseType: str
    label: str = ""
    flags: List[str] = field(default_factory=list)
    isReference: bool = False

@dataclass
class TypeInfo:
    typeId: str
    cppClass: str
    baseClass: str
    headerPath: str
    isVirtual: bool = False
    properties: List[PropertyInfo] = field(default_factory=list)

class TypeScanner:
    """Scans header files for RECLASS and REPROPERTY macro definitions"""

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

    # REPROPERTY: REPROPERTY(Type, "name", Label("UI Label"), Edit, Read);
    REPROPERTY_PATTERN = re.compile(
        r'REPROPERTY\s*\(\s*'
        r'([\w:<>]+)\s*,\s*'           # Type (z.B. rhea::Position, Ref<rhea::Mesh>)
        r'"([^"]+)"\s*'                # Property Name
        r'(?:,\s*(.+?))?\s*'           # Optional: Attributes (Label, Edit, Read, etc.)
        r'\);'
    )

    def __init__(self):
        self.types: List[TypeInfo] = []
        self.current_type: Optional[TypeInfo] = None

    def scan_file(self, file_path: Path) -> None:
        """Scan a single header file for RECLASS and REPROPERTY macros"""
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()

            # Scan für Type-Definitionen
            self._scan_reclass(content, file_path)
            self._scan_meclass(content, file_path)

        except Exception as e:
            print(f"Error scanning {file_path}: {e}", file=sys.stderr)

    def _scan_reclass(self, content: str, file_path: Path):
        """Scan for RECLASS macros"""
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

            # Search properties for this type after the RECLASS macro
            self._scan_properties_for_type(content, match.end(), type_info)

            self.types.append(type_info)
            print(f"  Found: {class_name} -> {resource_id} ({len(type_info.properties)} properties)")

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

            self._scan_properties_for_type(content, match.end(), type_info)

            self.types.append(type_info)
            print(f"  Found (virtual): {class_name} -> {resource_id} ({len(type_info.properties)} properties)")

    def _scan_meclass(self, content: str, file_path: Path):
        """Scan for MECLASS macros"""
        for match in self.MECLASS_PATTERN.finditer(content):
            class_name = match.group(1)
            base_class = match.group(2)
            resource_id = class_name

            type_info = TypeInfo(
                typeId=resource_id,
                cppClass=class_name,
                baseClass=base_class,
                headerPath=str(file_path)
            )

            self._scan_properties_for_type(content, match.end(), type_info)

            self.types.append(type_info)
            print(f"  Found (MECLASS): {class_name} -> {resource_id} ({len(type_info.properties)} properties)")

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

            self._scan_properties_for_type(content, match.end(), type_info)

            self.types.append(type_info)
            print(f"  Found (MECLASS_VIRTUAL): {class_name} -> {resource_id} ({len(type_info.properties)} properties)")

    def _scan_properties_for_type(self, content: str, start_pos: int, type_info: TypeInfo):
        """Scan REPROPERTY macros after the type definition"""
        # Find the next "public:" or "private:" or "}" after start_pos
        # Properties must be BEFORE the first "public:" (typically directly after RECLASS)
        section_end = content.find('public:', start_pos)
        if section_end == -1:
            section_end = content.find('private:', start_pos)
        if section_end == -1:
            section_end = len(content)

        property_section = content[start_pos:section_end]

        for match in self.REPROPERTY_PATTERN.finditer(property_section):
            prop_type = match.group(1).strip()
            prop_name = match.group(2).strip()
            attributes = match.group(3).strip() if match.group(3) else ""

            prop_info = self._parse_property(prop_type, prop_name, attributes)
            type_info.properties.append(prop_info)

    def _parse_property(self, prop_type: str, prop_name: str, attributes: str) -> PropertyInfo:
        """Parse REPROPERTY attributes"""
        label = prop_name  # Default label
        flags = []
        is_reference = False
        base_type = prop_type

        # Parse Ref<T>
        if prop_type.startswith('Ref<'):
            is_reference = True
            flags.append('Reference')
            # Extract T from Ref<T>
            start = 4
            end = prop_type.rfind('>')
            if end != -1:
                base_type = prop_type[start:end].strip()

        # Parse attributes: Label("..."), Edit, Read, etc.
        if attributes:
            # Label("...")
            label_match = re.search(r'Label\s*\(\s*"([^"]+)"\s*\)', attributes)
            if label_match:
                label = label_match.group(1)

            # Flags
            if 'Edit' in attributes:
                flags.append('Edit')
            if 'Read' in attributes:
                flags.append('Read')
            if 'Write' in attributes:
                flags.append('Write')
            if 'Serialize' in attributes:
                flags.append('Serialize')

        return PropertyInfo(
            name=prop_name,
            propertyType=prop_type,
            baseType=base_type,
            label=label,
            flags=flags,
            isReference=is_reference
        )

    def scan_directory(self, directory: Path) -> None:
        """Recursively scan directory for header files"""
        for file_path in directory.rglob('*.h'):
            self.scan_file(file_path)
        for file_path in directory.rglob('*.hpp'):
            self.scan_file(file_path)

    def scan_project(self, project_root: str) -> None:
        """Scan entire project"""
        root = Path(project_root)
        if not root.exists():
            print(f"Error: Project root {project_root} does not exist", file=sys.stderr)
            return

        print(f"Scanning {project_root} for RECLASS/MECLASS/REPROPERTY macros...")
        self.scan_directory(root)
        print(f"Scanned {len(self.types)} types")

    def save_registry(self, output_path: str) -> None:
        """Save type registry to JSON"""
        output = Path(output_path)
        output.parent.mkdir(parents=True, exist_ok=True)

        registry = {
            "version": "1.0",
            "types": [asdict(t) for t in self.types]
        }

        with open(output, 'w', encoding='utf-8') as f:
            json.dump(registry, f, indent=2)

        print(f"Registry saved to {output_path}")


def main():
    if len(sys.argv) < 2:
        print("Usage: python scan_types.py <project_root> [output_json]")
        print("Example: python scan_types.py Rhea .rhea/type_registry.json")
        sys.exit(1)

    project_root = sys.argv[1]
    output_json = sys.argv[2] if len(sys.argv) > 2 else ".rhea/type_registry.json"

    scanner = TypeScanner()
    scanner.scan_project(project_root)
    scanner.save_registry(output_json)


if __name__ == "__main__":
    main()
