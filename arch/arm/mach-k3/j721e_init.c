// SPDX-License-Identifier: GPL-2.0+
/*
 * J721E: SoC specific initialization
 *
 * Copyright (C) 2018-2019 Texas Instruments Incorporated - http://www.ti.com/
 *	Lokesh Vutla <lokeshvutla@ti.com>
 */

#include <common.h>
#include <spl.h>
#include <asm/io.h>
#include <asm/armv7_mpu.h>
#include <asm/arch/hardware.h>
#include <asm/arch/sysfw-loader.h>
#include "common.h"
#include <asm/arch/sys_proto.h>
#include <linux/soc/ti/ti_sci_protocol.h>
#include <dm.h>
#include <dm/uclass-internal.h>
#include <dm/pinctrl.h>
#include "../../../board/ti/common/board_detect.h"
#include <remoteproc.h>
#include <mmc.h>

#ifdef CONFIG_SPL_BUILD
#ifdef CONFIG_K3_LOAD_SYSFW
#ifdef CONFIG_TI_SECURE_DEVICE
struct fwl_data cbass_hc_cfg0_fwls[] = {
	{ "PCIE0_CFG", 2560, 8 },
	{ "PCIE1_CFG", 2561, 8 },
	{ "USB3SS0_CORE", 2568, 4 },
	{ "USB3SS1_CORE", 2570, 4 },
	{ "EMMC8SS0_CFG", 2576, 4 },
	{ "UFS_HCI0_CFG", 2580, 4 },
	{ "SERDES0", 2584, 1 },
	{ "SERDES1", 2585, 1 },
}, cbass_hc0_fwls[] = {
	{ "PCIE0_HP", 2528, 24 },
	{ "PCIE0_LP", 2529, 24 },
	{ "PCIE1_HP", 2530, 24 },
	{ "PCIE1_LP", 2531, 24 },
}, cbass_rc_cfg0_fwls[] = {
	{ "EMMCSD4SS0_CFG", 2380, 4 },
}, cbass_rc0_fwls[] = {
	{ "GPMC0", 2310, 8 },
}, infra_cbass0_fwls[] = {
	{ "PLL_MMR0", 8, 26 },
	{ "CTRL_MMR0", 9, 16 },
}, mcu_cbass0_fwls[] = {
	{ "MCU_R5FSS0_CORE0", 1024, 4 },
	{ "MCU_R5FSS0_CORE0_CFG", 1025, 2 },
	{ "MCU_R5FSS0_CORE1", 1028, 4 },
	{ "MCU_FSS0_CFG", 1032, 12 },
	{ "MCU_FSS0_S1", 1033, 8 },
	{ "MCU_FSS0_S0", 1036, 8 },
	{ "MCU_PSROM49152X32", 1048, 1 },
	{ "MCU_MSRAM128KX64", 1050, 8 },
	{ "MCU_CTRL_MMR0", 1200, 8 },
	{ "MCU_PLL_MMR0", 1201, 3 },
	{ "MCU_CPSW0", 1220, 2 },
}, wkup_cbass0_fwls[] = {
	{ "WKUP_CTRL_MMR0", 131, 16 },
};
#endif
#endif

static inline void cbass_qos_rmw(uintptr_t addr, uint32_t val, uint32_t mask)
{
	writel((readl(addr) & ~mask) | (val & mask), addr);
}

