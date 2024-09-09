# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2024 Red Hat, Inc.

# if not get_option('check_includes')
#     build = false
#     subdir_done()
# endif

project('Linux uapi headers check', 'c',
        version: '0.0.1',
        license: 'BSD',
        default_options: [
            'c_args=-nostdinc',
        ],
)

gen_c_file_for_header = find_program('../../buildtools/chkincs/gen_c_file_for_header.py')
gen_c_files = generator(gen_c_file_for_header,
        output: '@BASENAME@.c',
        arguments: ['@INPUT@', '@OUTPUT@'])

headers=files(
        'linux/vduse.h',
        'linux/virtio_net.h',
)
sources = files('main.c')
sources += gen_c_files.process(headers)

executable('chkuapiincs', sources, install: false)

