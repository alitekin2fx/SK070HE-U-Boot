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

/* parasoft-begin-suppress METRICS-18-3 "Follow the Cyclomatic Complexity limit of 10" */
/* parasoft-begin-suppress METRICS-36-3 "A function should not be called from more than 5 different functions" */
/* parasoft-begin-suppress METRICS-39-3 "The value of VOCF metric for a function should not be higher than 4" */
/* parasoft-begin-suppress METRICS-41-3 "Number of blocks of comments per statement" */

/**
 * This file contains sanity API functions. The purpose of sanity functions
 * is to check input parameters validity. They take the same parameters as
 * original API functions and return 0 on success or CDN_EINVAL on wrong parameter
 * value(s).
 */

#ifndef LPDDR4_SANITY_H
#define LPDDR4_SANITY_H

#include "cdn_errno.h"
#include "cdn_stdtypes.h"
#include "lpddr4_if.h"
#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t lpddr4_configsf(const lpddr4_config *obj);
static inline uint32_t lpddr4_privatedatasf(const lpddr4_privatedata *obj);

static inline uint32_t lpddr4_sanityfunction1(const lpddr4_config* config, const uint16_t* configSize);
static inline uint32_t lpddr4_sanityfunction2(const lpddr4_privatedata* pd, const lpddr4_config* cfg);
static inline uint32_t lpddr4_sanityfunction3(const lpddr4_privatedata* pd);
static inline uint32_t lpddr4_sanityfunction4(const lpddr4_privatedata* pd, const lpddr4_regblock cpp, const uint32_t* regvalue);
static inline uint32_t lpddr4_sanityfunction5(const lpddr4_privatedata* pd, const lpddr4_regblock cpp);
static inline uint32_t lpddr4_sanityfunction6(const lpddr4_privatedata* pd, const uint64_t* mmrValue, const uint8_t* mmrStatus);
static inline uint32_t lpddr4_sanityfunction7(const lpddr4_privatedata* pd, const uint8_t* mrwStatus);
static inline uint32_t lpddr4_sanityfunction14(const lpddr4_privatedata* pd, const uint64_t* mask);
static inline uint32_t lpddr4_sanityfunction15(const lpddr4_privatedata* pd, const uint64_t* mask);
static inline uint32_t lpddr4_sanityfunction16(const lpddr4_privatedata* pd, const lpddr4_ctlinterrupt intr, const bool* irqstatus);
static inline uint32_t lpddr4_sanityfunction17(const lpddr4_privatedata* pd, const lpddr4_ctlinterrupt intr);
static inline uint32_t lpddr4_sanityfunction18(const lpddr4_privatedata* pd, const uint32_t* mask);
static inline uint32_t lpddr4_sanityfunction20(const lpddr4_privatedata* pd, const lpddr4_phyindepinterrupt intr, const bool* irqstatus);
static inline uint32_t lpddr4_sanityfunction21(const lpddr4_privatedata* pd, const lpddr4_phyindepinterrupt intr);
static inline uint32_t lpddr4_sanityfunction22(const lpddr4_privatedata* pd, const lpddr4_debuginfo* debugInfo);
static inline uint32_t lpddr4_sanityfunction23(const lpddr4_privatedata* pd, const lpddr4_lpiwakeupparam* lpiWakeUpParam, const lpddr4_ctlfspnum* fspNum, const uint32_t* cycles);
static inline uint32_t lpddr4_sanityfunction25(const lpddr4_privatedata* pd, const lpddr4_reducmode* mode);
static inline uint32_t lpddr4_sanityfunction26(const lpddr4_privatedata* pd, const lpddr4_reducmode* mode);
static inline uint32_t lpddr4_sanityfunction27(const lpddr4_privatedata* pd, const bool* on_off);
static inline uint32_t lpddr4_sanityfunction29(const lpddr4_privatedata* pd, const lpddr4_dbimode* mode);
static inline uint32_t lpddr4_sanityfunction30(const lpddr4_privatedata* pd, const lpddr4_ctlfspnum* fspNum, const uint32_t* tref, const uint32_t* tras_max);
static inline uint32_t lpddr4_sanityfunction31(const lpddr4_privatedata* pd, const lpddr4_ctlfspnum* fspNum, const uint32_t* tref, const uint32_t* tras_max);