void setup_initiator_credentials(void)
{
	u32 i, mask;

	mask = QOS_ATYPE_MASK | QOS_VIRTID_MASK;

	/* Initiators for root_cell, virtid = 2 */
	/* MMC1*/
	cbass_qos_rmw(QOS_MMC1_RD_CBASS_MAP(0), QOS_PVU_CTX(2), mask);
	cbass_qos_rmw(QOS_MMC1_WR_CBASS_MAP(0), QOS_PVU_CTX(2), mask);

	/* DSS VID1, VID2, VIDL2 */
	cbass_qos_rmw(QOS_DSS0_DMA_CBASS_MAP(0), QOS_PVU_CTX(2), mask);
	cbass_qos_rmw(QOS_DSS0_DMA_CBASS_MAP(1), QOS_PVU_CTX(2), mask);
	cbass_qos_rmw(QOS_DSS0_DMA_CBASS_MAP(4), QOS_PVU_CTX(2), mask);
	cbass_qos_rmw(QOS_DSS0_DMA_CBASS_MAP(5), QOS_PVU_CTX(2), mask);
	cbass_qos_rmw(QOS_DSS0_DMA_CBASS_MAP(6), QOS_PVU_CTX(2), mask);
	cbass_qos_rmw(QOS_DSS0_DMA_CBASS_MAP(7), QOS_PVU_CTX(2), mask);

	/* GPU OS_id = 0 uses chanid = [0-3] */
	for (i = 0; i < 4; i++) {
		cbass_qos_rmw(QOS_GPU_M0_RD_CBASS_MAP(i), QOS_PVU_CTX(2), mask);
		cbass_qos_rmw(QOS_GPU_M0_WR_CBASS_MAP(i), QOS_PVU_CTX(2), mask);
		cbass_qos_rmw(QOS_GPU_M1_RD_CBASS_MAP(i), QOS_PVU_CTX(2), mask);
		cbass_qos_rmw(QOS_GPU_M1_WR_CBASS_MAP(i), QOS_PVU_CTX(2), mask);
	}

	/* D5520 chanid=[0-1] */
	for (i = 0; i < 2; i++) {
		cbass_qos_rmw(QOS_D5520_RD_CBASS_MAP(2), QOS_PVU_CTX(2), mask);
		cbass_qos_rmw(QOS_D5520_WR_CBASS_MAP(2), QOS_PVU_CTX(2), mask);
	}

	/* Initiators for inmate_cell, virtid=3 */
	/* MMC0 */
	cbass_qos_rmw(QOS_MMC0_RD_CBASS_MAP(0), QOS_PVU_CTX(3), mask);
	cbass_qos_rmw(QOS_MMC0_WR_CBASS_MAP(0), QOS_PVU_CTX(3), mask);

	/* DSS VIDL1 */
	cbass_qos_rmw(QOS_DSS0_DMA_CBASS_MAP(2), QOS_PVU_CTX(3), mask);
	cbass_qos_rmw(QOS_DSS0_DMA_CBASS_MAP(3), QOS_PVU_CTX(3), mask);

	/* GPU OS_id = 1 uses chanid = [4-7] */
	for (i = 4; i < 8; i++) {
		cbass_qos_rmw(QOS_GPU_M0_RD_CBASS_MAP(i), QOS_PVU_CTX(3), mask);
		cbass_qos_rmw(QOS_GPU_M0_WR_CBASS_MAP(i), QOS_PVU_CTX(3), mask);
		cbass_qos_rmw(QOS_GPU_M1_RD_CBASS_MAP(i), QOS_PVU_CTX(3), mask);
		cbass_qos_rmw(QOS_GPU_M1_WR_CBASS_MAP(i), QOS_PVU_CTX(3), mask);
	}
}

static void ctrl_mmr_unlock(void)
{
	/* Unlock all WKUP_CTRL_MMR0 module registers */
	mmr_unlock(WKUP_CTRL_MMR0_BASE, 0);
	mmr_unlock(WKUP_CTRL_MMR0_BASE, 1);
	mmr_unlock(WKUP_CTRL_MMR0_BASE, 2);
	mmr_unlock(WKUP_CTRL_MMR0_BASE, 3);
	mmr_unlock(WKUP_CTRL_MMR0_BASE, 4);
	mmr_unlock(WKUP_CTRL_MMR0_BASE, 6);
	mmr_unlock(WKUP_CTRL_MMR0_BASE, 7);

	/* Unlock all MCU_CTRL_MMR0 module registers */
	mmr_unlock(MCU_CTRL_MMR0_BASE, 0);
	mmr_unlock(MCU_CTRL_MMR0_BASE, 1);
	mmr_unlock(MCU_CTRL_MMR0_BASE, 2);
	mmr_unlock(MCU_CTRL_MMR0_BASE, 3);
	mmr_unlock(MCU_CTRL_MMR0_BASE, 4);

	/* Unlock all CTRL_MMR0 module registers */
	mmr_unlock(CTRL_MMR0_BASE, 0);
	mmr_unlock(CTRL_MMR0_BASE, 1);
	mmr_unlock(CTRL_MMR0_BASE, 2);
	mmr_unlock(CTRL_MMR0_BASE, 3);
	mmr_unlock(CTRL_MMR0_BASE, 5);
	if (soc_is_j721e())
		mmr_unlock(CTRL_MMR0_BASE, 6);
	mmr_unlock(CTRL_MMR0_BASE, 7);
}

