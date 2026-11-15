#!/usr/bin/env bash
# Regenerate the screenshots in docs/assets/img/ from the real example programs.
#
# Requirements: CMake + a C++17 compiler, Python 3, Node.js 18+, and Chrome/Chromium
# (set CHROMIUM_PATH if it is not in a standard location). Linux or macOS.
#
# Usage: scripts/screenshots/generate.sh [shot-name ...]   (default: all shots in shots.json)
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BUILD="${BUILD_DIR:-$ROOT/build-screenshots}"
OUT="$ROOT/docs/assets/img"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

echo "==> Building examples in $BUILD"
cmake -S "$ROOT" -B "$BUILD" -DCMAKE_BUILD_TYPE=Release -DCPPTUI_BUILD_EXAMPLES=ON >/dev/null
cmake --build "$BUILD" -j >/dev/null

echo "==> Installing renderer dependencies"
if [[ ! -d "$HERE/node_modules" || "$HERE/package-lock.json" -nt "$HERE/node_modules/.package-lock.json" ]]; then
  (cd "$HERE" && npm ci --silent --no-audit --no-fund)
fi

mkdir -p "$OUT"
python3 - "$HERE/shots.json" "$@" <<'PY' > "$TMP/shots.tsv"
import json, sys
cfg = json.load(open(sys.argv[1]))
only = set(sys.argv[2:])
d = cfg["defaults"]
for s in cfg["shots"]:
    if only and s["name"] not in only:
        continue
    g = lambda k: str(s.get(k, d.get(k, "")))
    gif = s.get("gif") or {}
    print("\t".join([s["name"], s["binary"], g("cols"), g("rows"), g("seconds"), g("fontSize"),
                     json.dumps(s["keys"]) if s.get("keys") else "-",
                     " ".join(str(gif.get(k, 0)) for k in ("from", "to", "fps", "hold")) if gif else "-"]))
PY

while IFS=$'\t' read -r name binary cols rows seconds font keys gif; do
  [[ "$keys" == "-" ]] && keys=""
  echo "==> $name"
  bin="$BUILD/$binary"
  [[ -x "$bin" ]] || { echo "  missing binary: $bin" >&2; exit 1; }
  (cd "$BUILD" && python3 "$HERE/capture.py" "$bin" "$cols" "$rows" "$seconds" "$TMP/$name.ansi" "$keys")
  node "$HERE/render.mjs" "$TMP/$name.ansi" "$OUT/$name.png" "$cols" "$rows" "$font"
  if [[ "$gif" != "-" ]]; then
    # shellcheck disable=SC2086  # "from to fps hold" is meant to split into four arguments
    node "$HERE/render.mjs" "$TMP/$name.ansi" "$OUT/$name.gif" "$cols" "$rows" "$font" $gif
  fi
done < "$TMP/shots.tsv"

echo "Done. Images are in docs/assets/img/"
