#! /bin/sh -e
# SPDX-License-Identifier: BSD-3-Clause
# Copyright 2015 6WIND S.A.
# Copyright 2019 Mellanox Technologies, Ltd
# Copyright (c) 2024 Red Hat, Inc.

# Run a quick forwarding test without hugepage, with one testpmd using
# virtio-user and another using vhost-user.

build=${1:-build} # first argument can be the build directory
testpmd=$1 # or first argument can be the testpmd path
corelist_server=${2:-0,1,2} # default using cores 0, 1, 2
corelist_client=${3:-0,1,3} # default using cores 0, 1, 3
eal_options=$4
testpmd_options=$5

sock=$(mktemp -t dpdk.virtio-user.XXXXXX.sock)
pid_client=
pid_server=
cleanup() {
	rm -f $sock
	[ "$pid_client $pid_server" != " " ] || return
	if kill -0 $pid_client $pid_server >/dev/null 2>/dev/null; then
		sleep 2
	fi
	if kill -0 $pid_client >/dev/null 2>/dev/null ; then
		echo ============================
		echo Force killing client testpmd
		kill -9 $pid_client
	fi
	if kill -0 $pid_server >/dev/null 2>/dev/null ; then
		echo ============================
		echo Force killing server testpmd
		kill -9 $pid_server
	fi
}

trap cleanup EXIT

[ -f "$testpmd" ] && build=$(dirname $(dirname $testpmd))
[ -f "$testpmd" ] || testpmd=$build/app/dpdk-testpmd
[ -f "$testpmd" ] || testpmd=$build/app/testpmd
if [ ! -f "$testpmd" ] ; then
	echo 'ERROR: testpmd cannot be found' >&2
	exit 1
fi

if ldd $testpmd | grep -q librte_ ; then
	export LD_LIBRARY_PATH=$build/lib:$LD_LIBRARY_PATH
	libs="-d $build/drivers"
else
	libs=
fi

rm -f $sock
( $testpmd -l $corelist_server --file-prefix=virtio-user --no-huge -m 1024 \
	--single-file-segments --no-pci $libs \
	--vdev net_virtio_user0,path=$sock,queues=2,server=1 $eal_options -- \
	--no-mlockall --stats-period 1 --rxq 2 --txq 2 --nb-cores=2 --tx-first -a $testpmd_options \
>/dev/null 2>/dev/null) &
pid_server=$!

( $testpmd -l $corelist_client --file-prefix=vhost --no-huge -m 1024 \
	--single-file-segments --no-pci $libs \
	--vdev net_vhost0,iface=$sock,queues=8,client=1 $eal_options -- \
	--no-mlockall --stats-period 1 --rxq 8 --txq 8 --nb-cores=2 --tx-first -a $testpmd_options \
) &
pid_client=$!

sleep 2
kill $pid_client
sleep 1
kill $pid_server