#if defined(CONFIG_K3_LOAD_SYSFW)
void k3_mmc_stop_clock(void)
{
	if (spl_boot_device() == BOOT_DEVICE_MMC1) {
		struct mmc *mmc = find_mmc_device(0);

		if (!mmc)
			return;

		mmc->saved_clock = mmc->clock;
		mmc_set_clock(mmc, 0, true);
	}
}

void k3_mmc_restart_clock(void)
{
	if (spl_boot_device() == BOOT_DEVICE_MMC1) {
		struct mmc *mmc = find_mmc_device(0);

		if (!mmc)
			return;

		mmc_set_clock(mmc, mmc->saved_clock, false);
	}
}
#endif

#ifdef CONFIG_CPU_V7R
void setup_navss_nb(void)
{
	/* Map orderid 8-15 to VBUSM.C thread 2 (real-time traffic) */
	writel(2, NAVSS0_NBSS_NB0_CFG_NB_THREADMAP);
	writel(2, NAVSS0_NBSS_NB1_CFG_NB_THREADMAP);
}

void setup_dss_credentials(void)
{
	unsigned int channel, group;

	/* two master ports: dma and fbdc */
	/* two groups: SRAM and DDR */
	/* 10 channels: (pipe << 1) | is_second_buffer */

	/* master port 1 (dma) */

	for (group = 0; group < 2; ++group) {
		writel(0x76543210, QOS_DSS0_DMA_CBASS_GRP_MAP1(group));
		writel(0xfedcba98, QOS_DSS0_DMA_CBASS_GRP_MAP2(group));
	}

	for (channel = 0; channel < 10; ++channel) {
		u8 orderid;
		u8 atype = 0;

		orderid = 0xf - channel;

		writel((atype << 28) | (orderid << 4), QOS_DSS0_DMA_CBASS_MAP(channel));
	}

	/* master port 2 (fbdc) */

	for (group = 0; group < 2; ++group) {
		writel(0x76543210, QOS_DSS0_FBDC_CBASS_GRP_MAP1(group));
		writel(0xfedcba98, QOS_DSS0_FBDC_CBASS_GRP_MAP2(group));
	}

	for (channel = 0; channel < 10; ++channel) {
		u8 orderid;
		u8 atype = 0;

		orderid = 0xf - channel;

		writel((atype << 28) | (orderid << 4), QOS_DSS0_FBDC_CBASS_MAP(channel));
	}
}
#endif

/*
 * This uninitialized global variable would normal end up in the .bss section,
 * but the .bss is cleared between writing and reading this variable, so move
 * it to the .data section.
 */
u32 bootindex __attribute__((section(".data")));
static struct rom_extended_boot_data bootdata __section(.data);

static void store_boot_info_from_rom(void)
{
	bootindex = *(u32 *)(CONFIG_SYS_K3_BOOT_PARAM_TABLE_INDEX);
	memcpy(&bootdata, (uintptr_t *)ROM_ENTENDED_BOOT_DATA_INFO,
	       sizeof(struct rom_extended_boot_data));
}

