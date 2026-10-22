#!/bin/sh -e
# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2026 Red Hat, Inc.
#
# Check for unnecessary internal symbol exports in DPDK drivers/libraries.
# An internal export is unnecessary if the symbol is never used outside
# the component that defines it.

cd $(dirname $0)/..

usage() {
    cat <<EOF
Usage: $(basename $0) [OPTIONS] [COMPONENT_PATH...]

List internal symbol exports and check for unnecessary ones.

Options:
  -h, --help     Show this help message
  -l, --list     List only component paths with issues
  -q, --quiet    No output, just exit code
  -v, --verbose  Show all components and all symbols

Examples:
  $(basename $0)                              # List unused symbols for all components
  $(basename $0) drivers/net/intel/idpf       # List unused symbols for specific component
  $(basename $0) -v drivers/net/intel/idpf    # List all symbols for specific component
EOF
    exit 0
}

list_only=0
quiet=0
verbose=0
components=""

while [ $# -gt 0 ]; do
    case $1 in
        -h|--help) usage ;;
        -l|--list) list_only=1; shift ;;
        -q|--quiet) quiet=1; shift ;;
        -v|--verbose) verbose=1; shift ;;
        *) components="$components $1"; shift ;;
    esac
done

# -l takes precedence: suppress other output, list components in main loop
[ $list_only -eq 0 ] || quiet=1

find_component_root() {
    dir=$1
    while [ -n "$dir" ] && [ "$dir" != "." ]; do
        if [ -f "$dir/meson.build" ]; then
            parent=$(dirname "$dir")
            # Component root: parent has no meson.build, or parent doesn't use subdir()
            if [ ! -f "$parent/meson.build" ] || ! grep -q "^subdir(" "$parent/meson.build"; then
                echo "$dir" && return
            fi
        fi
        dir=$(dirname "$dir")
    done
}

get_internal_symbols() {
    dir=$1
    git grep -h "^RTE_EXPORT_INTERNAL_SYMBOL(" -- "$dir" 2>/dev/null | \
        sed 's/^RTE_EXPORT_INTERNAL_SYMBOL(\([^)]*\))/\1/' | \
        grep -v '^per_lcore_' | \
        sort -u
}

check_component() {
    comp=$1
    [ -d "$comp" ] || return 1

    symbols=$(get_internal_symbols "$comp")
    [ -n "$symbols" ] || return 0

    pattern=$(echo $symbols | tr ' ' '|')

    used_outside=$(git grep -oh -w -E "$pattern" -- :!"$comp" 2>/dev/null | sort -u)

    header_refs=$(git grep -oh -w -E "$pattern" -- "$comp/**.h" 2>/dev/null | sort | uniq -c | \
        awk '$1 > 1 {print $2}')

    # First pass: check for any unused symbol
    has_unused=0
    for sym in $symbols; do
        if ! echo "$used_outside" | grep -q -x "$sym" && \
           ! echo "$header_refs" | grep -q -x "$sym"; then
            has_unused=1
            break
        fi
    done

    [ $verbose -eq 1 ] || [ $has_unused -eq 1 ] || return 0

    # Second pass: classify and optionally print
    [ $quiet -eq 1 ] || echo "$comp:"
    comp_ret=0
    for sym in $symbols; do
        if echo "$used_outside" | grep -q -x "$sym"; then
            [ $quiet -eq 0 ] && [ $verbose -eq 1 ] && echo "  $sym"
        elif echo "$header_refs" | grep -q -x "$sym"; then
            [ $quiet -eq 0 ] && [ $verbose -eq 1 ] && echo "  $sym"
        else
            # Count occurrences to classify:
            # - DEAD: only export exists (count=1) - a bug
            # - UNUSED: declaration + export + definition but never called (count≤3)
            # - LOCAL: called internally but not outside component
            count=$(git grep -w "$sym" 2>/dev/null | wc -l)
            if [ "$count" -eq 1 ]; then
                [ $quiet -eq 0 ] && echo "  DEAD: $sym"
                comp_ret=$((comp_ret | 4))
            elif [ "$count" -le 3 ]; then
                [ $quiet -eq 0 ] && echo "  UNUSED: $sym"
                comp_ret=$((comp_ret | 2))
            else
                [ $quiet -eq 0 ] && echo "  LOCAL: $sym"
                comp_ret=$((comp_ret | 1))
            fi
        fi
    done

    return $comp_ret
}

find_components() {
    git grep -l "^RTE_EXPORT_INTERNAL_SYMBOL(" -- ${@:-.} 2>/dev/null | \
        while read -r file; do
            find_component_root "$(dirname "$file")"
        done | sort -u
}

components=$(find_components $components)

# Exit codes (bitmask):
# 0 = clean
# 1 = LOCAL found
# 2 = UNUSED found
# 4 = DEAD found
ret=0
for comp in $components; do
    comp_ret=0
    check_component "$comp" || comp_ret=$?
    [ $list_only -eq 0 ] || [ $comp_ret -eq 0 ] || echo "$comp"
    ret=$((ret | comp_ret))
done

exit $ret