#define lpddr4_probeSF lpddr4_sanityfunction1
#define lpddr4_initSF lpddr4_sanityfunction2
#define lpddr4_startSF lpddr4_sanityfunction3
#define lpddr4_readregSF lpddr4_sanityfunction4
#define lpddr4_writeregSF lpddr4_sanityfunction5
#define lpddr4_getmmrregisterSF lpddr4_sanityfunction6
#define lpddr4_setmmrregisterSF lpddr4_sanityfunction7
#define lpddr4_writectlconfigSF lpddr4_sanityfunction3
#define lpddr4_writephyconfigSF lpddr4_sanityfunction3
#define lpddr4_writephyindepconfigSF lpddr4_sanityfunction3
#define lpddr4_readctlconfigSF lpddr4_sanityfunction3
#define lpddr4_readphyconfigSF lpddr4_sanityfunction3
#define lpddr4_readphyindepconfigSF lpddr4_sanityfunction3
#define lpddr4_getctlinterruptmaskSF lpddr4_sanityfunction14
#define lpddr4_setctlinterruptmaskSF lpddr4_sanityfunction15
#define lpddr4_checkctlinterruptSF lpddr4_sanityfunction16
#define lpddr4_ackctlinterruptSF lpddr4_sanityfunction17
#define LPDDR4_GetPhyIndepInterruptMSF lpddr4_sanityfunction18
#define LPDDR4_SetPhyIndepInterruptMSF lpddr4_sanityfunction18
#define LPDDR4_CheckPhyIndepInterrupSF lpddr4_sanityfunction20
#define lpddr4_ackphyindepinterruptSF lpddr4_sanityfunction21
#define lpddr4_getdebuginitinfoSF lpddr4_sanityfunction22
#define lpddr4_getlpiwakeuptimeSF lpddr4_sanityfunction23
#define lpddr4_setlpiwakeuptimeSF lpddr4_sanityfunction23
#define lpddr4_getreducmodeSF lpddr4_sanityfunction25
#define lpddr4_setreducmodeSF lpddr4_sanityfunction26
#define lpddr4_getdbireadmodeSF lpddr4_sanityfunction27
#define lpddr4_getdbiwritemodeSF lpddr4_sanityfunction27
#define lpddr4_setdbimodeSF lpddr4_sanityfunction29
#define lpddr4_getrefreshrateSF lpddr4_sanityfunction30
#define lpddr4_setrefreshrateSF lpddr4_sanityfunction31
#define lpddr4_refreshperchipselectSF lpddr4_sanityfunction3

/**
 * Function to validate struct Config
 *
 * @param[in] obj pointer to struct to be verified
 * @returns 0 for valid
 * @returns CDN_EINVAL for invalid
 */
static inline uint32_t lpddr4_configsf(const lpddr4_config *obj)
{
	uint32_t ret = 0;

	if (obj == NULL)
		ret = CDN_EINVAL;

	return ret;
}

/**
 * Function to validate struct PrivateData
 *
 * @param[in] obj pointer to struct to be verified
 * @returns 0 for valid
 * @returns CDN_EINVAL for invalid
 */
static inline uint32_t lpddr4_privatedatasf(const lpddr4_privatedata *obj)
{
	uint32_t ret = 0;

	if (obj == NULL)
		ret = CDN_EINVAL;

	return ret;
}

/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] config Driver/hardware configuration required.
 * @param[out] configSize Size of memory allocations required.
 * @return 0 success
 * @return CDN_EINVAL invalid parameters
 */
static inline uint32_t lpddr4_sanityfunction1(const lpddr4_config* config, const uint16_t* configSize)
{
	/* Declaring return variable */
	uint32_t ret = 0;

	if (configSize == NULL)
		ret = CDN_EINVAL;
	else if (lpddr4_configsf(config) == CDN_EINVAL)
		ret = CDN_EINVAL;
	else{
		/*
		 * All 'if ... else if' constructs shall be terminated with an 'else' statement
		 * (MISRA2012-RULE-15_7-3)
		 */
	}

	return ret;
}

/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] cfg Specifies driver/hardware configuration.
 * @return 0 success
 * @return CDN_EINVAL invalid parameters
 */
static inline uint32_t lpddr4_sanityfunction2(const lpddr4_privatedata* pd, const lpddr4_config* cfg)
{
	/* Declaring return variable */
	uint32_t ret = 0;

	if (lpddr4_privatedatasf(pd) == CDN_EINVAL)
		ret = CDN_EINVAL;
	else if (lpddr4_configsf(cfg) == CDN_EINVAL)
		ret = CDN_EINVAL;
	else{
		/*
		 * All 'if ... else if' constructs shall be terminated with an 'else' statement
		 * (MISRA2012-RULE-15_7-3)
		 */
	}

	return ret;
}

/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] pd Driver state info specific to this instance.
 * @return 0 success
 * @return CDN_EINVAL invalid parameters
 */
static inline uint32_t lpddr4_sanityfunction3(const lpddr4_privatedata* pd)
{
	/* Declaring return variable */
	uint32_t ret = 0;

	if (lpddr4_privatedatasf(pd) == CDN_EINVAL)
		ret = CDN_EINVAL;

	return ret;
}

/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] cpp Indicates whether controller, PHY or PHY Independent Module register
 * @param[out] regvalue Register value read
 * @return 0 success
 * @return CDN_EINVAL invalid parameters
 */
static inline uint32_t lpddr4_sanityfunction4(const lpddr4_privatedata* pd, const lpddr4_regblock cpp, const uint32_t* regvalue)
{
	/* Declaring return variable */
	uint32_t ret = 0;

	if (regvalue == NULL)
		ret = CDN_EINVAL;
	else if (lpddr4_privatedatasf(pd) == CDN_EINVAL)
		ret = CDN_EINVAL;
	else if (
		(cpp != LPDDR4_CTL_REGS) &&
		(cpp != LPDDR4_PHY_REGS) &&
		(cpp != LPDDR4_PHY_INDEP_REGS)
		)
		ret = CDN_EINVAL;
	else{
		/*
		 * All 'if ... else if' constructs shall be terminated with an 'else' statement
		 * (MISRA2012-RULE-15_7-3)
		 */
	}

	return ret;
}

