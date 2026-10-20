#!/bin/sh
if [ -z "$1" ]; then
    echo "Usage: ./init_project.sh <new_project_name>"
    exit 1
fi
OLD="a_ljc_program"
NEW="$1"

# Rename files
mv ${OLD}.c ${NEW}.c
mv ${OLD}.1 ${NEW}.1 2>/dev/null || true

# Replace text inside files (Linux & macOS compatible)
for file in Makefile *.c *.h *.rc *.manifest.xml; do
    if [ -f "$file" ]; then
        sed -i.bak "s/$OLD/$NEW/g" "$file" && rm "${file}.bak"
    fi
done
echo "Project successfully renamed to '$NEW'!"