/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

#ifndef EAL_VFIO_H_
#define EAL_VFIO_H_

/* get the vfio container that devices are bound to by default */
int vfio_get_default_container_fd(void);

int vfio_get_iommu_type(void);

int vfio_mp_sync_setup(void);
void vfio_mp_sync_cleanup(void);

#define EAL_VFIO_MP "eal_vfio_mp_sync"

#define SOCKET_REQ_CONTAINER 0x100
#define SOCKET_REQ_GROUP 0x200
#define SOCKET_REQ_DEFAULT_CONTAINER 0x400
#define SOCKET_REQ_IOMMU_TYPE 0x800
#define SOCKET_OK 0x0
#define SOCKET_NO_FD 0x1
#define SOCKET_ERR 0xFF

struct vfio_mp_param {
	int req;
	int result;
	union {
		int group_num;
		int iommu_type_id;
	};
};

#endif /* EAL_VFIO_H_ */