/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] cpp Indicates whether controller, PHY or PHY Independent Module register
 * @return 0 success
 * @return CDN_EINVAL invalid parameters
 */
static inline uint32_t lpddr4_sanityfunction5(const lpddr4_privatedata* pd, const lpddr4_regblock cpp)
{
	/* Declaring return variable */
	uint32_t ret = 0;

	if (lpddr4_privatedatasf(pd) == CDN_EINVAL)
		ret = CDN_EINVAL;
	else if (
		(cpp != LPDDR4_CTL_REGS) &&
		(cpp != LPDDR4_PHY_REGS) &&
		(cpp != LPDDR4_PHY_INDEP_REGS)
		)
		ret = CDN_EINVAL;
	else{
		/*
		 * All 'if ... else if' constructs shall be terminated with an 'else' statement
		 * (MISRA2012-RULE-15_7-3)
		 */
	}

	return ret;
}

/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] pd Driver state info specific to this instance.
 * @param[out] mmrValue Value which is read from memory mode register(mmr) for all devices.
 * @param[out] mmrStatus Status of mode register read(mrr) instruction.
 * @return 0 success
 * @return CDN_EINVAL invalid parameters
 */
static inline uint32_t lpddr4_sanityfunction6(const lpddr4_privatedata* pd, const uint64_t* mmrValue, const uint8_t* mmrStatus)
{
	/* Declaring return variable */
	uint32_t ret = 0;

	if (mmrValue == NULL)
		ret = CDN_EINVAL;
	else if (mmrStatus == NULL)
		ret = CDN_EINVAL;
	else if (lpddr4_privatedatasf(pd) == CDN_EINVAL)
		ret = CDN_EINVAL;
	else{
		/*
		 * All 'if ... else if' constructs shall be terminated with an 'else' statement
		 * (MISRA2012-RULE-15_7-3)
		 */
	}

	return ret;
}

/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] pd Driver state info specific to this instance.
 * @param[out] mrwStatus Status of mode register write(mrw) instruction.
 * @return 0 success
 * @return CDN_EINVAL invalid parameters
 */
static inline uint32_t lpddr4_sanityfunction7(const lpddr4_privatedata* pd, const uint8_t* mrwStatus)
{
	/* Declaring return variable */
	uint32_t ret = 0;

	if (mrwStatus == NULL)
		ret = CDN_EINVAL;
	else if (lpddr4_privatedatasf(pd) == CDN_EINVAL)
		ret = CDN_EINVAL;
	else{
		/*
		 * All 'if ... else if' constructs shall be terminated with an 'else' statement
		 * (MISRA2012-RULE-15_7-3)
		 */
	}

	return ret;
}

/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] pd Driver state info specific to this instance.
 * @param[out] mask Value of interrupt mask
 * @return 0 success
 * @return CDN_EINVAL invalid parameters
 */
static inline uint32_t lpddr4_sanityfunction14(const lpddr4_privatedata* pd, const uint64_t* mask)
{
	/* Declaring return variable */
	uint32_t ret = 0;

	if (mask == NULL)
		ret = CDN_EINVAL;
	else if (lpddr4_privatedatasf(pd) == CDN_EINVAL)
		ret = CDN_EINVAL;
	else{
		/*
		 * All 'if ... else if' constructs shall be terminated with an 'else' statement
		 * (MISRA2012-RULE-15_7-3)
		 */
	}

	return ret;
}

/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] mask Value of interrupt mask to be written
 * @return 0 success
 * @return CDN_EINVAL invalid parameters
 */
static inline uint32_t lpddr4_sanityfunction15(const lpddr4_privatedata* pd, const uint64_t* mask)
{
	/* Declaring return variable */
	uint32_t ret = 0;

	if (mask == NULL)
		ret = CDN_EINVAL;
	else if (lpddr4_privatedatasf(pd) == CDN_EINVAL)
		ret = CDN_EINVAL;
	else{
		/*
		 * All 'if ... else if' constructs shall be terminated with an 'else' statement
		 * (MISRA2012-RULE-15_7-3)
		 */
	}

	return ret;
}

/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] intr Interrupt to be checked
 * @param[out] irqstatus Status of the interrupt, TRUE if active
 * @return 0 success
 * @return CDN_EINVAL invalid parameters
 */
