# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2017 Intel Corporation

includes += include_directories('.')

cflags += [ '-DABI_VERSION="@0@"'.format(abi_version) ]

sources += files(
        'eal_common_bus.c',
        'eal_common_class.c',
        'eal_common_config.c',
        'eal_common_debug.c',
        'eal_common_dev.c',
        'eal_common_devargs.c',
        'eal_common_errno.c',
        'eal_common_fbarray.c',
        'eal_common_hexdump.c',
        'eal_common_interrupts.c',
        'eal_common_launch.c',
        'eal_common_lcore.c',
        'eal_common_lcore_var.c',
        'eal_common_mcfg.c',
        'eal_common_memalloc.c',
        'eal_common_memory.c',
        'eal_common_memzone.c',
        'eal_common_options.c',
        'eal_common_string_fns.c',
        'eal_common_tailqs.c',
        'eal_common_thread.c',
        'eal_common_timer.c',
        'eal_common_trace_points.c',
        'eal_common_uuid.c',
        'malloc_elem.c',
        'malloc_heap.c',
        'rte_bitset.c',
        'rte_malloc.c',
        'rte_random.c',
        'rte_reciprocal.c',
        'rte_service.c',
        'rte_version.c',
)
if is_linux or is_windows
    sources += files('eal_common_dynmem.c')
endif
if not is_windows
    sources += files(
            'eal_common_cpuflags.c',
            'eal_common_hypervisor.c',
            'eal_common_proc.c',
            'eal_common_trace.c',
            'eal_common_trace_ctf.c',
            'eal_common_trace_utils.c',
            'hotplug_mp.c',
            'malloc_mp.c',
            'rte_keepalive.c',
    )
endif