void board_init_f(ulong dummy)
{
#if defined(CONFIG_CPU_V7R) && defined(CONFIG_K3_AVS0)
	int offset;
	u32 val, dflt = 0;
#endif
#if defined(CONFIG_K3_J721E_DDRSS) || defined(CONFIG_K3_LOAD_SYSFW) || \
	defined(CONFIG_ESM_K3) || defined(CONFIG_ESM_PMIC)
	struct udevice *dev;
	int ret;
#endif
	/*
	 * Cannot delay this further as there is a chance that
	 * K3_BOOT_PARAM_TABLE_INDEX can be over written by SPL MALLOC section.
	 */
	store_boot_info_from_rom();

	/* Make all control module registers accessible */
	ctrl_mmr_unlock();

#ifdef CONFIG_CPU_V7R
	disable_linefill_optimization();
	setup_k3_mpu_regions();

	if (soc_is_j721e()) {
#ifndef CONFIG_TI_SECURE_DEVICE
		setup_navss_nb();
		setup_dss_credentials();
		setup_initiator_credentials();
#endif
	}
#endif

	/* Init DM early */
	spl_early_init();

#ifdef CONFIG_K3_LOAD_SYSFW
	/*
	 * Process pinctrl for the serial0 a.k.a. MCU_UART0 module and continue
	 * regardless of the result of pinctrl. Do this without probing the
	 * device, but instead by searching the device that would request the
	 * given sequence number if probed. The UART will be used by the system
	 * firmware (SYSFW) image for various purposes and SYSFW depends on us
	 * to initialize its pin settings.
	 */
	ret = uclass_find_device_by_seq(UCLASS_SERIAL, 0, true, &dev);
	if (!ret)
		pinctrl_select_state(dev, "default");

	/*
	 * Load, start up, and configure system controller firmware. Provide
	 * the U-Boot console init function to the SYSFW post-PM configuration
	 * callback hook, effectively switching on (or over) the console
	 * output.
	 */
	k3_sysfw_loader(is_rom_loaded_sysfw(&bootdata),
			k3_mmc_stop_clock, k3_mmc_restart_clock);

#ifdef CONFIG_SPL_CLK_K3
	/*
	 * Force probe of clk_k3 driver here to ensure basic default clock
	 * configuration is always done.
	 */
	ret = uclass_get_device_by_driver(UCLASS_CLK,
					  DM_GET_DRIVER(ti_clk),
					  &dev);
	if (ret)
		panic("Failed to initialize clk-k3!\n");
#endif

	/* Prepare console output */
	preloader_console_init();

	/* Disable ROM configured firewalls right after loading sysfw */
#ifdef CONFIG_TI_SECURE_DEVICE
	remove_fwl_configs(cbass_hc_cfg0_fwls, ARRAY_SIZE(cbass_hc_cfg0_fwls));
	remove_fwl_configs(cbass_hc0_fwls, ARRAY_SIZE(cbass_hc0_fwls));
	remove_fwl_configs(cbass_rc_cfg0_fwls, ARRAY_SIZE(cbass_rc_cfg0_fwls));
	remove_fwl_configs(cbass_rc0_fwls, ARRAY_SIZE(cbass_rc0_fwls));
	remove_fwl_configs(infra_cbass0_fwls, ARRAY_SIZE(infra_cbass0_fwls));
	remove_fwl_configs(mcu_cbass0_fwls, ARRAY_SIZE(mcu_cbass0_fwls));
	remove_fwl_configs(wkup_cbass0_fwls, ARRAY_SIZE(wkup_cbass0_fwls));
#endif
#else
	/* Prepare console output */
	preloader_console_init();
#endif

	/* Output System Firmware version info */
	k3_sysfw_print_ver();

	/* Perform EEPROM-based board detection */
	if (IS_ENABLED(CONFIG_TI_I2C_BOARD_DETECT))
		do_board_detect();

#if defined(CONFIG_CPU_V7R) && defined(CONFIG_K3_AVS0)
	if (board_ti_k3_is("J721EX-PM1-SOM")) {
		offset = fdt_node_offset_by_compatible(gd->fdt_blob, -1,
						       "ti,am654-vtm");
		val = fdt_getprop_u32_default_node(gd->fdt_blob, offset, 0,
						   "som1-supply-2", dflt);
		do_fixup_by_compat_u32((void *)gd->fdt_blob, "ti,am654-vtm",
				       "vdd-supply-2", val, 0);
	}

	ret = uclass_get_device_by_driver(UCLASS_MISC, DM_GET_DRIVER(k3_avs),
					  &dev);
	if (ret)
		printf("AVS init failed: %d\n", ret);
#endif

#ifdef CONFIG_ESM_K3
	if (board_ti_k3_is("J721EX-PM2-SOM") ||
	    board_ti_k3_is("J7200X-PM1-SOM")) {
		ret = uclass_get_device_by_driver(UCLASS_MISC,
						  DM_GET_DRIVER(k3_esm), &dev);
		if (ret)
			printf("MISC init failed: %d\n", ret);
	}
#endif

#ifdef CONFIG_ESM_PMIC
	if (board_ti_k3_is("J721EX-PM2-SOM") ||
	    board_ti_k3_is("J7200X-PM1-SOM")) {
		ret = uclass_get_device_by_driver(UCLASS_MISC,
						  DM_GET_DRIVER(pmic_esm),
						  &dev);
		if (ret)
			printf("ESM PMIC init failed: %d\n", ret);
	}
#endif

#if defined(CONFIG_K3_J721E_DDRSS)
	ret = uclass_get_device(UCLASS_RAM, 0, &dev);
	if (ret)
		panic("DRAM init failed: %d\n", ret);
#endif
}

