..  SPDX-License-Identifier: BSD-3-Clause
    Copyright(c) 2019 Intel Corporation

Intel(R) FPGA 5GNR FEC Poll Mode Driver
=======================================

The BBDEV FPGA 5GNR FEC poll mode driver (PMD) supports an FPGA implementation of a VRAN
LDPC Encode / Decode 5GNR wireless acceleration function, using Intel's PCI-e and FPGA
based Vista Creek (N3000, referred to as VC_5GNR in the code)
as well as Arrow Creek (N6000, referred to as AGX100 in the code).

Features
--------

FPGA 5GNR FEC PMD supports the following BBDEV capabilities:

- LDPC Encode in the DL
- LDPC Decode in the UL
- 8 VFs per PF (physical device)
- Maximum of 32 UL queues per VF
- Maximum of 32 DL queues per VF
- PCIe Gen-3 x8 Interface
- MSI-X
- SR-IOV

FPGA 5GNR FEC PMD supports the following BBDEV capabilities:

* For the LDPC encode operation:
   - ``RTE_BBDEV_LDPC_CRC_24B_ATTACH`` :  set to attach CRC24B to CB(s)
   - ``RTE_BBDEV_LDPC_RATE_MATCH`` :  if set then do not do Rate Match bypass

* For the LDPC decode operation:
   - ``RTE_BBDEV_LDPC_CRC_TYPE_24B_CHECK`` :  check CRC24B from CB(s)
   - ``RTE_BBDEV_LDPC_ITERATION_STOP_ENABLE`` :  disable early termination
   - ``RTE_BBDEV_LDPC_CRC_TYPE_24B_DROP`` :  drops CRC24B bits appended while decoding
   - ``RTE_BBDEV_LDPC_HQ_COMBINE_IN_ENABLE`` :  provides an input for HARQ combining
   - ``RTE_BBDEV_LDPC_HQ_COMBINE_OUT_ENABLE`` :  provides an input for HARQ combining
   - ``RTE_BBDEV_LDPC_INTERNAL_HARQ_MEMORY_IN_ENABLE`` :  HARQ memory input is internal
   - ``RTE_BBDEV_LDPC_INTERNAL_HARQ_MEMORY_OUT_ENABLE`` :  HARQ memory output is internal
   - ``RTE_BBDEV_LDPC_INTERNAL_HARQ_MEMORY_LOOPBACK`` :  loopback data to/from HARQ memory
   - ``RTE_BBDEV_LDPC_INTERNAL_HARQ_MEMORY_FILLERS`` :  HARQ memory includes the fillers bits


Limitations
-----------

FPGA 5GNR FEC does not support the following:

- Scatter-Gather function


Installation
------------

Section 3 of the DPDK manual provides instructions on installing and compiling DPDK.

DPDK requires hugepages to be configured as detailed in section 2 of the DPDK manual.
The bbdev test application has been tested with a configuration 40 x 1GB hugepages. The
hugepage configuration of a server may be examined using:

.. code-block:: console

   grep Huge* /proc/meminfo


Initialization
--------------

When the device first powers up, its PCI Physical Functions (PF) can be listed through this command:

Vista Creek (N3000)

.. code-block:: console

  sudo lspci -vd8086:0d8f

Arrow Creek (N6000)

.. code-block:: console

  sudo lspci -vd8086:5799

The physical and virtual functions are compatible with Linux UIO drivers:
``vfio_pci`` and ``igb_uio``. However, in order to work the FPGA 5GNR FEC device firstly needs
to be bound to one of these linux drivers through DPDK.

For more details on how to bind the PF device and create VF devices, see
:ref:`linux_gsg_binding_kernel`.


Configure the VFs through PF
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The PCI virtual functions must be configured before working or getting assigned
to VMs/Containers. The configuration involves allocating the number of hardware
queues, priorities, load balance, bandwidth and other settings necessary for the
device to perform FEC functions.

This configuration needs to be executed at least once after reboot or PCI FLR and can
be achieved by using the function ``rte_fpga_5gnr_fec_configure()``, which sets up the
parameters defined in ``rte_fpga_5gnr_fec_conf`` structure:

.. code-block:: c

  struct rte_fpga_5gnr_fec_conf {
      bool pf_mode_en;
      uint8_t vf_ul_queues_number[FPGA_5GNR_FEC_NUM_VFS];
      uint8_t vf_dl_queues_number[FPGA_5GNR_FEC_NUM_VFS];
      uint8_t ul_bandwidth;
      uint8_t dl_bandwidth;
      uint8_t ul_load_balance;
      uint8_t dl_load_balance;
  };

- ``pf_mode_en``: identifies whether only PF is to be used, or the VFs. PF and
  VFs are mutually exclusive and cannot run simultaneously.
  Set to 1 for PF mode enabled.
  If PF mode is enabled all queues available in the device are assigned
  exclusively to PF and 0 queues given to VFs.

