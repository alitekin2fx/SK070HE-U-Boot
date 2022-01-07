/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * K3: Architecture common definitions
 *
 * Copyright (C) 2018 Texas Instruments Incorporated - http://www.ti.com/
 *	Lokesh Vutla <lokeshvutla@ti.com>
 */

#include <asm/armv7_mpu.h>
#include <asm/hardware.h>

#define AM65X	0xbb5a
#define J721E	0xbb64
#define J7200	0xbb6d

#define REV_PG1_0	0
#define REV_PG2_0	1

struct fwl_data {
	const char *name;
	u16 fwl_id;
	u16 regions;
};

void setup_k3_mpu_regions(void);
int early_console_init(void);
void disable_linefill_optimization(void);
void start_non_linux_remote_cores(void);
int load_firmware(char *name_fw, char *name_loadaddr, u32 *loadaddr);
void remove_fwl_configs(struct fwl_data *fwl_data, size_t fwl_data_size);
void k3_sysfw_print_ver(void);
void mmr_unlock(phys_addr_t base, u32 partition);
bool is_rom_loaded_sysfw(struct rom_extended_boot_data *data);
void ti_secure_image_post_process(void **p_image, size_t *p_size);