static inline uint32_t lpddr4_sanityfunction16(const lpddr4_privatedata* pd, const lpddr4_ctlinterrupt intr, const bool* irqstatus)
{
	/* Declaring return variable */
	uint32_t ret = 0;

	if (irqstatus == NULL)
		ret = CDN_EINVAL;
	else if (lpddr4_privatedatasf(pd) == CDN_EINVAL)
		ret = CDN_EINVAL;
	else if (
		(intr != LPDDR4_TIMEOUT_ZQ_CAL_INIT) &&
		(intr != LPDDR4_TIMEOUT_ZQ_CALLATCH) &&
		(intr != LPDDR4_TIMEOUT_ZQ_CALSTART) &&
		(intr != LPDDR4_TIMEOUT_MRR_TEMP) &&
		(intr != LPDDR4_TIMEOUT_DQS_OSC_REQ) &&
		(intr != LPDDR4_TIMEOUT_DFI_UPDATE) &&
		(intr != LPDDR4_TIMEOUT_LP_WAKEUP) &&
		(intr != LPDDR4_TIMEOUT_AUTO_REFRESH_MAX) &&
		(intr != LPDDR4_ECC_ERROR) &&
		(intr != LPDDR4_LP_DONE) &&
		(intr != LPDDR4_LP_TIMEOUT) &&
		(intr != LPDDR4_PORT_TIMEOUT) &&
		(intr != LPDDR4_RFIFO_TIMEOUT) &&
		(intr != LPDDR4_TRAINING_ZQ_STATUS) &&
		(intr != LPDDR4_TRAINING_DQS_OSC_DONE) &&
		(intr != LPDDR4_TRAINING_DQS_OSC_UPDATE_DONE) &&
		(intr != LPDDR4_TRAINING_DQS_OSC_OVERFLOW) &&
		(intr != LPDDR4_TRAINING_DQS_OSC_VAR_OUT) &&
		(intr != LPDDR4_USERIF_OUTSIDE_MEM_ACCESS) &&
		(intr != LPDDR4_USERIF_MULTI_OUTSIDE_MEM_ACCESS) &&
		(intr != LPDDR4_USERIF_PORT_CMD_ERROR) &&
		(intr != LPDDR4_USERIF_WRAP) &&
		(intr != LPDDR4_USERIF_INVAL_SETTING) &&
		(intr != LPDDR4_MISC_MRR_TRAFFIC) &&
		(intr != LPDDR4_MISC_SW_REQ_MODE) &&
		(intr != LPDDR4_MISC_CHANGE_TEMP_REFRESH) &&
		(intr != LPDDR4_MISC_TEMP_ALERT) &&
		(intr != LPDDR4_MISC_REFRESH_STATUS) &&
		(intr != LPDDR4_BIST_DONE) &&
		(intr != LPDDR4_CRC) &&
		(intr != LPDDR4_DFI_UPDATE_ERROR) &&
		(intr != LPDDR4_DFI_PHY_ERROR) &&
		(intr != LPDDR4_DFI_BUS_ERROR) &&
		(intr != LPDDR4_DFI_STATE_CHANGE) &&
		(intr != LPDDR4_DFI_DLL_SYNC_DONE) &&
		(intr != LPDDR4_DFI_TIMEOUT) &&
		(intr != LPDDR4_DIMM) &&
		(intr != LPDDR4_FREQ_DFS_REQ_HW_IGNORE) &&
		(intr != LPDDR4_FREQ_DFS_HW_TERMINATE) &&
		(intr != LPDDR4_FREQ_DFS_HW_DONE) &&
		(intr != LPDDR4_FREQ_DFS_REQ_SW_IGNORE) &&
		(intr != LPDDR4_FREQ_DFS_SW_TERMINATE) &&
		(intr != LPDDR4_FREQ_DFS_SW_DONE) &&
		(intr != LPDDR4_INIT_MEM_RESET_DONE) &&
		(intr != LPDDR4_INIT_MC_DONE) &&
		(intr != LPDDR4_INIT_POWER_ON_STATE) &&
		(intr != LPDDR4_MODE_MRR_ERROR) &&
		(intr != LPDDR4_MODE_READ_REQ_DONE) &&
		(intr != LPDDR4_MODE_WRITE_REQ_DONE) &&
		(intr != LPDDR4_PARITY_ERROR) &&
		(intr != LPDDR4_LOR_BIT)
		)
		ret = CDN_EINVAL;
	else{
		/*
		 * All 'if ... else if' constructs shall be terminated with an 'else' statement
		 * (MISRA2012-RULE-15_7-3)
		 */
	}

	return ret;
}

/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] intr Interrupt to be acknowledged
 * @return 0 success
 * @return CDN_EINVAL invalid parameters
 */
