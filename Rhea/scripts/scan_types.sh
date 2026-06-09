#!/bin/bash
# Rhea Type Registry Scanner
# Scans the project for RECLASS macros and generates type_registry.json

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
CACHE_DIR="$PROJECT_ROOT/.rhea/cache"
OUTPUT_FILE="$CACHE_DIR/type_registry.json"
PYTHON_SCRIPT="$PROJECT_ROOT/tools/scan_types.py"

echo "========================================"
echo "Rhea Type Registry Scanner"
echo "========================================"
echo ""
echo "Project Root: $PROJECT_ROOT"
echo "Cache Dir: $CACHE_DIR"
echo "Output: $OUTPUT_FILE"
echo ""

# Check if Python is available
if ! command -v python3 &> /dev/null; then
	echo "ERROR: python3 not found in PATH"
	echo "Please install Python 3.6+ or add it to PATH"
	exit 1
fi

# Check if scan script exists
if [ ! -f "$PYTHON_SCRIPT" ]; then
	echo "ERROR: Scanner script not found: $PYTHON_SCRIPT"
	exit 1
fi

# Create cache directory if it doesn't exist
if [ ! -d "$CACHE_DIR" ]; then
	echo "Creating cache directory..."
	mkdir -p "$CACHE_DIR"
fi

# Run the scanner
echo "Running type scanner..."
echo ""
python3 "$PYTHON_SCRIPT" "$PROJECT_ROOT" "$OUTPUT_FILE"

if [ $? -ne 0 ]; then
	echo ""
	echo "ERROR: Scanner failed"
	exit 1
fi

echo ""
echo "========================================"
echo "Type registry updated successfully!"
echo "========================================"
echo ""
echo "Cache file: $OUTPUT_FILE"
echo ""

# Show statistics
if [ -f "$OUTPUT_FILE" ]; then
	FILE_SIZE=$(wc -c < "$OUTPUT_FILE")
	echo "File size: $FILE_SIZE bytes"

	# Count number of types
	TYPE_COUNT=$(grep -o '"typeId"' "$OUTPUT_FILE" | wc -l)
	echo "Types found: $TYPE_COUNT"
fi

echo ""