u32 spl_boot_mode(const u32 boot_device)
{
	switch (boot_device) {
	case BOOT_DEVICE_MMC1:
		return MMCSD_MODE_EMMCBOOT;
	case BOOT_DEVICE_MMC2:
		return MMCSD_MODE_FS;
	default:
		return MMCSD_MODE_RAW;
	}
}

static u32 __get_backup_bootmedia(u32 main_devstat)
{
	u32 bkup_boot = (main_devstat & MAIN_DEVSTAT_BKUP_BOOTMODE_MASK) >>
			MAIN_DEVSTAT_BKUP_BOOTMODE_SHIFT;

	switch (bkup_boot) {
	case BACKUP_BOOT_DEVICE_USB:
		return BOOT_DEVICE_DFU;
	case BACKUP_BOOT_DEVICE_UART:
		return BOOT_DEVICE_UART;
	case BACKUP_BOOT_DEVICE_ETHERNET:
		return BOOT_DEVICE_ETHERNET;
	case BACKUP_BOOT_DEVICE_MMC2:
	{
		u32 port = (main_devstat & MAIN_DEVSTAT_BKUP_MMC_PORT_MASK) >>
			    MAIN_DEVSTAT_BKUP_MMC_PORT_SHIFT;
		if (port == 0x0)
			return BOOT_DEVICE_MMC1;
		return BOOT_DEVICE_MMC2;
	}
	case BACKUP_BOOT_DEVICE_SPI:
		return BOOT_DEVICE_SPI;
	case BACKUP_BOOT_DEVICE_I2C:
		return BOOT_DEVICE_I2C;
	}

	return BOOT_DEVICE_RAM;
}

static u32 __get_primary_bootmedia(u32 main_devstat, u32 wkup_devstat)
{

	u32 bootmode = (wkup_devstat & WKUP_DEVSTAT_PRIMARY_BOOTMODE_MASK) >>
			WKUP_DEVSTAT_PRIMARY_BOOTMODE_SHIFT;

	bootmode |= (main_devstat & MAIN_DEVSTAT_BOOT_MODE_B_MASK) <<
			BOOT_MODE_B_SHIFT;

	if (bootmode == BOOT_DEVICE_OSPI || bootmode ==	BOOT_DEVICE_QSPI ||
	    bootmode == BOOT_DEVICE_XSPI)
		bootmode = BOOT_DEVICE_SPI;

	if (bootmode == BOOT_DEVICE_MMC2) {
		u32 port = (main_devstat &
			    MAIN_DEVSTAT_PRIM_BOOTMODE_MMC_PORT_MASK) >>
			   MAIN_DEVSTAT_PRIM_BOOTMODE_PORT_SHIFT;
		if (port == 0x0)
			bootmode = BOOT_DEVICE_MMC1;
	}

	return bootmode;
}

