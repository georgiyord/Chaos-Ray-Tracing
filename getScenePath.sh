#!/usr/bin/env bash

set -euo pipefail

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <CRT##-Scene#>" >&2
    echo "Example: $0 CRT11-Scene0" >&2
    exit 1
fi

package="$1"

out="$(nix eval --raw ".#${package}.outPath")"

if [ ! -e "$out" ]; then
    echo "Package '${package}' is not built yet; building..." >&2
    nix build ".#${package}" --no-link
fi

echo "$out/$(nix eval --raw ".#${package}.scene")"