static inline uint32_t lpddr4_sanityfunction17(const lpddr4_privatedata* pd, const lpddr4_ctlinterrupt intr)
{
	/* Declaring return variable */
	uint32_t ret = 0;

	if (lpddr4_privatedatasf(pd) == CDN_EINVAL)
		ret = CDN_EINVAL;
	else if (
		(intr != LPDDR4_TIMEOUT_ZQ_CAL_INIT) &&
		(intr != LPDDR4_TIMEOUT_ZQ_CALLATCH) &&
		(intr != LPDDR4_TIMEOUT_ZQ_CALSTART) &&
		(intr != LPDDR4_TIMEOUT_MRR_TEMP) &&
		(intr != LPDDR4_TIMEOUT_DQS_OSC_REQ) &&
		(intr != LPDDR4_TIMEOUT_DFI_UPDATE) &&
		(intr != LPDDR4_TIMEOUT_LP_WAKEUP) &&
		(intr != LPDDR4_TIMEOUT_AUTO_REFRESH_MAX) &&
		(intr != LPDDR4_ECC_ERROR) &&
		(intr != LPDDR4_LP_DONE) &&
		(intr != LPDDR4_LP_TIMEOUT) &&
		(intr != LPDDR4_PORT_TIMEOUT) &&
		(intr != LPDDR4_RFIFO_TIMEOUT) &&
		(intr != LPDDR4_TRAINING_ZQ_STATUS) &&
		(intr != LPDDR4_TRAINING_DQS_OSC_DONE) &&
		(intr != LPDDR4_TRAINING_DQS_OSC_UPDATE_DONE) &&
		(intr != LPDDR4_TRAINING_DQS_OSC_OVERFLOW) &&
		(intr != LPDDR4_TRAINING_DQS_OSC_VAR_OUT) &&
		(intr != LPDDR4_USERIF_OUTSIDE_MEM_ACCESS) &&
		(intr != LPDDR4_USERIF_MULTI_OUTSIDE_MEM_ACCESS) &&
		(intr != LPDDR4_USERIF_PORT_CMD_ERROR) &&
		(intr != LPDDR4_USERIF_WRAP) &&
		(intr != LPDDR4_USERIF_INVAL_SETTING) &&
		(intr != LPDDR4_MISC_MRR_TRAFFIC) &&
		(intr != LPDDR4_MISC_SW_REQ_MODE) &&
		(intr != LPDDR4_MISC_CHANGE_TEMP_REFRESH) &&
		(intr != LPDDR4_MISC_TEMP_ALERT) &&
		(intr != LPDDR4_MISC_REFRESH_STATUS) &&
		(intr != LPDDR4_BIST_DONE) &&
		(intr != LPDDR4_CRC) &&
		(intr != LPDDR4_DFI_UPDATE_ERROR) &&
		(intr != LPDDR4_DFI_PHY_ERROR) &&
		(intr != LPDDR4_DFI_BUS_ERROR) &&
		(intr != LPDDR4_DFI_STATE_CHANGE) &&
		(intr != LPDDR4_DFI_DLL_SYNC_DONE) &&
		(intr != LPDDR4_DFI_TIMEOUT) &&
		(intr != LPDDR4_DIMM) &&
		(intr != LPDDR4_FREQ_DFS_REQ_HW_IGNORE) &&
		(intr != LPDDR4_FREQ_DFS_HW_TERMINATE) &&
		(intr != LPDDR4_FREQ_DFS_HW_DONE) &&
		(intr != LPDDR4_FREQ_DFS_REQ_SW_IGNORE) &&
		(intr != LPDDR4_FREQ_DFS_SW_TERMINATE) &&
		(intr != LPDDR4_FREQ_DFS_SW_DONE) &&
		(intr != LPDDR4_INIT_MEM_RESET_DONE) &&
		(intr != LPDDR4_INIT_MC_DONE) &&
		(intr != LPDDR4_INIT_POWER_ON_STATE) &&
		(intr != LPDDR4_MODE_MRR_ERROR) &&
		(intr != LPDDR4_MODE_READ_REQ_DONE) &&
		(intr != LPDDR4_MODE_WRITE_REQ_DONE) &&
		(intr != LPDDR4_PARITY_ERROR) &&
		(intr != LPDDR4_LOR_BIT)
		)
		ret = CDN_EINVAL;
	else{
		/*
		 * All 'if ... else if' constructs shall be terminated with an 'else' statement
		 * (MISRA2012-RULE-15_7-3)
		 */
	}

	return ret;
}

/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] pd Driver state info specific to this instance.
 * @param[out] mask Value of interrupt mask
 * @return 0 success
 * @return CDN_EINVAL invalid parameters
 */
static inline uint32_t lpddr4_sanityfunction18(const lpddr4_privatedata* pd, const uint32_t* mask)
{
	/* Declaring return variable */
	uint32_t ret = 0;

	if (mask == NULL)
		ret = CDN_EINVAL;
	else if (lpddr4_privatedatasf(pd) == CDN_EINVAL)
		ret = CDN_EINVAL;
	else{
		/*
		 * All 'if ... else if' constructs shall be terminated with an 'else' statement
		 * (MISRA2012-RULE-15_7-3)
		 */
	}

	return ret;
}

/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] intr Interrupt to be checked
 * @param[out] irqstatus Status of the interrupt, TRUE if active
 * @return 0 success
 * @return CDN_EINVAL invalid parameters
 */