u32 spl_boot_device(void)
{
	u32 wkup_devstat = readl(CTRLMMR_WKUP_DEVSTAT);
	u32 main_devstat;

	if (wkup_devstat & WKUP_DEVSTAT_MCU_OMLY_MASK) {
		printf("ERROR: MCU only boot is not yet supported\n");
		return BOOT_DEVICE_RAM;
	}

	/* MAIN CTRL MMR can only be read if MCU ONLY is 0 */
	main_devstat = readl(CTRLMMR_MAIN_DEVSTAT);

	if (bootindex == K3_PRIMARY_BOOTMODE)
		return __get_primary_bootmedia(main_devstat, wkup_devstat);
	else
		return __get_backup_bootmedia(main_devstat);
}
#endif

#ifdef CONFIG_SYS_K3_SPL_ATF

#define J721E_DEV_A72SS0_CORE0			202
#define J721E_DEV_MCU_RTI0			262
#define J721E_DEV_MCU_RTI1			263
#define J721E_DEV_MCU_ARMSS0_CPU0		250
#define J721E_DEV_MCU_ARMSS0_CPU1		251

void release_resources_for_core_shutdown(void)
{
	struct ti_sci_handle *ti_sci;
	struct ti_sci_dev_ops *dev_ops;
	struct ti_sci_proc_ops *proc_ops;
	int ret;
	u32 i;

	const u32 put_device_ids[] = {
		J721E_DEV_MCU_RTI0,
		J721E_DEV_MCU_RTI1,
	};

	const u32 get_device_ids[] = {
		J721E_DEV_A72SS0_CORE0
	};

	ti_sci = get_ti_sci_handle();
	dev_ops = &ti_sci->ops.dev_ops;
	proc_ops = &ti_sci->ops.proc_ops;

	/* Iterate through list of devices to get (enable) */
	for (i = 0; i < ARRAY_SIZE(get_device_ids); i++) {
		u32 id = get_device_ids[i];

		ret = dev_ops->get_device(ti_sci, id);
		if (ret)
			panic("Failed to get device %u (%d)\n", id, ret);
	}

	/* Iterate through list of devices to put (shutdown) */
	for (i = 0; i < ARRAY_SIZE(put_device_ids); i++) {
		u32 id = put_device_ids[i];

		ret = dev_ops->put_device(ti_sci, id);
		if (ret)
			panic("Failed to put device %u (%d)\n", id, ret);
	}

	const u32 put_core_ids[] = {
		J721E_DEV_MCU_ARMSS0_CPU1,
		J721E_DEV_MCU_ARMSS0_CPU0,	/* Handle CPU0 after CPU1 */
	};

	/* Iterate through list of cores to put (shutdown) */
	for (i = 0; i < ARRAY_SIZE(put_core_ids); i++) {
		u32 id = put_core_ids[i];

		/*
		 * Queue up the core shutdown request. Note that this call
		 * needs to be followed up by an actual invocation of an WFE
		 * or WFI CPU instruction.
		 */
		ret = proc_ops->proc_shutdown_no_wait(ti_sci, id);
		if (ret)
			panic("Failed sending core %u shutdown message (%d)\n",
			      id, ret);
	}
}
#endif

#ifdef CONFIG_SYS_K3_SPL_ATF
void start_non_linux_remote_cores(void)
{
	int size = 0, ret;
	u32 loadaddr = 0;

	/*
	 * Skip booting of Main R5FSS Core0 in R5 SPL temporarily. Re-enable
	 * in a future release after the R5 SPL re-architecture is sorted out.
	 */
	if (soc_is_j721e() || soc_is_j7200())
		return;

	size = load_firmware("mainr5f0_0fwname", "mainr5f0_0loadaddr",
			     &loadaddr);
	if (size <= 0)
		goto err_load;

	/* assuming remoteproc 2 is aliased for the needed remotecore */
	ret = rproc_load(2, loadaddr, size);
	if (ret) {
		printf("Firmware failed to start on rproc (%d)\n", ret);
		goto err_load;
	}

	ret = rproc_start(2);
	if (ret) {
		printf("Firmware init failed on rproc (%d)\n", ret);
		goto err_load;
	}

	printf("Remoteproc 2 started successfully\n");

	return;

err_load:
	rproc_reset(2);
}
#endif