- ``vf_*l_queues_number``: defines the hardware queue mapping for every VF.

- ``*l_bandwidth``: Only used for the Vista Creek schedule algorithm
  in case of congestion on PCIe interface.
  The device allocates different bandwidth to UL and DL.
  The weight is configured by this setting.
  The unit of weight is 3 code blocks.
  For example, if the code block cbps (code block per second) ratio
  between UL and DL is 12:1, then the configuration value should be set to 36:3.
  The schedule algorithm is based on code block regardless the length of each block.

- ``*l_load_balance``: hardware queues are load-balanced in a round-robin
  fashion. Queues get filled first-in first-out until they reach a pre-defined
  watermark level, if exceeded, they won't get assigned new code blocks..
  This watermark is defined by this setting.

  If all hardware queues exceeds the watermark, no code blocks will be
  streamed in from UL/DL code block FIFO.


An example configuration code calling the function ``rte_fpga_5gnr_fec_configure()`` is shown
below:

.. code-block:: c

  struct rte_fpga_5gnr_fec_conf conf;
  unsigned int i;

  memset(&conf, 0, sizeof(struct rte_fpga_5gnr_fec_conf));
  conf.pf_mode_en = 1;

  for (i = 0; i < FPGA_5GNR_FEC_NUM_VFS; ++i) {
      conf.vf_ul_queues_number[i] = 4;
      conf.vf_dl_queues_number[i] = 4;
  }
  conf.ul_bandwidth = 12;
  conf.dl_bandwidth = 5;
  conf.dl_load_balance = 64;
  conf.ul_load_balance = 64;

  /* setup FPGA PF */
  ret = rte_fpga_5gnr_fec_configure(info->dev_name, &conf);
  TEST_ASSERT_SUCCESS(ret,
      "Failed to configure 5GNR FPGA PF for bbdev %s",
      info->dev_name);


Test Application
----------------

BBDEV provides a test application, ``test-bbdev.py`` and range of test data for testing
the functionality of the device, depending on the device's capabilities.

.. code-block:: console

   "-p", "--testapp-path": specifies path to the bbdev test app.
   "-e", "--eal-params"	: EAL arguments which are passed to the test app.
   "-t", "--timeout"	: Timeout in seconds (default=300).
   "-c", "--test-cases"	: Defines test cases to run. Run all if not specified.
   "-v", "--test-vector"	: Test vector path (default=dpdk_path+/app/test-bbdev/test_vectors/bbdev_null.data).
   "-n", "--num-ops"	: Number of operations to process on device (default=32).
   "-b", "--burst-size"	: Operations enqueue/dequeue burst size (default=32).
   "-l", "--num-lcores"	: Number of lcores to run (default=16).
   "-i", "--init-device" : Initialise PF device with default values.


To execute the test application tool using simple decode or encode data,
type one of the following:

.. code-block:: console

   ./test-bbdev.py -c validation -n 64 -b 1 -v ./ldpc_dec_default.data
   ./test-bbdev.py -c validation -n 64 -b 1 -v ./ldpc_enc_default.data


The test application ``test-bbdev.py`` supports the ability to configure the PF device
with a default set of values, if the "-i" or "- -init-device" option is included.
The default values are defined in ``test_bbdev_perf.c`` as:

- VF_UL_QUEUE_VALUE 4
- VF_DL_QUEUE_VALUE 4
- UL_BANDWIDTH 3
- DL_BANDWIDTH 3
- UL_LOAD_BALANCE 128
- DL_LOAD_BALANCE 128


Test Vectors
~~~~~~~~~~~~

In addition to the simple LDPC decoder and LDPC encoder tests, bbdev also provides
a range of additional tests under the test_vectors folder, which may be useful. The results
of these tests will depend on the FPGA 5GNR FEC capabilities.


Alternate Baseband Device configuration tool
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

On top of the embedded configuration feature supported in test-bbdev using "- -init-device"
option, there is also a tool available to perform that device configuration using a companion
application.
The ``pf_bb_config`` application notably enables then to run bbdev-test from the VF
and not only limited to the PF as captured above.

See for more details: https://github.com/intel/pf-bb-config

Specifically for the BBDEV FPGA 5GNR FEC PMD, the command below can be used:

Vista Creek (N3000)

.. code-block:: console

  ./pf_bb_config FPGA_5GNR -c fpga_5gnr/fpga_5gnr_config_vf.cfg
  ./test-bbdev.py -e="-l 4-11 -a${VF_PCI_ADDR}" -c validation -n 64 -b 32 -l 1 -v ./ldpc_dec_default.data

Arrow Creek (N6000)

.. code-block:: console

  ./pf_bb_config AGX100 -c agx100/agx100_config_1vf.cfg
  ./test-bbdev.py -e="-l 4-11 -a${VF_PCI_ADDR}" -c validation -n 64 -b 32 -l 1 -v ./ldpc_dec_default.data
