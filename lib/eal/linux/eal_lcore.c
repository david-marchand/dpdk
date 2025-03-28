/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

#include <errno.h>
#include <glob.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>

#include <rte_log.h>

#include "eal_private.h"
#include "eal_filesystem.h"
#include "eal_thread.h"

#define SYS_CPU_DIR "/sys/devices/system/cpu/cpu%u"
#define CORE_ID_FILE "topology/core_id"
#define NUMA_NODE_PATH "/sys/devices/system/node"

/* Check if a cpu is present by the presence of the cpu information for it */
int
eal_cpu_detected(unsigned lcore_id)
{
	char path[PATH_MAX];
	int len = snprintf(path, sizeof(path), SYS_CPU_DIR
		"/"CORE_ID_FILE, lcore_id);
	if (len <= 0 || (unsigned)len >= sizeof(path))
		return 0;
	if (access(path, F_OK) != 0)
		return 0;

	return 1;
}

/*
 * Get CPU socket id (NUMA node) for a logical core.
 *
 * This searches each nodeX directories in /sys for the symlink for the given
 * lcore_id and returns the numa node where the lcore is found. If lcore is not
 * found on any numa node, returns zero.
 */
unsigned
eal_cpu_socket_id(unsigned lcore_id)
{
	char pattern[PATH_MAX];
	glob_t gl;

	snprintf(pattern, sizeof(pattern), "%s/node*/cpu%u", NUMA_NODE_PATH, lcore_id);
	if (glob(pattern, 0, NULL, &gl) == 0) {
		long socket;

		errno = 0;
		socket = strtol(gl.gl_pathv[0] + strlen(NUMA_NODE_PATH) + strlen("/node"),
			NULL, 10);
		if (errno == 0)
			return socket;
	}
	return 0;
}

/* Get the cpu core id value from the /sys/.../cpuX core_id value */
unsigned
eal_cpu_core_id(unsigned lcore_id)
{
	char path[PATH_MAX];
	unsigned long id;

	int len = snprintf(path, sizeof(path), SYS_CPU_DIR "/%s", lcore_id, CORE_ID_FILE);
	if (len <= 0 || (unsigned)len >= sizeof(path))
		goto err;
	if (eal_parse_sysfs_value(path, &id) != 0)
		goto err;
	return (unsigned)id;

err:
	EAL_LOG(ERR, "Error reading core id value from %s "
			"for lcore %u - assuming core 0", SYS_CPU_DIR, lcore_id);
	return 0;
}