static inline uint32_t lpddr4_sanityfunction20(const lpddr4_privatedata* pd, const lpddr4_phyindepinterrupt intr, const bool* irqstatus)
{
	/* Declaring return variable */
	uint32_t ret = 0;

	if (irqstatus == NULL)
		ret = CDN_EINVAL;
	else if (lpddr4_privatedatasf(pd) == CDN_EINVAL)
		ret = CDN_EINVAL;
	else if (
		(intr != LPDDR4_PHY_INDEP_INIT_DONE_BIT) &&
		(intr != LPDDR4_PHY_INDEP_CA_PARITY_ERR_BIT) &&
		(intr != LPDDR4_PHY_INDEP_RDLVL_ERROR_BIT) &&
		(intr != LPDDR4_PHY_INDEP_RDLVL_GATE_ERROR_BIT) &&
		(intr != LPDDR4_PHY_INDEP_WRLVL_ERROR_BIT) &&
		(intr != LPDDR4_PHY_INDEP_CALVL_ERROR_BIT) &&
		(intr != LPDDR4_PHY_INDEP_WDQLVL_ERROR_BIT) &&
		(intr != LPDDR4_PHY_INDEP_UPDATE_ERROR_BIT) &&
		(intr != LPDDR4_PHY_INDEP_RDLVL_REQ_BIT) &&
		(intr != LPDDR4_PHY_INDEP_RDLVL_GATE_REQ_BIT) &&
		(intr != LPDDR4_PHY_INDEP_WRLVL_REQ_BIT) &&
		(intr != LPDDR4_PHY_INDEP_CALVL_REQ_BIT) &&
		(intr != LPDDR4_PHY_INDEP_WDQLVL_REQ_BIT) &&
		(intr != LPDDR4_PHY_INDEP_LVL_DONE_BIT) &&
		(intr != LPDDR4_PHY_INDEP_BIST_DONE_BIT) &&
		(intr != LPDDR4_PHY_INDEP_TDFI_INIT_TIME_OUT_BIT) &&
		(intr != LPDDR4_PHY_INDEP_DLL_LOCK_STATE_CHANGE_BIT) &&
		(intr != LPDDR4_PHY_INDEP_MEM_RST_VALID_BIT) &&
		(intr != LPDDR4_PHY_INDEP_ZQ_STATUS_BIT) &&
		(intr != LPDDR4_PHY_INDEP_PERIPHERAL_MRR_DONE_BIT) &&
		(intr != LPDDR4_PHY_INDEP_WRITE_NODEREG_DONE_BIT) &&
		(intr != LPDDR4_PHY_INDEP_FREQ_CHANGE_DONE_BIT) &&
		(intr != LPDDR4_PHY_INDEP_RDLVL_GATE_DONE_BIT) &&
		(intr != LPDDR4_PHY_INDEP_RDLVL_DONE_BIT) &&
		(intr != LPDDR4_PHY_INDEP_WRLVL_DONE_BIT) &&
		(intr != LPDDR4_PHY_INDEP_CALVL_DONE__BIT) &&
		(intr != LPDDR4_PHY_INDEP_WDQLVL_DONE_BIT) &&
		(intr != LPDDR4_PHY_INDEP_VREF_DONE_BIT) &&
		(intr != LPDDR4_PHY_INDEP_ANY_VALID_BIT)
		)
		ret = CDN_EINVAL;
	else{
		/*
		 * All 'if ... else if' constructs shall be terminated with an 'else' statement
		 * (MISRA2012-RULE-15_7-3)
		 */
	}

	return ret;
}

/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] intr Interrupt to be acknowledged
 * @return 0 success
 * @return CDN_EINVAL invalid parameters
 */
static inline uint32_t lpddr4_sanityfunction21(const lpddr4_privatedata* pd, const lpddr4_phyindepinterrupt intr)
{
	/* Declaring return variable */
	uint32_t ret = 0;

	if (lpddr4_privatedatasf(pd) == CDN_EINVAL)
		ret = CDN_EINVAL;
	else if (
		(intr != LPDDR4_PHY_INDEP_INIT_DONE_BIT) &&
		(intr != LPDDR4_PHY_INDEP_CA_PARITY_ERR_BIT) &&
		(intr != LPDDR4_PHY_INDEP_RDLVL_ERROR_BIT) &&
		(intr != LPDDR4_PHY_INDEP_RDLVL_GATE_ERROR_BIT) &&
		(intr != LPDDR4_PHY_INDEP_WRLVL_ERROR_BIT) &&
		(intr != LPDDR4_PHY_INDEP_CALVL_ERROR_BIT) &&
		(intr != LPDDR4_PHY_INDEP_WDQLVL_ERROR_BIT) &&
		(intr != LPDDR4_PHY_INDEP_UPDATE_ERROR_BIT) &&
		(intr != LPDDR4_PHY_INDEP_RDLVL_REQ_BIT) &&
		(intr != LPDDR4_PHY_INDEP_RDLVL_GATE_REQ_BIT) &&
		(intr != LPDDR4_PHY_INDEP_WRLVL_REQ_BIT) &&
		(intr != LPDDR4_PHY_INDEP_CALVL_REQ_BIT) &&
		(intr != LPDDR4_PHY_INDEP_WDQLVL_REQ_BIT) &&
		(intr != LPDDR4_PHY_INDEP_LVL_DONE_BIT) &&
		(intr != LPDDR4_PHY_INDEP_BIST_DONE_BIT) &&
		(intr != LPDDR4_PHY_INDEP_TDFI_INIT_TIME_OUT_BIT) &&
		(intr != LPDDR4_PHY_INDEP_DLL_LOCK_STATE_CHANGE_BIT) &&
		(intr != LPDDR4_PHY_INDEP_MEM_RST_VALID_BIT) &&
		(intr != LPDDR4_PHY_INDEP_ZQ_STATUS_BIT) &&
		(intr != LPDDR4_PHY_INDEP_PERIPHERAL_MRR_DONE_BIT) &&
		(intr != LPDDR4_PHY_INDEP_WRITE_NODEREG_DONE_BIT) &&
		(intr != LPDDR4_PHY_INDEP_FREQ_CHANGE_DONE_BIT) &&
		(intr != LPDDR4_PHY_INDEP_RDLVL_GATE_DONE_BIT) &&
		(intr != LPDDR4_PHY_INDEP_RDLVL_DONE_BIT) &&
		(intr != LPDDR4_PHY_INDEP_WRLVL_DONE_BIT) &&
		(intr != LPDDR4_PHY_INDEP_CALVL_DONE__BIT) &&
		(intr != LPDDR4_PHY_INDEP_WDQLVL_DONE_BIT) &&
		(intr != LPDDR4_PHY_INDEP_VREF_DONE_BIT) &&
		(intr != LPDDR4_PHY_INDEP_ANY_VALID_BIT)
		)
		ret = CDN_EINVAL;
	else{
		/*
		 * All 'if ... else if' constructs shall be terminated with an 'else' statement
		 * (MISRA2012-RULE-15_7-3)
		 */
	}

	return ret;
}

