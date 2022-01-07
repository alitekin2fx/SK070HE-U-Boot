/**********************************************************************
* Copyright (C) 2012-2020 Cadence Design Systems, Inc.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* 1. Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its
* contributors may be used to endorse or promote products derived from
* this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
**********************************************************************
* WARNING: This file is auto-generated using api-generator utility.
*          api-generator: 12.02.13bb8d5
*          Do not edit it manually.
**********************************************************************
* Cadence Core Driver for LPDDR4_16Bit.
**********************************************************************/

#ifndef LPDDR4_IF_H
#define LPDDR4_IF_H

#include "cdn_stdtypes.h"
#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup ConfigInfo  Configuration and Hardware Operation Information
 *  The following definitions specify the driver operation environment that
 *  is defined by hardware configuration or client code. These defines are
 *  located in the header file of the core driver.
 *  @{
 */

/**********************************************************************
* Defines
**********************************************************************/
/** Number of chip-selects */
#define LPDDR4_MAX_CS (2U)

/** Number of accessible registers for controller. */
#define LPDDR4_CTL_REG_COUNT (423U)

/** Number of accessible registers for PHY Independent Module. */
#define LPDDR4_PHY_INDEP_REG_COUNT (345U)

/** Number of accessible registers for PHY. */
#define LPDDR4_PHY_REG_COUNT (1406U)

/**
 *  @}
 */

/** @defgroup DataStructure Dynamic Data Structures
 *  This section defines the data structures used by the driver to provide
 *  hardware information, modification and dynamic operation of the driver.
 *  These data structures are defined in the header file of the core driver
 *  and utilized by the API.
 *  @{
 */

/**********************************************************************
* Forward declarations
**********************************************************************/
typedef struct lpddr4_config_s lpddr4_config;
typedef struct lpddr4_privatedata_s lpddr4_privatedata;
typedef struct lpddr4_debuginfo_s lpddr4_debuginfo;
typedef struct lpddr4_fspmoderegs_s lpddr4_fspmoderegs;

/**********************************************************************
* Enumerations
**********************************************************************/
/** This is used to indicate whether the Controller, PHY, or PHY Independent module is addressed. */
typedef enum {
	LPDDR4_CTL_REGS		= 0U,
	LPDDR4_PHY_REGS		= 1U,
	LPDDR4_PHY_INDEP_REGS	= 2U
} lpddr4_regblock;

/** Controller status or error interrupts. */
typedef enum {
	LPDDR4_TIMEOUT_ZQ_CAL_INIT		= 0U,
	LPDDR4_TIMEOUT_ZQ_CALLATCH		= 1U,
	LPDDR4_TIMEOUT_ZQ_CALSTART		= 2U,
	LPDDR4_TIMEOUT_MRR_TEMP			= 3U,
	LPDDR4_TIMEOUT_DQS_OSC_REQ		= 4U,
	LPDDR4_TIMEOUT_DFI_UPDATE		= 5U,
	LPDDR4_TIMEOUT_LP_WAKEUP		= 6U,
	LPDDR4_TIMEOUT_AUTO_REFRESH_MAX		= 7U,
	LPDDR4_ECC_ERROR			= 8U,
	LPDDR4_LP_DONE				= 9U,
	LPDDR4_LP_TIMEOUT			= 10U,
	LPDDR4_PORT_TIMEOUT			= 11U,
	LPDDR4_RFIFO_TIMEOUT			= 12U,
	LPDDR4_TRAINING_ZQ_STATUS		= 13U,
	LPDDR4_TRAINING_DQS_OSC_DONE		= 14U,
	LPDDR4_TRAINING_DQS_OSC_UPDATE_DONE	= 15U,
	LPDDR4_TRAINING_DQS_OSC_OVERFLOW	= 16U,
	LPDDR4_TRAINING_DQS_OSC_VAR_OUT		= 17U,
	LPDDR4_USERIF_OUTSIDE_MEM_ACCESS	= 18U,
	LPDDR4_USERIF_MULTI_OUTSIDE_MEM_ACCESS	= 19U,
	LPDDR4_USERIF_PORT_CMD_ERROR		= 20U,
	LPDDR4_USERIF_WRAP			= 21U,
	LPDDR4_USERIF_INVAL_SETTING		= 22U,
	LPDDR4_MISC_MRR_TRAFFIC			= 23U,
	LPDDR4_MISC_SW_REQ_MODE			= 24U,
	LPDDR4_MISC_CHANGE_TEMP_REFRESH		= 25U,
	LPDDR4_MISC_TEMP_ALERT			= 26U,
	LPDDR4_MISC_REFRESH_STATUS		= 27U,
	LPDDR4_BIST_DONE			= 28U,
	LPDDR4_CRC				= 29U,
	LPDDR4_DFI_UPDATE_ERROR			= 30U,
	LPDDR4_DFI_PHY_ERROR			= 31U,
	LPDDR4_DFI_BUS_ERROR			= 32U,
	LPDDR4_DFI_STATE_CHANGE			= 33U,
	LPDDR4_DFI_DLL_SYNC_DONE		= 34U,
	LPDDR4_DFI_TIMEOUT			= 35U,
	LPDDR4_DIMM				= 36U,
	LPDDR4_FREQ_DFS_REQ_HW_IGNORE		= 37U,
	LPDDR4_FREQ_DFS_HW_TERMINATE		= 38U,
	LPDDR4_FREQ_DFS_HW_DONE			= 39U,
	LPDDR4_FREQ_DFS_REQ_SW_IGNORE		= 40U,
	LPDDR4_FREQ_DFS_SW_TERMINATE		= 41U,
	LPDDR4_FREQ_DFS_SW_DONE			= 42U,
	LPDDR4_INIT_MEM_RESET_DONE		= 43U,
	LPDDR4_INIT_MC_DONE			= 44U,
	LPDDR4_INIT_POWER_ON_STATE		= 45U,
	LPDDR4_MODE_MRR_ERROR			= 46U,
	LPDDR4_MODE_READ_REQ_DONE		= 47U,
	LPDDR4_MODE_WRITE_REQ_DONE		= 48U,
	LPDDR4_PARITY_ERROR			= 49U,
	LPDDR4_LOR_BIT				= 50U
} lpddr4_ctlinterrupt;

/** PHY Independent Module status or error interrupts. */
typedef enum {
	LPDDR4_PHY_INDEP_INIT_DONE_BIT			= 0U,
	LPDDR4_PHY_INDEP_CA_PARITY_ERR_BIT		= 1U,
	LPDDR4_PHY_INDEP_RDLVL_ERROR_BIT		= 2U,
	LPDDR4_PHY_INDEP_RDLVL_GATE_ERROR_BIT		= 3U,
	LPDDR4_PHY_INDEP_WRLVL_ERROR_BIT		= 4U,
	LPDDR4_PHY_INDEP_CALVL_ERROR_BIT		= 5U,
	LPDDR4_PHY_INDEP_WDQLVL_ERROR_BIT		= 6U,
	LPDDR4_PHY_INDEP_UPDATE_ERROR_BIT		= 7U,
	LPDDR4_PHY_INDEP_RDLVL_REQ_BIT			= 8U,
	LPDDR4_PHY_INDEP_RDLVL_GATE_REQ_BIT		= 9U,
	LPDDR4_PHY_INDEP_WRLVL_REQ_BIT			= 10U,
	LPDDR4_PHY_INDEP_CALVL_REQ_BIT			= 11U,
	LPDDR4_PHY_INDEP_WDQLVL_REQ_BIT			= 12U,
	LPDDR4_PHY_INDEP_LVL_DONE_BIT			= 13U,
	LPDDR4_PHY_INDEP_BIST_DONE_BIT			= 14U,
	LPDDR4_PHY_INDEP_TDFI_INIT_TIME_OUT_BIT		= 15U,
	LPDDR4_PHY_INDEP_DLL_LOCK_STATE_CHANGE_BIT	= 16U,
	LPDDR4_PHY_INDEP_MEM_RST_VALID_BIT		= 17U,
	LPDDR4_PHY_INDEP_ZQ_STATUS_BIT			= 18U,
	LPDDR4_PHY_INDEP_PERIPHERAL_MRR_DONE_BIT	= 19U,
	LPDDR4_PHY_INDEP_WRITE_NODEREG_DONE_BIT		= 20U,
	LPDDR4_PHY_INDEP_FREQ_CHANGE_DONE_BIT		= 21U,
	LPDDR4_PHY_INDEP_RDLVL_GATE_DONE_BIT		= 22U,
	LPDDR4_PHY_INDEP_RDLVL_DONE_BIT			= 23U,
	LPDDR4_PHY_INDEP_WRLVL_DONE_BIT			= 24U,
	LPDDR4_PHY_INDEP_CALVL_DONE__BIT		= 25U,
	LPDDR4_PHY_INDEP_WDQLVL_DONE_BIT		= 26U,
	LPDDR4_PHY_INDEP_VREF_DONE_BIT			= 27U,
	LPDDR4_PHY_INDEP_ANY_VALID_BIT			= 28U
} lpddr4_phyindepinterrupt;

/** List of informations and warnings from driver. */
typedef enum {
	LPDDR4_DRV_NONE			= 0U,
	LPDDR4_DRV_SOC_PLL_UPDATE	= 1U
} lpddr4_infotype;

/** Low power interface wake up timing parameters */
typedef enum {
	LPDDR4_LPI_PD_WAKEUP_FN				= 0U,
	LPDDR4_LPI_SR_SHORT_WAKEUP_FN			= 1U,
	LPDDR4_LPI_SR_LONG_WAKEUP_FN			= 2U,
	LPDDR4_LPI_SR_LONG_MCCLK_GATE_WAKEUP_FN		= 3U,
	LPDDR4_LPI_SRPD_SHORT_WAKEUP_FN			= 4U,
	LPDDR4_LPI_SRPD_LONG_WAKEUP_FN			= 5U,
	LPDDR4_LPI_SRPD_LONG_MCCLK_GATE_WAKEUP_FN	= 6U
} lpddr4_lpiwakeupparam;

/** Half Datapath mode setting */
typedef enum {
	LPDDR4_REDUC_ON		= 0U,
	LPDDR4_REDUC_OFF	= 1U
} lpddr4_reducmode;

/** Data Byte Inversion mode setting */
typedef enum {
	LPDDR4_DBI_RD_ON	= 0U,
	LPDDR4_DBI_RD_OFF	= 1U,
	LPDDR4_DBI_WR_ON	= 2U,
	LPDDR4_DBI_WR_OFF	= 3U
} lpddr4_dbimode;

/** Controller Frequency Set Point number  */
typedef enum {
	LPDDR4_FSP_0	= 0U,
	LPDDR4_FSP_1	= 1U,
	LPDDR4_FSP_2	= 2U
} lpddr4_ctlfspnum;

/**********************************************************************
* Callbacks
**********************************************************************/
/**
 * Reports informations and warnings that need to be communicated.
 * Params:
 * pd - driver state info specific to this instance.
 * infoType - Type of information.
 */
typedef void (*lpddr4_infocallback)(const lpddr4_privatedata* pd, lpddr4_infotype infoType);

/**
 * Reports interrupts received by the controller.
 * Params:
 * pd - driver state info specific to this instance.
 * ctlInterrupt - Interrupt raised
 * chipSelect - Chip for which interrupt raised
 */
typedef void (*lpddr4_ctlcallback)(const lpddr4_privatedata* pd, lpddr4_ctlinterrupt ctlInterrupt, uint8_t chipSelect);

/**
 * Reports interrupts received by the PHY Independent Module.
 * Params:
 * privateData - driver state info specific to this instance.
 * phyIndepInterrupt - Interrupt raised
 * chipSelect - Chip for which interrupt raised
 */
typedef void (*lpddr4_phyindepcallback)(const lpddr4_privatedata* pd, lpddr4_phyindepinterrupt phyIndepInterrupt, uint8_t chipSelect);

/**
 *  @}
 */

/** @defgroup DriverFunctionAPI Driver Function API
 *  Prototypes for the driver API functions. The user application can link statically to the
 *  necessary API functions and call them directly.
 *  @{
 */

/**********************************************************************
* API methods
**********************************************************************/

/**
 * Checks configuration object.
 * @param[in] config Driver/hardware configuration required.
 * @param[out] configSize Size of memory allocations required.
 * @return CDN_EOK on success (requirements structure filled).
 * @return CDN_ENOTSUP if configuration cannot be supported due to driver/hardware constraints.
 */
uint32_t lpddr4_probe(const lpddr4_config* config, uint16_t* configSize);

/**
 * Init function to be called after LPDDR4_probe() to set up the
 * driver configuration.  Memory should be allocated for drv_data
 * (using the size determined using LPDDR4_probe)  before calling this
 * API.  init_settings should be initialised with base addresses for
 * PHY Indepenent Module, Controller and PHY before calling this
 * function.  If callbacks are required for interrupt handling, these
 * should also be configured in init_settings.
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] cfg Specifies driver/hardware configuration.
 * @return CDN_EOK on success
 * @return CDN_EINVAL if illegal/inconsistent values in cfg.
 * @return CDN_ENOTSUP if hardware has an inconsistent configuration or doesn't support feature(s) required by 'config' parameters.
 */
uint32_t lpddr4_init(lpddr4_privatedata* pd, const lpddr4_config* cfg);

/**
 * Start the driver.
 * @param[in] pd Driver state info specific to this instance.
 */
uint32_t lpddr4_start(const lpddr4_privatedata* pd);

/**
 * Read a register from the controller, PHY or PHY Independent Module
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] cpp Indicates whether controller, PHY or PHY Independent Module register
 * @param[in] regOffset Register offset
 * @param[out] regvalue Register value read
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if regOffset if out of range or regvalue is NULL
 */
uint32_t lpddr4_readreg(const lpddr4_privatedata* pd, lpddr4_regblock cpp, uint32_t regOffset, uint32_t* regvalue);

/**
 * Write a register in the controller, PHY or PHY Independent Module
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] cpp Indicates whether controller, PHY or PHY Independent Module register
 * @param[in] regOffset Register offset
 * @param[in] regvalue Register value to be written
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if regOffset is out of range or regvalue is NULL
 */
uint32_t lpddr4_writereg(const lpddr4_privatedata* pd, lpddr4_regblock cpp, uint32_t regOffset, uint32_t regvalue);

/**
 * Read a memory mode register from DRAM
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] readModeRegVal Value to set in 'read_modereg' parameter.
 * @param[out] mmrValue Value which is read from memory mode register(mmr) for all devices.
 * @param[out] mmrStatus Status of mode register read(mrr) instruction.
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if regNumber is out of range or regvalue is NULL
 */
uint32_t lpddr4_getmmrregister(const lpddr4_privatedata* pd, uint32_t readModeRegVal, uint64_t* mmrValue, uint8_t* mmrStatus);

/**
 * Write a memory mode register in DRAM
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] writeModeRegVal Value to set in 'write_modereg' parameter.
 * @param[out] mrwStatus Status of mode register write(mrw) instruction.
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if regNumber is out of range or regvalue is NULL
 */
uint32_t lpddr4_setmmrregister(const lpddr4_privatedata* pd, uint32_t writeModeRegVal, uint8_t* mrwStatus);

/**
 * Write a set of initialisation values to the controller registers
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] regvalues Register values to be written
 * @param[in] regNum Register Number to be written
 * @param[in] regCount Count of registers to be written.
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if regvalues is NULL
 */
uint32_t lpddr4_writectlconfig(const lpddr4_privatedata* pd, uint32_t regvalues[], uint16_t regNum[], uint16_t regCount);

/**
 * Write a set of initialisation values to the PHY registers
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] regvalues Register values to be written
 * @param[in] regNum Register Number to be written
 * @param[in] regCount Count of registers to be written.
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if regvalues is NULL
 */
uint32_t lpddr4_writephyconfig(const lpddr4_privatedata* pd, uint32_t regvalues[], uint16_t regNum[], uint16_t regCount);

/**
 * Write a set of initialisation values to the PHY Independent Module
 * registers
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] regvalues Register values to be written
 * @param[in] regNum Register Number to be written
 * @param[in] regCount Count of registers to be written.
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if regvalues is NULL
 */
uint32_t lpddr4_writephyindepconfig(const lpddr4_privatedata* pd, uint32_t regvalues[], uint16_t regNum[], uint16_t regCount);

/**
 * Read values of the controller registers in bulk and store in
 * memory.
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] regvalues Pointer to feedback the read values.
 * @param[in] regNum Register Number to be read
 * @param[in] regCount Count of registers to be read.
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if regvalues is NULL
 */
uint32_t lpddr4_readctlconfig(const lpddr4_privatedata* pd, uint32_t regvalues[], uint16_t regNum[], uint16_t regCount);

/**
 * Read the values of the PHY module registers in bulk and store in
 * memory.
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] regvalues Pointer to feedback the read values.
 * @param[in] regNum Register Number to be read
 * @param[in] regCount Count of registers to be read.
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if regvalues is NULL
 */
uint32_t lpddr4_readphyconfig(const lpddr4_privatedata* pd, uint32_t regvalues[], uint16_t regNum[], uint16_t regCount);

/**
 * Read the values of the PHY Independent module registers in bulk and
 * store in memory.
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] regvalues Pointer to feedback the read values.
 * @param[in] regNum Register Number to be read
 * @param[in] regCount Count of registers to be read.
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if regvalues is NULL
 */
uint32_t lpddr4_readphyindepconfig(const lpddr4_privatedata* pd, uint32_t regvalues[], uint16_t regNum[], uint16_t regCount);

/**
 * Read the current interrupt mask for the controller
 * @param[in] pd Driver state info specific to this instance.
 * @param[out] mask Value of interrupt mask
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if mask pointer is NULL
 */
uint32_t lpddr4_getctlinterruptmask(const lpddr4_privatedata* pd, uint64_t* mask);

/**
 * Sets the interrupt mask for the controller
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] mask Value of interrupt mask to be written
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if mask pointer is NULL
 */
uint32_t lpddr4_setctlinterruptmask(const lpddr4_privatedata* pd, const uint64_t* mask);

/**
 * Check whether a specific controller interrupt is active
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] intr Interrupt to be checked
 * @param[out] irqstatus Status of the interrupt, TRUE if active
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if intr is not valid
 */
uint32_t lpddr4_checkctlinterrupt(const lpddr4_privatedata* pd, lpddr4_ctlinterrupt intr, bool* irqstatus);

/**
 * Acknowledge  a specific controller interrupt
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] intr Interrupt to be acknowledged
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if intr is not valid
 */
uint32_t lpddr4_ackctlinterrupt(const lpddr4_privatedata* pd, lpddr4_ctlinterrupt intr);

/**
 * Read the current interrupt mask for the PHY Independent Module
 * @param[in] pd Driver state info specific to this instance.
 * @param[out] mask Value of interrupt mask
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if mask pointer is NULL
 */
uint32_t lpddr4_getphyindepinterruptmask(const lpddr4_privatedata* pd, uint32_t* mask);

/**
 * Sets the interrupt mask for the PHY Independent Module
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] mask Value of interrupt mask to be written
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if mask pointer is NULL
 */
uint32_t lpddr4_setphyindepinterruptmask(const lpddr4_privatedata* pd, const uint32_t* mask);

/**
 * Check whether a specific PHY Independent Module interrupt is active
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] intr Interrupt to be checked
 * @param[out] irqstatus Status of the interrupt, TRUE if active
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if intr is not valid
 */
uint32_t lpddr4_checkphyindepinterrupt(const lpddr4_privatedata* pd, lpddr4_phyindepinterrupt intr, bool* irqstatus);

/**
 * Acknowledge  a specific PHY Independent Module interrupt
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] intr Interrupt to be acknowledged
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if intr is not valid
 */
uint32_t lpddr4_ackphyindepinterrupt(const lpddr4_privatedata* pd, lpddr4_phyindepinterrupt intr);

/**
 * Retrieve status information after a failed init.  The
 * DebugStructInfo will be filled  in with error codes which can be
 * referenced against the driver documentation for further details.
 * @param[in] pd Driver state info specific to this instance.
 * @param[out] debugInfo status
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if debugInfo is NULL
 */
uint32_t lpddr4_getdebuginitinfo(const lpddr4_privatedata* pd, lpddr4_debuginfo* debugInfo);

/**
 * Get the current value of Low power Interface wake up time.
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] lpiWakeUpParam LPI timing parameter
 * @param[in] fspNum Frequency copy
 * @param[out] cycles Timing value(in cycles)
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if powerMode is NULL
 */
uint32_t lpddr4_getlpiwakeuptime(const lpddr4_privatedata* pd, const lpddr4_lpiwakeupparam* lpiWakeUpParam, const lpddr4_ctlfspnum* fspNum, uint32_t* cycles);

/**
 * Set the current value of Low power Interface wake up time.
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] lpiWakeUpParam LPI timing parameter
 * @param[in] fspNum Frequency copy
 * @param[in] cycles Timing value(in cycles)
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if powerMode is NULL
 */
uint32_t lpddr4_setlpiwakeuptime(const lpddr4_privatedata* pd, const lpddr4_lpiwakeupparam* lpiWakeUpParam, const lpddr4_ctlfspnum* fspNum, const uint32_t* cycles);

/**
 * Get the current value for the Half Datapath option
 * @param[in] pd Driver state info specific to this instance.
 * @param[out] mode Half Datapath setting
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if mode is NULL
 */
uint32_t lpddr4_getreducmode(const lpddr4_privatedata* pd, lpddr4_reducmode* mode);

/**
 * Set the value for the Half Datapath option.  This API must be
 * called before startup of memory.
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] mode Half Datapath setting
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if mode is NULL
 */
uint32_t lpddr4_setreducmode(const lpddr4_privatedata* pd, const lpddr4_reducmode* mode);

/**
 * Get the current value for Data Bus Inversion setting.  This will be
 * compared with the   current DRAM setting using the MR3 register.
 * @param[in] pd Driver state info specific to this instance.
 * @param[out] on_off DBI read value
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if on_off is NULL
 */
uint32_t lpddr4_getdbireadmode(const lpddr4_privatedata* pd, bool* on_off);

/**
 * Get the current value for Data Bus Inversion setting.  This will be
 * compared with the   current DRAM setting using the MR3 register.
 * @param[in] pd Driver state info specific to this instance.
 * @param[out] on_off DBI write value
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if on_off is NULL
 */
uint32_t lpddr4_getdbiwritemode(const lpddr4_privatedata* pd, bool* on_off);

/**
 * Set the mode for Data Bus Inversion. This will also be set in DRAM
 * using the MR3   controller register. This API must be called before
 * startup of memory.
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] mode status
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if mode is NULL
 */
uint32_t lpddr4_setdbimode(const lpddr4_privatedata* pd, const lpddr4_dbimode* mode);

/**
 * Get the current value for the refresh rate (reading Refresh per
 * command timing).
 * @param[in] pd Driver state info specific to this instance.
 * @param[out] fspNum Frequency set number
 * @param[in] tref Refresh rate (in cycles)
 * @param[out] tras_max Maximum row active time (in cycles)
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if tras_max is NULL
 */
uint32_t lpddr4_getrefreshrate(const lpddr4_privatedata* pd, const lpddr4_ctlfspnum* fspNum, uint32_t* tref, uint32_t* tras_max);

/**
 * Set the refresh rate (writing Refresh per command timing).
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] fspNum Frequency set number
 * @param[in] tref Refresh rate (in cycles)
 * @param[in] tras_max Maximum row active time (in cycles)
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if tras_max is NULL
 */
uint32_t lpddr4_setrefreshrate(const lpddr4_privatedata* pd, const lpddr4_ctlfspnum* fspNum, const uint32_t* tref, const uint32_t* tras_max);

/**
 * Handle Refreshing per chip select
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] trefInterval status
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if pd is NULL
 */
uint32_t lpddr4_refreshperchipselect(const lpddr4_privatedata* pd, const uint32_t trefInterval);

/**
 *  @}
 */

#ifdef __cplusplus
}
#endif

#endif  /* LPDDR4_IF_H */
