#!/bin/sh -xe

# determine minimum Meson version
ci_dir=$(dirname $(readlink -f $0))
meson_ver=$(python3 $ci_dir/../buildtools/get-min-meson-version.py)

python3 -m pip install --upgrade "meson==$meson_ver"

# setup hugepages. error ignored because having hugepage is not mandatory.
cat /proc/meminfo
sh -c 'echo 1024 > /proc/sys/vm/nr_hugepages' || true
cat /proc/meminfo