/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] pd Driver state info specific to this instance.
 * @param[out] debugInfo status
 * @return 0 success
 * @return CDN_EINVAL invalid parameters
 */
static inline uint32_t lpddr4_sanityfunction22(const lpddr4_privatedata* pd, const lpddr4_debuginfo* debugInfo)
{
	/* Declaring return variable */
	uint32_t ret = 0;

	if (debugInfo == NULL)
		ret = CDN_EINVAL;
	else if (lpddr4_privatedatasf(pd) == CDN_EINVAL)
		ret = CDN_EINVAL;
	else{
		/*
		 * All 'if ... else if' constructs shall be terminated with an 'else' statement
		 * (MISRA2012-RULE-15_7-3)
		 */
	}

	return ret;
}

/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] lpiWakeUpParam LPI timing parameter
 * @param[in] fspNum Frequency copy
 * @param[out] cycles Timing value(in cycles)
 * @return 0 success
 * @return CDN_EINVAL invalid parameters
 */
static inline uint32_t lpddr4_sanityfunction23(const lpddr4_privatedata* pd, const lpddr4_lpiwakeupparam* lpiWakeUpParam, const lpddr4_ctlfspnum* fspNum, const uint32_t* cycles)
{
	/* Declaring return variable */
	uint32_t ret = 0;

	if (lpiWakeUpParam == NULL)
		ret = CDN_EINVAL;
	else if (fspNum == NULL)
		ret = CDN_EINVAL;
	else if (cycles == NULL)
		ret = CDN_EINVAL;
	else if (lpddr4_privatedatasf(pd) == CDN_EINVAL)
		ret = CDN_EINVAL;
	else if (
		(*lpiWakeUpParam != LPDDR4_LPI_PD_WAKEUP_FN) &&
		(*lpiWakeUpParam != LPDDR4_LPI_SR_SHORT_WAKEUP_FN) &&
		(*lpiWakeUpParam != LPDDR4_LPI_SR_LONG_WAKEUP_FN) &&
		(*lpiWakeUpParam != LPDDR4_LPI_SR_LONG_MCCLK_GATE_WAKEUP_FN) &&
		(*lpiWakeUpParam != LPDDR4_LPI_SRPD_SHORT_WAKEUP_FN) &&
		(*lpiWakeUpParam != LPDDR4_LPI_SRPD_LONG_WAKEUP_FN) &&
		(*lpiWakeUpParam != LPDDR4_LPI_SRPD_LONG_MCCLK_GATE_WAKEUP_FN)
		)
		ret = CDN_EINVAL;
	else if (
		(*fspNum != LPDDR4_FSP_0) &&
		(*fspNum != LPDDR4_FSP_1) &&
		(*fspNum != LPDDR4_FSP_2)
		)
		ret = CDN_EINVAL;
	else{
		/*
		 * All 'if ... else if' constructs shall be terminated with an 'else' statement
		 * (MISRA2012-RULE-15_7-3)
		 */
	}

	return ret;
}

/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] pd Driver state info specific to this instance.
 * @param[out] mode Half Datapath setting
 * @return 0 success
 * @return CDN_EINVAL invalid parameters
 */
static inline uint32_t lpddr4_sanityfunction25(const lpddr4_privatedata* pd, const lpddr4_reducmode* mode)
{
	/* Declaring return variable */
	uint32_t ret = 0;

	if (mode == NULL)
		ret = CDN_EINVAL;
	else if (lpddr4_privatedatasf(pd) == CDN_EINVAL)
		ret = CDN_EINVAL;
	else{
		/*
		 * All 'if ... else if' constructs shall be terminated with an 'else' statement
		 * (MISRA2012-RULE-15_7-3)
		 */
	}

	return ret;
}

/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] mode Half Datapath setting
 * @return 0 success
 * @return CDN_EINVAL invalid parameters
 */
