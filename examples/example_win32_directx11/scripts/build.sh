#!/usr/bin/env bash
set -u

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DIR="$(dirname "$SCRIPT_DIR")"
cd "$DIR" || exit 1

find_msbuild() {
  if command -v MSBuild.exe >/dev/null 2>&1; then command -v MSBuild.exe; return 0; fi
  local ver ed base p
  for base in "/c/Program Files" "/c/Program Files (x86)"; do
    for ver in 2022 2019; do
      for ed in Community Professional Enterprise BuildTools; do
        p="$base/Microsoft Visual Studio/$ver/$ed/MSBuild/Current/Bin/MSBuild.exe"
        [ -x "$p" ] && { printf '%s\n' "$p"; return 0; }
      done
    done
  done
  return 1
}

MSBUILD="$(find_msbuild)" || {
  echo "### MSBuild.exe not found. Install Visual Studio 2022 (with the C++ workload) or add MSBuild to PATH. ###"
  exit 1
}
echo "=== MSBuild: $MSBUILD ==="

echo "=== kill running instance (if any) ==="
taskkill //IM example_win32_directx11.exe //F 2>/dev/null
sleep 1

echo "=== build Release|x64 ==="
"$MSBUILD" example_win32_directx11.vcxproj \
  //p:Configuration=Release //p:Platform=x64 \
  //p:PlatformToolset=v143 //p:WindowsTargetPlatformVersion=10.0.22621.0 \
  //v:minimal //nologo
rc=$?
if [ $rc -ne 0 ]; then echo "### BUILD FAILED (rc=$rc) ###"; exit $rc; fi
echo "=== BUILD OK ==="

if [ "${1:-}" = "run" ]; then
  echo "=== launch ==="
  ( ./Release/example_win32_directx11.exe & )
  sleep 3
  tasklist //FI "IMAGENAME eq example_win32_directx11.exe" 2>/dev/null | grep -i example || echo "(process not found — may have crashed)"
fi