static inline uint32_t lpddr4_sanityfunction26(const lpddr4_privatedata* pd, const lpddr4_reducmode* mode)
{
	/* Declaring return variable */
	uint32_t ret = 0;

	if (mode == NULL)
		ret = CDN_EINVAL;
	else if (lpddr4_privatedatasf(pd) == CDN_EINVAL)
		ret = CDN_EINVAL;
	else if (
		(*mode != LPDDR4_REDUC_ON) &&
		(*mode != LPDDR4_REDUC_OFF)
		)
		ret = CDN_EINVAL;
	else{
		/*
		 * All 'if ... else if' constructs shall be terminated with an 'else' statement
		 * (MISRA2012-RULE-15_7-3)
		 */
	}

	return ret;
}

/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] pd Driver state info specific to this instance.
 * @param[out] on_off DBI read value
 * @return 0 success
 * @return CDN_EINVAL invalid parameters
 */
static inline uint32_t lpddr4_sanityfunction27(const lpddr4_privatedata* pd, const bool* on_off)
{
	/* Declaring return variable */
	uint32_t ret = 0;

	if (on_off == NULL)
		ret = CDN_EINVAL;
	else if (lpddr4_privatedatasf(pd) == CDN_EINVAL)
		ret = CDN_EINVAL;
	else{
		/*
		 * All 'if ... else if' constructs shall be terminated with an 'else' statement
		 * (MISRA2012-RULE-15_7-3)
		 */
	}

	return ret;
}

/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] mode status
 * @return 0 success
 * @return CDN_EINVAL invalid parameters
 */
static inline uint32_t lpddr4_sanityfunction29(const lpddr4_privatedata* pd, const lpddr4_dbimode* mode)
{
	/* Declaring return variable */
	uint32_t ret = 0;

	if (mode == NULL)
		ret = CDN_EINVAL;
	else if (lpddr4_privatedatasf(pd) == CDN_EINVAL)
		ret = CDN_EINVAL;
	else if (
		(*mode != LPDDR4_DBI_RD_ON) &&
		(*mode != LPDDR4_DBI_RD_OFF) &&
		(*mode != LPDDR4_DBI_WR_ON) &&
		(*mode != LPDDR4_DBI_WR_OFF)
		)
		ret = CDN_EINVAL;
	else{
		/*
		 * All 'if ... else if' constructs shall be terminated with an 'else' statement
		 * (MISRA2012-RULE-15_7-3)
		 */
	}

	return ret;
}

/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] pd Driver state info specific to this instance.
 * @param[out] fspNum Frequency set number
 * @param[in] tref Refresh rate (in cycles)
 * @param[out] tras_max Maximum row active time (in cycles)
 * @return 0 success
 * @return CDN_EINVAL invalid parameters
 */
static inline uint32_t lpddr4_sanityfunction30(const lpddr4_privatedata* pd, const lpddr4_ctlfspnum* fspNum, const uint32_t* tref, const uint32_t* tras_max)
{
	/* Declaring return variable */
	uint32_t ret = 0;

	if (fspNum == NULL)
		ret = CDN_EINVAL;
	else if (tref == NULL)
		ret = CDN_EINVAL;
	else if (tras_max == NULL)
		ret = CDN_EINVAL;
	else if (lpddr4_privatedatasf(pd) == CDN_EINVAL)
		ret = CDN_EINVAL;
	else{
		/*
		 * All 'if ... else if' constructs shall be terminated with an 'else' statement
		 * (MISRA2012-RULE-15_7-3)
		 */
	}

	return ret;
}

/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] fspNum Frequency set number
 * @param[in] tref Refresh rate (in cycles)
 * @param[in] tras_max Maximum row active time (in cycles)
 * @return 0 success
 * @return CDN_EINVAL invalid parameters
 */
static inline uint32_t lpddr4_sanityfunction31(const lpddr4_privatedata* pd, const lpddr4_ctlfspnum* fspNum, const uint32_t* tref, const uint32_t* tras_max)
{
	/* Declaring return variable */
	uint32_t ret = 0;

	if (fspNum == NULL)
		ret = CDN_EINVAL;
	else if (tref == NULL)
		ret = CDN_EINVAL;
	else if (tras_max == NULL)
		ret = CDN_EINVAL;
	else if (lpddr4_privatedatasf(pd) == CDN_EINVAL)
		ret = CDN_EINVAL;
	else if (
		(*fspNum != LPDDR4_FSP_0) &&
		(*fspNum != LPDDR4_FSP_1) &&
		(*fspNum != LPDDR4_FSP_2)
		)
		ret = CDN_EINVAL;
	else{
		/*
		 * All 'if ... else if' constructs shall be terminated with an 'else' statement
		 * (MISRA2012-RULE-15_7-3)
		 */
	}

	return ret;
}

#ifdef __cplusplus
}
#endif

/* parasoft-end-suppress METRICS-41-3 */
/* parasoft-end-suppress METRICS-39-3 */
/* parasoft-end-suppress METRICS-36-3 */
/* parasoft-end-suppress METRICS-18-3 */

#endif  /* LPDDR4_SANITY_H */
