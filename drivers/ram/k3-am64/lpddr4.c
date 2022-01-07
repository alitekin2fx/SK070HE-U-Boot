// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 * Copyright (C) 2012-2020 Cadence Design Systems, Inc.
 * Copyright (C) 2020 Texas Instruments Incorporated - http://www.ti.com/
 * lpddr4.c
 *
 *****************************************************************************
*/
#include "cdn_errno.h"

#include "cps_drv_lpddr4.h"
#include "lpddr4_ctl_regs.h"
#include "lpddr4_if.h"
#include "lpddr4_private.h"
#include "lpddr4_sanity.h"
#include "lpddr4_structs_if.h"

#define LPDDR4_CUSTOM_TIMEOUT_DELAY 100000000U

#define LPDDR4_CPS_NS_DELAY_TIME 10000000U

#define GRP_SHIFT 1
#define INT_SHIFT 2

#define CPS_DelayNs(x) {uint32_t i; for(i=0U; i<x ;i++) {} }

static uint32_t CtlIntMap[51][3] = {
	{0,  0,	 7 },
	{1,  0,	 8 },
	{2,  0,	 9 },
	{3,  0,	 14},
	{4,  0,	 15},
	{5,  0,	 16},
	{6,  0,	 17},
	{7,  0,	 19},
	{8,  1,	 0 },
	{9,  2,	 0 },
	{10, 2,	 3 },
	{11, 3,	 0 },
	{12, 4,	 0 },
	{13, 5,	 11},
	{14, 5,	 12},
	{15, 5,	 13},
	{16, 5,	 14},
	{17, 5,	 15},
	{18, 6,	 0 },
	{19, 6,	 1 },
	{20, 6,	 2 },
	{21, 6,	 6 },
	{22, 6,	 7 },
	{23, 7,	 3 },
	{24, 7,	 4 },
	{25, 7,	 5 },
	{26, 7,	 6 },
	{27, 7,	 7 },
	{28, 8,	 0 },
	{29, 9,	 0 },
	{30, 10, 0 },
	{31, 10, 1 },
	{32, 10, 2 },
	{33, 10, 3 },
	{34, 10, 4 },
	{35, 10, 5 },
	{36, 11, 0 },
	{37, 12, 0 },
	{38, 12, 1 },
	{39, 12, 2 },
	{40, 12, 3 },
	{41, 12, 4 },
	{42, 12, 5 },
	{43, 13, 0 },
	{44, 13, 1 },
	{45, 13, 3 },
	{46, 14, 0 },
	{47, 14, 2 },
	{48, 14, 3 },
	{49, 15, 2 },
	{50, 16, 0 },
};

/**
 * Internal Function:Poll for status of interrupt received by the Controller.
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] irqbit Interrupt status bit to be checked.
 * @param[in] delay time delay.
 * @return CDN_EOK on success (Interrupt status high).
 * @return CDN_EIO on poll time out.
 * @return CDN_EINVAL checking status was not successful.
 */
static uint32_t lpddr4_pollctlirq(const lpddr4_privatedata* pd, lpddr4_ctlinterrupt irqbit,  uint32_t delay)
{

	uint32_t result = 0U;
	uint32_t timeout = 0U;
	bool irqstatus = false;

	/* Loop until irqstatus found to be 1 or if value of 'result' !=CDN_EOK */
	do {
		if (++timeout == delay) {
			result = CDN_EIO;
			break;
		}
		CPS_DelayNs(LPDDR4_CPS_NS_DELAY_TIME);
		result = lpddr4_checkctlinterrupt(pd, irqbit, &irqstatus);
	} while ((irqstatus == false) && (result == (uint32_t)CDN_EOK));

	return result;
}

/**
 * Internal Function:Poll for status of interrupt received by the PHY Independent Module.
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] irqbit Interrupt status bit to be checked.
 * @param[in] delay time delay.
 * @return CDN_EOK on success (Interrupt status high).
 * @return CDN_EIO on poll time out.
 * @return CDN_EINVAL checking status was not successful.
 */
static uint32_t lpddr4_pollphyindepirq(const lpddr4_privatedata* pd, lpddr4_phyindepinterrupt irqbit,  uint32_t delay)
{

	uint32_t result = 0U;
	uint32_t timeout = 0U;
	bool irqstatus = false;

	/* Loop until irqstatus found to be 1 or if value of 'result' !=CDN_EOK */
	do {
		if (++timeout == delay) {
			result = CDN_EIO;
			break;
		}
		CPS_DelayNs(LPDDR4_CPS_NS_DELAY_TIME);
		result = lpddr4_checkphyindepinterrupt(pd, irqbit, &irqstatus);
	} while ((irqstatus == false) && (result == (uint32_t)CDN_EOK));

	return result;
}

/**
 * Internal Function:Trigger function to poll and Ack IRQs
 * @param[in] pd Driver state info specific to this instance.
 * @return CDN_EOK on success (Interrupt status high).
 * @return CDN_EIO on poll time out.
 * @return CDN_EINVAL checking status was not successful.
 */
static uint32_t lpddr4_pollandackirq(const lpddr4_privatedata* pd)
{
	uint32_t result = 0U;

	/* Wait for PhyIndependent module to finish up ctl init sequence */
	result = lpddr4_pollphyindepirq(pd,LPDDR4_PHY_INDEP_INIT_DONE_BIT, LPDDR4_CUSTOM_TIMEOUT_DELAY);

	/* Ack to clear the PhyIndependent interrupt bit */
	if (result == (uint32_t)CDN_EOK)
		result = lpddr4_ackphyindepinterrupt(pd,LPDDR4_PHY_INDEP_INIT_DONE_BIT);
	/* Wait for the CTL end of initialization */
	if (result == (uint32_t)CDN_EOK)
		result = lpddr4_pollctlirq(pd,LPDDR4_INIT_MC_DONE, LPDDR4_CUSTOM_TIMEOUT_DELAY);
	/* Ack to clear the Ctl interrupt bit */
	if (result == (uint32_t)CDN_EOK)
		result = lpddr4_ackctlinterrupt(pd,LPDDR4_INIT_MC_DONE);
	return result;
}

/**
 * Internal Function: Controller start sequence.
 * @param[in] pd Driver state info specific to this instance.
 * @return CDN_EOK on success.
 * @return CDN_EINVAL starting controller was not successful.
 */
static uint32_t lpddr4_startsequencecontroller(const lpddr4_privatedata* pd)
{
	uint32_t result = 0U;
	uint32_t regval = 0U;

	LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;
	lpddr4_infotype infoType;

	/* Set the PI_start to initiate leveling procedure */
	regval = CPS_FLD_SET(LPDDR4__PI_START__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__PI_START__REG)));
	CPS_REG_WRITE((&(ctlRegBase->LPDDR4__PI_START__REG)), regval);

	/* Set the Ctl_start  */
	regval = CPS_FLD_SET(LPDDR4__START__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__START__REG)));
	CPS_REG_WRITE(&(ctlRegBase->LPDDR4__START__REG), regval);

	if (pd->infoHandler != NULL) {
		/* If a handler is registered, call it with the relevant information type */
		infoType = LPDDR4_DRV_SOC_PLL_UPDATE;
		pd->infoHandler(pd, infoType);
	}

	result = lpddr4_pollandackirq(pd);

	return result;
}

/**
 * Internal Function: To add the offset to given address.
 * @param[in] addr Address to which the offset has to be added.
 * @param[in] regOffset The offset
 * @return regAddr The address value after the summation.
 */
static volatile uint32_t* lpddr4_addoffset(volatile uint32_t* addr, uint32_t regOffset)
{

	volatile uint32_t* local_addr = addr;
	/* Declaring as array to add the offset value.*/
	volatile uint32_t* regAddr = &local_addr[regOffset];

	return regAddr;
}

/**
 * Checks configuration object.
 * @param[in] config Driver/hardware configuration required.
 * @param[out] configSize Size of memory allocations required.
 * @return CDN_EOK on success (requirements structure filled).
 * @return CDN_ENOTSUP if configuration cannot be supported due to driver/hardware constraints.
 */
uint32_t lpddr4_probe(const lpddr4_config* config, uint16_t* configSize)
{
	uint32_t result;

	result = (uint32_t)(lpddr4_probeSF(config, configSize));
	if ( result == (uint32_t)CDN_EOK )
		*configSize = (uint16_t)(sizeof(lpddr4_privatedata));
	return result;
}

/**
 * Init function to be called after LPDDR4_probe() to set up the driver configuration.
 * Memory should be allocated for drv_data (using the size determined using LPDDR4_probe) before
 * calling  this API, init_settings should be initialized with base addresses for PHY Independent Module,
 * Controller and PHY before calling this function.
 * If callbacks are required for interrupt handling, these should also be configured in init_settings.
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] cfg Specifies driver/hardware configuration.
 * @return CDN_EOK on success
 * @return CDN_EINVAL if illegal/inconsistent values in cfg.
 * @return CDN_ENOTSUP if hardware has an inconsistent configuration or doesn't support feature(s)
 * required by 'config' parameters.
 */
uint32_t lpddr4_init(lpddr4_privatedata* pd, const lpddr4_config* cfg)
{
	uint32_t result = 0U;
	uint16_t productID = 0U;
	uint32_t version[2] = {0,0};

	result = lpddr4_initSF(pd, cfg);
	if ( result == (uint32_t)CDN_EOK ) {
		/* Validate Magic number */
		LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)cfg->ctlbase;
		productID = (uint16_t)(CPS_FLD_READ(LPDDR4__CONTROLLER_ID__FLD,
						    CPS_REG_READ(&(ctlRegBase->LPDDR4__CONTROLLER_ID__REG))));
		version[0] = (uint32_t)(CPS_FLD_READ(LPDDR4__CONTROLLER_VERSION_0__FLD,
						     CPS_REG_READ(&(ctlRegBase->LPDDR4__CONTROLLER_VERSION_0__REG))));
		version[1] = (uint32_t)(CPS_FLD_READ(LPDDR4__CONTROLLER_VERSION_1__FLD,
						     CPS_REG_READ(&(ctlRegBase->LPDDR4__CONTROLLER_VERSION_1__REG))));
		if ((productID == PRODUCT_ID) && (version[0] == VERSION_0) && (version[1] == VERSION_1)) {
			/* Populating configuration data to pd */
			pd->ctlBase = ctlRegBase;
			pd->infoHandler = (lpddr4_infocallback)cfg->infohandler;
			pd->ctlInterruptHandler = (lpddr4_ctlcallback)cfg->ctlinterrupthandler;
			pd->phyIndepInterruptHandler = (lpddr4_phyindepcallback)cfg->phyindepinterrupthandler;
		} else
			/* Magic number validation failed - Driver doesn't support given IP version */
			result = (uint32_t)CDN_EOPNOTSUPP;
	}
	return result;
}

/**
 * Start the driver.
 * @param[in] pd Driver state info specific to this instance.
 */
uint32_t lpddr4_start(const lpddr4_privatedata* pd)
{
	uint32_t result = 0U;
	uint32_t regval = 0U;

	result = lpddr4_startSF(pd);
	if (result == (uint32_t)CDN_EOK) {
		LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;

		/* Enable PI as the initiator for DRAM */
		regval = CPS_FLD_SET(LPDDR4__PI_NORMAL_LVL_SEQ__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__PI_NORMAL_LVL_SEQ__REG)));
		CPS_REG_WRITE((&(ctlRegBase->LPDDR4__PI_NORMAL_LVL_SEQ__REG)), regval);
		regval = CPS_FLD_SET(LPDDR4__PI_INIT_LVL_EN__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__PI_INIT_LVL_EN__REG)));
		CPS_REG_WRITE((&(ctlRegBase->LPDDR4__PI_INIT_LVL_EN__REG)), regval);

		/* Start PI init sequence. */
		result = lpddr4_startsequencecontroller(pd);
	}
	return result;
}

/**
 * Read a register from the controller, PHY or PHY Independent Module
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] cpp Indicates whether controller, PHY or PHY Independent Module register
 * @param[in] regOffset Register offset
 * @param[out] regvalue Register value read
 * @return CDN_EOK on success.
 * @return CDN_EINVAL if regOffset if out of range or regvalue is NULL
 */
uint32_t lpddr4_readreg(const lpddr4_privatedata* pd, lpddr4_regblock cpp, uint32_t regOffset, uint32_t* regvalue)
{
	uint32_t result = 0U;

	result = lpddr4_readregSF(pd, cpp, regvalue);
	if (result == (uint32_t)CDN_EOK) {
		LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;

		if (cpp == LPDDR4_CTL_REGS) {
			if (regOffset >= LPDDR4_CTL_REG_COUNT)
				/* Return if user provider invalid register number */
				result = CDN_EINVAL;
			else
				*regvalue = CPS_REG_READ(lpddr4_addoffset(&(ctlRegBase->DENALI_CTL_0), regOffset));
		} else if (cpp == LPDDR4_PHY_REGS) {
			if (regOffset >= LPDDR4_PHY_REG_COUNT)
				/* Return if user provider invalid register number */
				result = CDN_EINVAL;
			else
				*regvalue = CPS_REG_READ(lpddr4_addoffset(&(ctlRegBase->DENALI_PHY_0), regOffset));

		} else {
			if (regOffset >= LPDDR4_PHY_INDEP_REG_COUNT)
				/* Return if user provider invalid register number */
				result = CDN_EINVAL;
			else
				*regvalue = CPS_REG_READ(lpddr4_addoffset(&(ctlRegBase->DENALI_PI_0), regOffset));
		}
	}
	return result;
}

#ifdef REG_WRITE_VERIF

/**
 * Internal Function: To fetch the read-write mask for the PHY data slice registers.
 * @param[in] dSliceNum The data slice
 * @param[in] arrayOffset The offset in data slice read-write array
 * @return the read-write mask for the corresponding offset.
 */
static uint32_t LPDDR4_getDSliceMask(uint32_t dSliceNum, uint32_t arrayOffset )
{

	uint32_t rwMask = 0U;

	/* Fetch the read-write mask from the respective mask array */
	switch (dSliceNum) {
	case 0:
		if (arrayOffset < DSLICE0_REG_COUNT)
			rwMask = g_lpddr4_data_slice_0_rw_mask[arrayOffset];
		break;
	case 1:
		if (arrayOffset < DSLICE1_REG_COUNT)
			rwMask = g_lpddr4_data_slice_1_rw_mask[arrayOffset];
		break;
	case 2:
		if (arrayOffset < DSLICE2_REG_COUNT)
			rwMask = g_lpddr4_data_slice_2_rw_mask[arrayOffset];
		break;
	default:
		/* Since the regOffset is valid, should be data_slice_3 */
		if (arrayOffset < DSLICE3_REG_COUNT)
			rwMask = g_lpddr4_data_slice_3_rw_mask[arrayOffset];
		break;
	}
	return rwMask;
}
/**
 * Internal Function: To fetch the read-write mask for the phy registers.
 * @param[in] regOffset The offset
 * @return the read-write mask for the corresponding offset.
 */
static uint32_t LPDDR4_getPhyRwMask( uint32_t regOffset )
{

	uint32_t rwMask = 0U;
	uint32_t arrayOffset = 0U;
	uint32_t sliceNum, sliceOffset = 0U;

	/* Iterate to figure out the slice for the given offset */
	for (sliceNum = 0U; sliceNum <= (DSLICE_NUM + ASLICE_NUM); sliceNum++) {
		sliceOffset = sliceOffset + (uint32_t)SLICE_WIDTH;
		if (regOffset < sliceOffset)
			break;
	}
	arrayOffset = regOffset - (sliceOffset - (uint32_t)SLICE_WIDTH);

	/* Fetch the read-write mask from the respective mask array */
	if (sliceNum < DSLICE_NUM)
		rwMask = LPDDR4_getDSliceMask( sliceNum, arrayOffset );
	else {
		if (sliceNum == DSLICE_NUM) {
			if (arrayOffset < ASLICE0_REG_COUNT)
				rwMask = g_lpddr4_address_slice_0_rw_mask[arrayOffset];
		} else {
			if (arrayOffset < PHY_CORE_REG_COUNT)
				/* Since the regOffset is valid, should be phy_core */
				rwMask = g_lpddr4_phy_core_rw_mask[arrayOffset];
		}
	}
	return rwMask;
}

/**
 * Internal Function: To verify register writes.
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] regOffset The offset
 * @param[in] regvalue The value written to the register
 * @return CDN_EOK on success
 * @return CDN_EIO if value read looks different than the written.
 * @return CDN_EINVAL if illegal/inconsistent values in cfg.
 */
static uint32_t LPDDR4_VerifyRegWrite( const lpddr4_privatedata* pd,lpddr4_regblock cpp, uint32_t regOffset, uint32_t regvalue)
{

	uint32_t result = (uint32_t) CDN_EOK;
	uint32_t regReadVal = 0U;
	uint32_t rwMask = 0U;

	/* Read the concern register to compare */
	result = lpddr4_readreg(pd, cpp, regOffset, &regReadVal);

	if (result == (uint32_t)CDN_EOK) {
		/* Selecting the appropriate mask for the given register block*/
		switch (cpp) {
		case LPDDR4_PHY_INDEP_REGS:
			rwMask = g_lpddr4_pi_rw_mask[regOffset];
			break;
		case LPDDR4_PHY_REGS:
			rwMask = LPDDR4_getPhyRwMask(regOffset);
			break;
		default:
			/* LPDDR4_CTL_REGS is considered as the default (sanity check already confirmed it as valid cpp) */
			rwMask = g_lpddr4_ddr_controller_rw_mask[regOffset];
			break;
		}

		/* Compare the read and written values. */
		if ((rwMask & regReadVal) != (regvalue & rwMask))
			result = CDN_EIO;
	}
	return result;
}
#endif

uint32_t lpddr4_writereg(const lpddr4_privatedata* pd, lpddr4_regblock cpp, uint32_t regOffset, uint32_t regvalue)
{
	uint32_t result = 0U;

	result = lpddr4_writeregSF(pd, cpp);
	if (result == (uint32_t)CDN_EOK) {
		LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;

		if (cpp == LPDDR4_CTL_REGS) {
			if (regOffset >= LPDDR4_CTL_REG_COUNT)
				/* Return if user provider invalid register number */
				result = CDN_EINVAL;
			else
				CPS_REG_WRITE(lpddr4_addoffset(&(ctlRegBase->DENALI_CTL_0), regOffset), regvalue);
		} else if (cpp == LPDDR4_PHY_REGS) {
			if (regOffset >= LPDDR4_PHY_REG_COUNT)
				/* Return if user provider invalid register number */
				result = CDN_EINVAL;
			else
				CPS_REG_WRITE(lpddr4_addoffset(&(ctlRegBase->DENALI_PHY_0), regOffset), regvalue);
		} else {
			if (regOffset >= LPDDR4_PHY_INDEP_REG_COUNT)
				/* Return if user provider invalid register number */
				result = CDN_EINVAL;
			else
				CPS_REG_WRITE(lpddr4_addoffset(&(ctlRegBase->DENALI_PI_0), regOffset), regvalue);
		}
	}

	return result;
}
static uint32_t lpddr4_checkmmrreaderror(const lpddr4_privatedata* pd, uint64_t* mmrValue, uint8_t* mrrStatus)
{

	uint64_t lowerData;
	LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;
	uint32_t result = (uint32_t)CDN_EOK;

	/* Check if mode register read error interrupt occurred */
	if (lpddr4_pollctlirq(pd, LPDDR4_MODE_MRR_ERROR, 100) == 0U) {
		/* Mode register read error interrupt, read MRR status register and return.*/
		*mrrStatus = (uint8_t)CPS_FLD_READ(LPDDR4__MRR_ERROR_STATUS__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__MRR_ERROR_STATUS__REG)));
		*mmrValue = 0;
		result = CDN_EIO;
	} else {
		*mrrStatus = 0;
		/* Mode register read was successful, read DATA */
		lowerData = CPS_REG_READ(&(ctlRegBase->LPDDR4__PERIPHERAL_MRR_DATA__REG));
		*mmrValue = (uint64_t)((*mmrValue << WORD_SHIFT) | lowerData);
		/* Acknowledge MR_READ_DONE interrupt to clear it */
		result = lpddr4_ackctlinterrupt(pd, LPDDR4_MODE_READ_REQ_DONE);
	}
	return result;
}

uint32_t lpddr4_getmmrregister(const lpddr4_privatedata* pd, uint32_t readModeRegVal, uint64_t* mmrValue, uint8_t* mmrStatus)
{

	uint32_t result = 0U;
	uint32_t tDelay = 1000U;
	uint32_t regval = 0U;

	result = lpddr4_getmmrregisterSF(pd, mmrValue, mmrStatus);
	if (result == (uint32_t)CDN_EOK) {

		LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;

		/* Populate the calculated value to the register  */
		regval = CPS_FLD_WRITE(LPDDR4__READ_MODEREG__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__READ_MODEREG__REG)), readModeRegVal);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__READ_MODEREG__REG), regval);

		/* Wait until the Read is done */
		result = lpddr4_pollctlirq(pd, LPDDR4_MODE_READ_REQ_DONE, tDelay);
	}
	if (result == (uint32_t)CDN_EOK)
		result = lpddr4_checkmmrreaderror(pd, mmrValue, mmrStatus);
	return result;
}

static uint32_t lpddr4_writemmrregister(const lpddr4_privatedata* pd, uint32_t writeModeRegVal)
{

	uint32_t result = (uint32_t) CDN_EOK;
	uint32_t tDelay = 1000U;
	uint32_t regval = 0U;
	LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;

	/* Populate the calculated value to the register  */
	regval = CPS_FLD_WRITE(LPDDR4__WRITE_MODEREG__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__WRITE_MODEREG__REG)), writeModeRegVal);
	CPS_REG_WRITE(&(ctlRegBase->LPDDR4__WRITE_MODEREG__REG), regval);

	result = lpddr4_pollctlirq(pd, LPDDR4_MODE_WRITE_REQ_DONE, tDelay);

	return result;
}

uint32_t lpddr4_setmmrregister(const lpddr4_privatedata* pd, uint32_t writeModeRegVal, uint8_t* mrwStatus)
{
	uint32_t result = 0U;

	result = lpddr4_setmmrregisterSF(pd, mrwStatus);
	if (result == (uint32_t)CDN_EOK) {

		/* Function call to trigger Mode register write */
		result = lpddr4_writemmrregister(pd, writeModeRegVal);

		if (result == (uint32_t)CDN_EOK)
			result = lpddr4_ackctlinterrupt(pd, LPDDR4_MODE_WRITE_REQ_DONE);
		/* Read the status of mode register write */
		if (result == (uint32_t)CDN_EOK) {
			LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;
			*mrwStatus = (uint8_t)CPS_FLD_READ(LPDDR4__MRW_STATUS__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__MRW_STATUS__REG)));
			if ((*mrwStatus) != 0U)
				result = CDN_EIO;
		}
	}

	return result;
}

uint32_t lpddr4_writectlconfig(const lpddr4_privatedata* pd, uint32_t regvalues[], uint16_t regNum[], uint16_t regCount)
{
	uint32_t result;
	uint32_t aIndex;

	result = lpddr4_writectlconfigSF(pd );
	if ( (regvalues == NULL) || (regNum == NULL ))
		result = CDN_EINVAL;

	if (result == (uint32_t)CDN_EOK) {
		/* Iterate through controller register numbers that user prefer to update.*/
		for (aIndex = 0; aIndex < regCount; aIndex++) {
			/* Calling the write API with the user provided data */
			result =  lpddr4_writereg(pd, LPDDR4_CTL_REGS, (uint32_t)regNum[aIndex],
						  (uint32_t)regvalues[aIndex]);
		}
	}
	return result;
}

uint32_t lpddr4_writephyindepconfig(const lpddr4_privatedata* pd, uint32_t regvalues[], uint16_t regNum[], uint16_t regCount)
{
	uint32_t result;
	uint32_t aIndex;

	result = lpddr4_writephyindepconfigSF(pd);
	if ( (regvalues == NULL) || (regNum == NULL) )
		result = CDN_EINVAL;
	if (result == (uint32_t)CDN_EOK) {
		/* Iterate through PHY Independent module  register numbers that user prefer to update.*/
		for (aIndex = 0; aIndex < regCount; aIndex++) {
			/* Calling the write API with the user provided data */
			result =  lpddr4_writereg(pd, LPDDR4_PHY_INDEP_REGS, (uint32_t)regNum[aIndex],
						  (uint32_t)regvalues[aIndex]);
		}
	}
	return result;
}

uint32_t lpddr4_writephyconfig(const lpddr4_privatedata* pd, uint32_t regvalues[], uint16_t regNum[], uint16_t regCount)
{
	uint32_t result;
	uint32_t aIndex;

	result = lpddr4_writephyconfigSF(pd);
	if ( (regvalues == NULL) || ( regNum == NULL ))
		result = CDN_EINVAL;
	if (result == (uint32_t)CDN_EOK) {
		/* Iterate through PHY  register numbers that user prefer to update.*/
		for (aIndex = 0; aIndex < regCount; aIndex++) {
			/* Calling the write API with the user provided data */
			result =  lpddr4_writereg(pd, LPDDR4_PHY_REGS, (uint32_t)regNum[aIndex],
						  (uint32_t)regvalues[aIndex]);
		}
	}
	return result;
}

uint32_t lpddr4_readctlconfig(const lpddr4_privatedata* pd, uint32_t regvalues[], uint16_t regNum[], uint16_t regCount)
{
	uint32_t result;
	uint32_t aIndex;

	result = lpddr4_readctlconfigSF(pd);
	if ( (regvalues == NULL) || (regNum == NULL) )
		result = CDN_EINVAL;
	if (result == (uint32_t)CDN_EOK) {
		/* Iterate through PHY  register numbers that user prefer to read.*/
		for (aIndex = 0; aIndex < regCount; aIndex++) {
			/* Calling the read API with the user provided data */
			result =  lpddr4_readreg(pd, LPDDR4_CTL_REGS, (uint32_t)regNum[aIndex],
						 (uint32_t*)(&regvalues[aIndex]));
		}
	}
	return result;
}

uint32_t lpddr4_readphyindepconfig(const lpddr4_privatedata* pd, uint32_t regvalues[], uint16_t regNum[], uint16_t regCount)
{
	uint32_t result;
	uint32_t aIndex;

	result = lpddr4_readphyindepconfigSF(pd);
	if ( (regvalues == NULL) || (regNum == NULL) )
		result = CDN_EINVAL;
	if (result == (uint32_t)CDN_EOK) {
		/* Iterate through PHY  register numbers that user prefer to read.*/
		for (aIndex = 0; aIndex < regCount; aIndex++) {
			/* Calling the read API with the user provided data */
			result =  lpddr4_readreg(pd, LPDDR4_PHY_INDEP_REGS, (uint32_t)regNum[aIndex],
						 (uint32_t*)(&regvalues[aIndex]));
		}
	}
	return result;
}

uint32_t lpddr4_readphyconfig(const lpddr4_privatedata* pd, uint32_t regvalues[], uint16_t regNum[], uint16_t regCount)
{
	uint32_t result;
	uint32_t aIndex;

	result = lpddr4_readphyconfigSF(pd);
	if ( (regvalues == NULL) || (regNum == NULL ))
		result = CDN_EINVAL;
	if (result == (uint32_t)CDN_EOK) {
		/* Iterate through PHY  register numbers that user prefer to read.*/
		for (aIndex = 0; aIndex < regCount; aIndex++) {
			/* Calling the read API with the user provided data */
			result =  lpddr4_readreg(pd, LPDDR4_PHY_REGS, (uint32_t)regNum[aIndex],
						 (uint32_t*)(&regvalues[aIndex]));
		}
	}
	return result;
}

uint32_t lpddr4_getctlinterruptmask(const lpddr4_privatedata* pd, uint64_t* mask)
{
	uint32_t result = 0U;

	result = lpddr4_getctlinterruptmaskSF(pd,  mask);
	if (result == (uint32_t)CDN_EOK) {
		LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;
		/* Reading the appropriate mask register */
		*mask = (uint64_t)(CPS_FLD_READ(LPDDR4__INT_MASK_MASTER__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__INT_MASK_MASTER__REG))));
	}
	return result;
}

uint32_t lpddr4_setctlinterruptmask(const lpddr4_privatedata* pd, const uint64_t* mask)
{
	uint32_t result;
	uint32_t regval = 0;
	const uint64_t ui64One = 1ULL;
	const uint32_t ui32IrqCount = (uint32_t)32U;

	result = lpddr4_setctlinterruptmaskSF(pd, mask);
	if ((result == (uint32_t)CDN_EOK) && (ui32IrqCount <= 32U)) {
		/* Return if the user given value is higher than the field width */
		if (*mask >= (ui64One << ui32IrqCount))
			result = CDN_EINVAL;
	}

	if (result == (uint32_t)CDN_EOK) {
		LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;
		/* Extracting the value from the register and writing to mask register again after modify*/
		regval = CPS_FLD_WRITE(LPDDR4__INT_MASK_MASTER__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__INT_MASK_MASTER__REG)), *mask);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__INT_MASK_MASTER__REG), regval);
	}
	return result;
}

static void lpddr4_checkctlinterrupt_4(LPDDR4_CtlRegs* ctlRegBase, lpddr4_ctlinterrupt intr,
				       uint32_t* ctlGrpIrqStatus, uint32_t* CtlMasterIntFlag)
{

	/* Check interrupt status for the interrupt source.*/
	/* Checked interrupt status related to init process */
	if ((intr >= LPDDR4_INIT_MEM_RESET_DONE) && (intr <= LPDDR4_INIT_POWER_ON_STATE))
		*ctlGrpIrqStatus = CPS_FLD_READ(LPDDR4__INT_STATUS_INIT__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__INT_STATUS_INIT__REG)));
	/* Checked interrupt status related to mode register settings */
	else if ((intr >= LPDDR4_MODE_MRR_ERROR) && (intr <= LPDDR4_MODE_WRITE_REQ_DONE))
		*ctlGrpIrqStatus = CPS_REG_READ(&(ctlRegBase->LPDDR4__INT_STATUS_MODE__REG));
	else if (intr == LPDDR4_BIST_DONE)
		*ctlGrpIrqStatus = CPS_FLD_READ(LPDDR4__INT_STATUS_BIST__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__INT_STATUS_BIST__REG)));
	else if (intr == LPDDR4_PARITY_ERROR)
		*ctlGrpIrqStatus = CPS_FLD_READ(LPDDR4__INT_STATUS_PARITY__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__INT_STATUS_PARITY__REG)));
	else
		*CtlMasterIntFlag = 1;
}

static void lpddr4_checkctlinterrupt_3(LPDDR4_CtlRegs* ctlRegBase, lpddr4_ctlinterrupt intr,
				       uint32_t* ctlGrpIrqStatus, uint32_t* CtlMasterIntFlag)
{

	/* Check interrupt status for the interrupt source.*/
	/* Checked interrupt status related to frequency settings */
	if ((intr >= LPDDR4_FREQ_DFS_REQ_HW_IGNORE) && (intr <= LPDDR4_FREQ_DFS_SW_DONE))
		*ctlGrpIrqStatus = CPS_FLD_READ(LPDDR4__INT_STATUS_FREQ__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__INT_STATUS_FREQ__REG)));
	/* Checked interrupt status related to low power mode */
	else if ((intr >= LPDDR4_LP_DONE) && (intr <= LPDDR4_LP_TIMEOUT))
		*ctlGrpIrqStatus = CPS_FLD_READ(LPDDR4__INT_STATUS_LOWPOWER__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__INT_STATUS_LOWPOWER__REG)));
	else
		/*Splitted interrupt checking into sub functions due to MISRA rules*/
		lpddr4_checkctlinterrupt_4(ctlRegBase, intr, ctlGrpIrqStatus, CtlMasterIntFlag);
}

static void lpddr4_checkctlinterrupt_2(LPDDR4_CtlRegs* ctlRegBase, lpddr4_ctlinterrupt intr,
				       uint32_t* ctlGrpIrqStatus, uint32_t* CtlMasterIntFlag)
{

	/* Check interrupt status for the interrupt source.*/
	/* Checked interrupt status related to timeout operation */
	if (intr <= LPDDR4_TIMEOUT_AUTO_REFRESH_MAX)
		*ctlGrpIrqStatus = CPS_REG_READ(&(ctlRegBase->LPDDR4__INT_STATUS_TIMEOUT__REG));
	/* Checked interrupt status related to training operation */
	else if ((intr >= LPDDR4_TRAINING_ZQ_STATUS) && (intr <= LPDDR4_TRAINING_DQS_OSC_VAR_OUT))
		*ctlGrpIrqStatus = CPS_REG_READ(&(ctlRegBase->LPDDR4__INT_STATUS_TRAINING__REG));
	/* Checked interrupt status related to user interface settings */
	else if ((intr >= LPDDR4_USERIF_OUTSIDE_MEM_ACCESS) && (intr <= LPDDR4_USERIF_INVAL_SETTING))
		*ctlGrpIrqStatus = CPS_REG_READ(&(ctlRegBase->LPDDR4__INT_STATUS_USERIF__REG));
	/* Checked interrupt status related to misc settings */
	else if ((intr >= LPDDR4_MISC_MRR_TRAFFIC) && (intr <= LPDDR4_MISC_REFRESH_STATUS))
		*ctlGrpIrqStatus = CPS_FLD_READ(LPDDR4__INT_STATUS_MISC__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__INT_STATUS_MISC__REG)));
	/* Checked interrupt status related to DFI settings */
	else if ((intr >= LPDDR4_DFI_UPDATE_ERROR) && (intr <= LPDDR4_DFI_TIMEOUT))
		*ctlGrpIrqStatus = CPS_FLD_READ(LPDDR4__INT_STATUS_DFI__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__INT_STATUS_DFI__REG)));
	else
		/*Splitted interrupt checking into sub functions due to MISRA rules*/
		lpddr4_checkctlinterrupt_3(ctlRegBase, intr, ctlGrpIrqStatus, CtlMasterIntFlag);
}

uint32_t lpddr4_checkctlinterrupt(const lpddr4_privatedata* pd, lpddr4_ctlinterrupt intr, bool* irqstatus)
{
	uint32_t result;
	uint32_t ctlMasterIrqStatus = 0U;
	uint32_t ctlGrpIrqStatus = 0U;
	uint32_t CtlMasterIntFlag = 0U;

	/* NOTE:This function assume irq status is mentioned in master and group register.
	 * This function check the set bit in both the registers master as well as group.
	 * function returns a irqstatus true based on the set bits
	 */
	result = lpddr4_checkctlinterruptSF(pd, intr, irqstatus);
	if (result == (uint32_t)CDN_EOK) {
		LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;
		/* Checked interrupt source bits in master register */
		ctlMasterIrqStatus = (CPS_REG_READ(&(ctlRegBase->LPDDR4__INT_STATUS_MASTER__REG)) & (~(1 << 31)));

		/*Splitted interrupt checking into sub functions due to MISRA rules*/
		lpddr4_checkctlinterrupt_2(ctlRegBase, intr, &ctlGrpIrqStatus,&CtlMasterIntFlag);

		/* MISRA compliance (Shifting operation) check */
		if (CtlIntMap[intr][INT_SHIFT] < WORD_SHIFT) {
			if ((((ctlMasterIrqStatus  >> CtlIntMap[intr][GRP_SHIFT]) & BIT_MASK) > 0U) &&
			    (((ctlGrpIrqStatus >> CtlIntMap[intr][INT_SHIFT]) & BIT_MASK) > 0U) &&
			    (CtlMasterIntFlag == 0))
				*irqstatus = true;
			else if ((((ctlMasterIrqStatus >> CtlIntMap[intr][GRP_SHIFT]) & BIT_MASK) > 0U) &&
				 (CtlMasterIntFlag == 1))
				*irqstatus = true;
			else
				*irqstatus = false;
		}
	}
	return result;
}

static void lpddr4_ackctlinterrupt_4(LPDDR4_CtlRegs* ctlRegBase, lpddr4_ctlinterrupt intr)
{
	uint32_t regval = 0;

	/* Set appropriate ACK bit for respective interrupt source*/
	/* Handled interrupts related to mode register settings */
	if ((intr >= LPDDR4_MODE_MRR_ERROR) && (intr <= LPDDR4_MODE_WRITE_REQ_DONE))
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__INT_ACK_MODE__REG), (uint32_t)BIT_MASK << CtlIntMap[intr][INT_SHIFT]);
	/* Handled interrupts related to BIST functionalities */
	else if (intr == LPDDR4_BIST_DONE) {
		regval = CPS_FLD_WRITE(LPDDR4__INT_ACK_BIST__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__INT_ACK_BIST__REG)),
				       ((uint32_t)BIT_MASK << CtlIntMap[intr][INT_SHIFT]));
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__INT_ACK_BIST__REG), regval);
	}
	/* Handled interrupts related to parity */
	else if (intr == LPDDR4_PARITY_ERROR) {
		regval = CPS_FLD_WRITE(LPDDR4__INT_ACK_PARITY__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__INT_ACK_PARITY__REG)),
				       ((uint32_t)BIT_MASK << CtlIntMap[intr][INT_SHIFT]));
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__INT_ACK_PARITY__REG), regval);
	}else  {
	}
}

static void lpddr4_ackctlinterrupt_3(LPDDR4_CtlRegs* ctlRegBase, lpddr4_ctlinterrupt intr)
{
	uint32_t regval = 0;

	/* Set appropriate ACK bit for respective interrupt source*/
	/* Handled interrupts related to low powermode */
	if ((intr >= LPDDR4_LP_DONE) && (intr <= LPDDR4_LP_TIMEOUT)) {
		regval = CPS_FLD_WRITE(LPDDR4__INT_ACK_LOWPOWER__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__INT_ACK_LOWPOWER__REG)),
				       ((uint32_t)BIT_MASK << CtlIntMap[intr][INT_SHIFT]));
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__INT_ACK_LOWPOWER__REG), regval);
	}
	/* Handled interrupts related to initialization sequence */
	else if ((intr >= LPDDR4_INIT_MEM_RESET_DONE) && (intr <= LPDDR4_INIT_POWER_ON_STATE)) {
		regval = CPS_FLD_WRITE(LPDDR4__INT_ACK_INIT__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__INT_ACK_INIT__REG)),
				       ((uint32_t)BIT_MASK << CtlIntMap[intr][INT_SHIFT]));
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__INT_ACK_INIT__REG), regval);
	}else
		/*Splitted into another function due to misra rules*/
		lpddr4_ackctlinterrupt_4(ctlRegBase, intr);
}

static void  lpddr4_ackctlinterrupt_2(LPDDR4_CtlRegs* ctlRegBase, lpddr4_ctlinterrupt intr)
{
	uint32_t regval = 0;

	/* Set appropriate ACK bit for respective interrupt source*/
	/* Handled interrupts related to DFI interfaces */
	if ((intr >= LPDDR4_DFI_UPDATE_ERROR) && (intr <= LPDDR4_DFI_TIMEOUT))
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__INT_ACK_DFI__REG), (uint32_t)BIT_MASK << CtlIntMap[intr][INT_SHIFT]);
	/* Handled interrupts related to frequency settings */
	else if ((intr >= LPDDR4_FREQ_DFS_REQ_HW_IGNORE) && (intr <= LPDDR4_FREQ_DFS_SW_DONE)) {
		regval = CPS_FLD_WRITE(LPDDR4__INT_ACK_FREQ__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__INT_ACK_FREQ__REG)),
				       ((uint32_t)BIT_MASK << CtlIntMap[intr][INT_SHIFT]));
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__INT_ACK_FREQ__REG), regval);
	}else
		/*Splitted into another function due to misra rules*/
		lpddr4_ackctlinterrupt_3(ctlRegBase, intr);
}

uint32_t lpddr4_ackctlinterrupt(const lpddr4_privatedata* pd, lpddr4_ctlinterrupt intr)
{
	uint32_t result;

	/* NOTE:This function set the respective bit the ACK register based on the interrupt type */
	result = lpddr4_ackctlinterruptSF(pd, intr);
	if (result == (uint32_t)CDN_EOK) {
		LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;
		/* Handled interrupts related to timeout settings */
		if (intr <= LPDDR4_TIMEOUT_AUTO_REFRESH_MAX)
			CPS_REG_WRITE(&(ctlRegBase->LPDDR4__INT_ACK_TIMEOUT__REG), (uint32_t)BIT_MASK << CtlIntMap[intr][INT_SHIFT]);
		/* Handled interrupts related to training and calibration settings */
		else if ((intr >= LPDDR4_TRAINING_ZQ_STATUS) && (intr <= LPDDR4_TRAINING_DQS_OSC_VAR_OUT))
			CPS_REG_WRITE(&(ctlRegBase->LPDDR4__INT_ACK_TRAINING__REG), (uint32_t)BIT_MASK << CtlIntMap[intr][INT_SHIFT]);
		/* Handled interrupts related to user interface settings */
		else if ((intr >= LPDDR4_USERIF_OUTSIDE_MEM_ACCESS) && (intr <= LPDDR4_USERIF_INVAL_SETTING))
			CPS_REG_WRITE(&(ctlRegBase->LPDDR4__INT_ACK_USERIF__REG), (uint32_t)BIT_MASK << CtlIntMap[intr][INT_SHIFT]);
		/* Handled interrupts related to misc settings */
		else if ((intr >= LPDDR4_MISC_MRR_TRAFFIC) && (intr <= LPDDR4_MISC_REFRESH_STATUS))
			CPS_REG_WRITE(&(ctlRegBase->LPDDR4__INT_ACK_MISC__REG), (uint32_t)BIT_MASK << CtlIntMap[intr][INT_SHIFT]);
		else
			/*Splitted into another function due to misra rules*/
			lpddr4_ackctlinterrupt_2(ctlRegBase, intr);
	}
	return result;
}

uint32_t lpddr4_getphyindepinterruptmask(const lpddr4_privatedata* pd, uint32_t* mask)
{
	uint32_t result;

	result = LPDDR4_GetPhyIndepInterruptMSF(pd,  mask);
	if (result == (uint32_t)CDN_EOK) {
		LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;
		/* Reading mask register */
		*mask = CPS_FLD_READ(LPDDR4__PI_INT_MASK__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__PI_INT_MASK__REG)));
	}
	return result;
}

uint32_t lpddr4_setphyindepinterruptmask(const lpddr4_privatedata* pd, const uint32_t* mask)
{
	uint32_t result;
	uint32_t regval = 0;
	const uint32_t ui32IrqCount = (uint32_t)LPDDR4_PHY_INDEP_DLL_LOCK_STATE_CHANGE_BIT + 1U;

	result = LPDDR4_SetPhyIndepInterruptMSF(pd, mask);
	if ((result == (uint32_t)CDN_EOK) && (ui32IrqCount < WORD_SHIFT)) {
		/* Return if the user given value is higher than the field width */
		if (*mask >= (1U << ui32IrqCount))
			result = CDN_EINVAL;
	}
	if (result == (uint32_t)CDN_EOK) {
		LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;

		/* Writing to the user requested interrupt mask */
		regval = CPS_FLD_WRITE(LPDDR4__PI_INT_MASK__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__PI_INT_MASK__REG)), *mask);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__PI_INT_MASK__REG), regval);
	}
	return result;
}

uint32_t lpddr4_checkphyindepinterrupt(const lpddr4_privatedata* pd, lpddr4_phyindepinterrupt intr, bool* irqstatus)
{
	uint32_t result = 0;
	uint32_t phyIndepIrqStatus = 0;

	result = LPDDR4_CheckPhyIndepInterrupSF(pd, intr, irqstatus);
	/* Confirming that the value of interrupt is less than register width */
	if ((result == (uint32_t)CDN_EOK) && ((uint32_t)intr < WORD_SHIFT)) {
		LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;

		/* Reading the requested bit to check interrupt status */
		phyIndepIrqStatus = CPS_REG_READ(&(ctlRegBase->LPDDR4__PI_INT_STATUS__REG));
		*irqstatus = (((phyIndepIrqStatus >> (uint32_t)intr ) & BIT_MASK) > 0U);
	}
	return result;
}

uint32_t lpddr4_ackphyindepinterrupt(const lpddr4_privatedata* pd, lpddr4_phyindepinterrupt intr)
{
	uint32_t result = 0U;
	uint32_t regval = 0U;
	uint32_t ui32ShiftInterrupt = (uint32_t)intr;

	result = lpddr4_ackphyindepinterruptSF(pd, intr);
	/* Confirming that the value of interrupt is less than register width */
	if ((result == (uint32_t)CDN_EOK) && (ui32ShiftInterrupt < WORD_SHIFT)) {
		LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;

		/* Write 1 to the requested bit to ACk the interrupt */
		regval = ((uint32_t)BIT_MASK << ui32ShiftInterrupt);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__PI_INT_ACK__REG), regval);
	}

	return result;
}

/* Check for caTrainingError */
static void lpddr4_checkcatrainingerror(LPDDR4_CtlRegs* ctlRegBase, lpddr4_debuginfo* debugInfo, bool* errFoundPtr)
{

	uint32_t regval;
	uint32_t errBitMask = 0U;
	uint32_t snum;
	volatile uint32_t* regAddress;

	regAddress = (volatile uint32_t*)(&(ctlRegBase->LPDDR4__PHY_ADR_CALVL_OBS1_0__REG));
	errBitMask = (CA_TRAIN_RL) | (NIBBLE_MASK);
	/* PHY_ADR_CALVL_OBS1[4] – Right found
	   PHY_ADR_CALVL_OBS1[5] – left found
	   Both the above fields should be high and below field should be zero.
	   PHY_ADR_CALVL_OBS1[3:0] – calvl_state
	 */
	for (snum = 0U; snum < ASLICE_NUM; snum++) {
		regval = CPS_REG_READ(regAddress);
		if ((regval & errBitMask) != CA_TRAIN_RL) {
			debugInfo->caTraingError = true;
			*errFoundPtr = true;
		}
		regAddress = lpddr4_addoffset(regAddress, (uint32_t)SLICE_WIDTH);
	}
}

/* Check for  wrLvlError */
static void lpddr4_checkwrlvlerror(LPDDR4_CtlRegs* ctlRegBase, lpddr4_debuginfo* debugInfo, bool* errFoundPtr)
{

	uint32_t regval;
	uint32_t errBitMask = 0U;
	uint32_t snum;
	volatile uint32_t* regAddress;

	regAddress = (volatile uint32_t*)(&(ctlRegBase->LPDDR4__PHY_WRLVL_STATUS_OBS_0__REG));
	/* Bit 12 is used for error check */
	errBitMask = (BIT_MASK << 12);
	for (snum = 0U; snum < DSLICE_NUM; snum++) {
		regval = CPS_REG_READ(regAddress);
		if ((regval & errBitMask) != 0U) {
			debugInfo->wrLvlError = true;
			*errFoundPtr = true;
		}
		regAddress = lpddr4_addoffset(regAddress, (uint32_t)SLICE_WIDTH);
	}
}

/* Check for  GateLvlError */
static void lpddr4_checkgatelvlerror(LPDDR4_CtlRegs* ctlRegBase, lpddr4_debuginfo* debugInfo, bool* errFoundPtr)
{

	uint32_t regval;
	uint32_t errBitMask = 0U;
	uint32_t snum;
	volatile uint32_t* regAddress;

	regAddress = (volatile uint32_t*)(&(ctlRegBase->LPDDR4__PHY_GTLVL_STATUS_OBS_0__REG));
	/* PHY_GTLVL_STATUS_OBS[6] – gate_level min error
	 * PHY_GTLVL_STATUS_OBS[7] – gate_level max error
	 * All the above bit fields should be zero */
	errBitMask = GATE_LVL_ERROR_FIELDS;
	for (snum = 0U; snum < DSLICE_NUM; snum++) {
		regval = CPS_REG_READ(regAddress);
		if ((regval & errBitMask) != 0U) {
			debugInfo->gateLvlError = true;
			*errFoundPtr = true;
		}
		regAddress = lpddr4_addoffset(regAddress, (uint32_t)SLICE_WIDTH);
	}
}

/* Check for  ReadLvlError */
static void lpddr4_checkreadlvlerror(LPDDR4_CtlRegs* ctlRegBase, lpddr4_debuginfo* debugInfo, bool* errFoundPtr)
{

	uint32_t regval;
	uint32_t errBitMask = 0U;
	uint32_t snum;
	volatile uint32_t* regAddress;

	regAddress = (volatile uint32_t*)(&(ctlRegBase->LPDDR4__PHY_RDLVL_STATUS_OBS_0__REG));
	/* PHY_RDLVL_STATUS_OBS[23:16] – failed bits : should be zero.
	   PHY_RDLVL_STATUS_OBS[31:28] – rdlvl_state : should be zero */
	errBitMask = READ_LVL_ERROR_FIELDS;
	for (snum = 0U; snum < DSLICE_NUM; snum++) {
		regval = CPS_REG_READ(regAddress);
		if ((regval & errBitMask) != 0U) {
			debugInfo->readLvlError = true;
			*errFoundPtr = true;
		}
		regAddress = lpddr4_addoffset(regAddress, (uint32_t)SLICE_WIDTH);
	}
}

/* Check for  DqTrainingError */
static void lpddr4_checkdqtrainingerror(LPDDR4_CtlRegs* ctlRegBase, lpddr4_debuginfo* debugInfo, bool* errFoundPtr)
{

	uint32_t regval;
	uint32_t errBitMask = 0U;
	uint32_t snum;
	volatile uint32_t* regAddress;

	regAddress = (volatile uint32_t*)(&(ctlRegBase->LPDDR4__PHY_WDQLVL_STATUS_OBS_0__REG));
	/* PHY_WDQLVL_STATUS_OBS[26:18] should all be zero. */
	errBitMask = DQ_LVL_STATUS;
	for (snum = 0U; snum < DSLICE_NUM; snum++) {
		regval = CPS_REG_READ(regAddress);
		if ((regval & errBitMask) != 0U) {
			debugInfo->dqTrainingError = true;
			*errFoundPtr = true;
		}
		regAddress = lpddr4_addoffset(regAddress, (uint32_t)SLICE_WIDTH);
	}
}

/**
 * Internal Function:For checking errors in training/levelling sequence.
 * @param[in] pd Driver state info specific to this instance.
 * @param[in] debugInfo pointer to debug information.
 * @param[out] errFoundPtr pointer to return if error found.
 * @return CDN_EOK on success (Interrupt status high).
 * @return CDN_EINVAL checking or unmasking was not successful.
 */
static bool lpddr4_checklvlerrors(const lpddr4_privatedata* pd, lpddr4_debuginfo* debugInfo, bool errFound)
{

	bool localErrFound = errFound;

	LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;

	if (localErrFound == false)
		/* Check for ca training error */
		lpddr4_checkcatrainingerror(ctlRegBase, debugInfo, &localErrFound);

	if (localErrFound == false)
		/* Check for Write leveling error */
		lpddr4_checkwrlvlerror(ctlRegBase, debugInfo, &localErrFound);

	if (localErrFound == false)
		/* Check for Gate leveling error */
		lpddr4_checkgatelvlerror(ctlRegBase, debugInfo, &localErrFound);

	if (localErrFound == false)
		/* Check for Read leveling error */
		lpddr4_checkreadlvlerror(ctlRegBase, debugInfo, &localErrFound);

	if (localErrFound == false)
		/* Check for DQ training error */
		lpddr4_checkdqtrainingerror(ctlRegBase, debugInfo, &localErrFound);
	return localErrFound;
}

static bool lpddr4_seterror(volatile uint32_t* reg, uint32_t errBitMask, bool* errFoundPtr, const uint32_t errorInfoBits)
{

	uint32_t regval = 0U;

	/* Read the respective observation register */
	regval = CPS_REG_READ(reg);
	/* Compare the error bit values */
	if ((regval & errBitMask) != errorInfoBits)
		*errFoundPtr = true;
	return *errFoundPtr;
}

static void lpddr4_seterrors(LPDDR4_CtlRegs* ctlRegBase,  lpddr4_debuginfo* debugInfo, bool* errFoundPtr)
{

	uint32_t errBitMask = (BIT_MASK << 0x1U) | (BIT_MASK);

	/* Check PLL observation registers for PLL lock errors */

	debugInfo->pllError = lpddr4_seterror(&(ctlRegBase->LPDDR4__PHY_PLL_OBS_0__REG),
					      errBitMask, errFoundPtr, PLL_READY);
	if (*errFoundPtr == false)
		debugInfo->pllError = lpddr4_seterror(&(ctlRegBase->LPDDR4__PHY_PLL_OBS_1__REG),
						      errBitMask, errFoundPtr, PLL_READY);

	/* Check for IO Calibration errors */
	if (*errFoundPtr == false)
		debugInfo->ioCalibError = lpddr4_seterror(&(ctlRegBase->LPDDR4__PHY_CAL_RESULT_OBS_0__REG),
							  IO_CALIB_DONE, errFoundPtr, IO_CALIB_DONE);
	if (*errFoundPtr == false)
		debugInfo->ioCalibError = lpddr4_seterror(&(ctlRegBase->LPDDR4__PHY_CAL_RESULT2_OBS_0__REG),
							  IO_CALIB_DONE, errFoundPtr, IO_CALIB_DONE);
	if (*errFoundPtr == false)
		debugInfo->ioCalibError = lpddr4_seterror(&(ctlRegBase->LPDDR4__PHY_CAL_RESULT3_OBS_0__REG),
							  IO_CALIB_FIELD, errFoundPtr, IO_CALIB_STATE);
}

static void lpddr4_setphysnapsettings(LPDDR4_CtlRegs* ctlRegBase, const bool errorFound)
{

	uint32_t snum = 0U;
	volatile uint32_t* regAddress;
	uint32_t regval = 0U;

	/* Setting SC_PHY_SNAP_OBS_REGS_x to get a snapshot*/
	if (errorFound == false) {
		regAddress = (volatile uint32_t*)(&(ctlRegBase->LPDDR4__SC_PHY_SNAP_OBS_REGS_0__REG));
		/* Iterate through each PHY Data Slice */
		for (snum = 0U; snum < DSLICE_NUM; snum++) {
			regval = CPS_FLD_SET(LPDDR4__SC_PHY_SNAP_OBS_REGS_0__FLD,CPS_REG_READ(regAddress));
			CPS_REG_WRITE(regAddress, regval);
			regAddress = lpddr4_addoffset(regAddress, (uint32_t)SLICE_WIDTH);
		}
	}
}

static void lpddr4_setphyadrsnapsettings(LPDDR4_CtlRegs* ctlRegBase, const bool errorFound)
{

	uint32_t snum = 0U;
	volatile uint32_t* regAddress;
	uint32_t regval = 0U;

	/* Setting SC_PHY ADR_SNAP_OBS_REGS_x to get a snapshot*/
	if (errorFound == false) {
		regAddress = (volatile uint32_t*)(&(ctlRegBase->LPDDR4__SC_PHY_ADR_SNAP_OBS_REGS_0__REG));
		/* Iterate through each PHY Address Slice */
		for (snum = 0U; snum < ASLICE_NUM; snum++) {
			regval = CPS_FLD_SET(LPDDR4__SC_PHY_ADR_SNAP_OBS_REGS_0__FLD,CPS_REG_READ(regAddress));
			CPS_REG_WRITE(regAddress, regval);
			regAddress = lpddr4_addoffset(regAddress, (uint32_t)SLICE_WIDTH);
		}
	}
}

static void lpddr4_setsettings(LPDDR4_CtlRegs* ctlRegBase, const bool errorFound)
{

	/* Calling functions to enable snap shots of OBS registers */
	lpddr4_setphysnapsettings(ctlRegBase, errorFound);
	lpddr4_setphyadrsnapsettings(ctlRegBase, errorFound);
}

uint32_t lpddr4_getdebuginitinfo(const lpddr4_privatedata* pd, lpddr4_debuginfo* debugInfo)
{

	uint32_t result = 0U;
	bool errorFound = false;

	/* Calling Sanity Function to verify the input variables*/
	result = lpddr4_getdebuginitinfoSF(pd, debugInfo);
	if (result == (uint32_t)CDN_EOK) {

		LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;
		lpddr4_seterrors(ctlRegBase, debugInfo, &errorFound);
		/* Function to setup Snap for OBS registers */
		lpddr4_setsettings(ctlRegBase, errorFound);
		/* Function Check various levelling errors */
		errorFound = lpddr4_checklvlerrors(pd, debugInfo, errorFound);
	}

	if (errorFound == true)
		result = (uint32_t)CDN_EPROTO;

	return result;
}

static void readpdwakeup(const lpddr4_ctlfspnum* fspNum, LPDDR4_CtlRegs* ctlRegBase, uint32_t* cycles)
{

	/* Read the appropriate register, based on user given frequency.*/
	if (*fspNum == LPDDR4_FSP_0)
		*cycles = CPS_FLD_READ(LPDDR4__LPI_PD_WAKEUP_F0__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_PD_WAKEUP_F0__REG)));
	else if (*fspNum == LPDDR4_FSP_1)
		*cycles = CPS_FLD_READ(LPDDR4__LPI_PD_WAKEUP_F1__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_PD_WAKEUP_F1__REG)));
	else
		/* Default register (sanity function already confirmed the variable value) */
		*cycles = CPS_FLD_READ(LPDDR4__LPI_PD_WAKEUP_F2__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_PD_WAKEUP_F2__REG)));
}

static void readsrshortwakeup(const lpddr4_ctlfspnum* fspNum, LPDDR4_CtlRegs* ctlRegBase, uint32_t* cycles)
{

	/* Read the appropriate register, based on user given frequency.*/
	if (*fspNum == LPDDR4_FSP_0)
		*cycles = CPS_FLD_READ(LPDDR4__LPI_SR_SHORT_WAKEUP_F0__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SR_SHORT_WAKEUP_F0__REG)));
	else if (*fspNum == LPDDR4_FSP_1)
		*cycles = CPS_FLD_READ(LPDDR4__LPI_SR_SHORT_WAKEUP_F1__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SR_SHORT_WAKEUP_F1__REG)));
	else
		/* Default register (sanity function already confirmed the variable value) */
		*cycles = CPS_FLD_READ(LPDDR4__LPI_SR_SHORT_WAKEUP_F2__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SR_SHORT_WAKEUP_F2__REG)));
}

static void readsrlongwakeup(const lpddr4_ctlfspnum* fspNum, LPDDR4_CtlRegs* ctlRegBase, uint32_t* cycles)
{

	/* Read the appropriate register, based on user given frequency.*/
	if (*fspNum == LPDDR4_FSP_0)
		*cycles = CPS_FLD_READ(LPDDR4__LPI_SR_LONG_WAKEUP_F0__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SR_LONG_WAKEUP_F0__REG)));
	else if (*fspNum == LPDDR4_FSP_1)
		*cycles = CPS_FLD_READ(LPDDR4__LPI_SR_LONG_WAKEUP_F1__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SR_LONG_WAKEUP_F1__REG)));
	else
		/* Default register (sanity function already confirmed the variable value) */
		*cycles = CPS_FLD_READ(LPDDR4__LPI_SR_LONG_WAKEUP_F2__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SR_LONG_WAKEUP_F2__REG)));
}

static void readsrlonggatewakeup(const lpddr4_ctlfspnum* fspNum, LPDDR4_CtlRegs* ctlRegBase, uint32_t* cycles)
{

	/* Read the appropriate register, based on user given frequency.*/
	if (*fspNum == LPDDR4_FSP_0)
		*cycles = CPS_FLD_READ(LPDDR4__LPI_SR_LONG_MCCLK_GATE_WAKEUP_F0__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SR_LONG_MCCLK_GATE_WAKEUP_F0__REG)));
	else if (*fspNum == LPDDR4_FSP_1)
		*cycles = CPS_FLD_READ(LPDDR4__LPI_SR_LONG_MCCLK_GATE_WAKEUP_F1__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SR_LONG_MCCLK_GATE_WAKEUP_F1__REG)));
	else
		/* Default register (sanity function already confirmed the variable value) */
		*cycles = CPS_FLD_READ(LPDDR4__LPI_SR_LONG_MCCLK_GATE_WAKEUP_F2__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SR_LONG_MCCLK_GATE_WAKEUP_F2__REG)));
}

static void readsrdpshortwakeup(const lpddr4_ctlfspnum* fspNum, LPDDR4_CtlRegs* ctlRegBase, uint32_t* cycles)
{

	/* Read the appropriate register, based on user given frequency.*/
	if (*fspNum == LPDDR4_FSP_0)
		*cycles = CPS_FLD_READ(LPDDR4__LPI_SRPD_SHORT_WAKEUP_F0__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SRPD_SHORT_WAKEUP_F0__REG)));
	else if (*fspNum == LPDDR4_FSP_1)
		*cycles = CPS_FLD_READ(LPDDR4__LPI_SRPD_SHORT_WAKEUP_F1__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SRPD_SHORT_WAKEUP_F1__REG)));
	else
		/* Default register (sanity function already confirmed the variable value) */
		*cycles = CPS_FLD_READ(LPDDR4__LPI_SRPD_SHORT_WAKEUP_F2__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SRPD_SHORT_WAKEUP_F2__REG)));
}

static void readsrdplongwakeup(const lpddr4_ctlfspnum* fspNum, LPDDR4_CtlRegs* ctlRegBase, uint32_t* cycles)
{

	/* Read the appropriate register, based on user given frequency.*/
	if (*fspNum == LPDDR4_FSP_0)
		*cycles = CPS_FLD_READ(LPDDR4__LPI_SRPD_LONG_WAKEUP_F0__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SRPD_LONG_WAKEUP_F0__REG)));
	else if (*fspNum == LPDDR4_FSP_1)
		*cycles = CPS_FLD_READ(LPDDR4__LPI_SRPD_LONG_WAKEUP_F1__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SRPD_LONG_WAKEUP_F1__REG)));
	else
		/* Default register (sanity function already confirmed the variable value) */
		*cycles = CPS_FLD_READ(LPDDR4__LPI_SRPD_LONG_WAKEUP_F2__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SRPD_LONG_WAKEUP_F2__REG)));
}

static void readsrdplonggatewakeup(const lpddr4_ctlfspnum* fspNum, LPDDR4_CtlRegs* ctlRegBase, uint32_t* cycles)
{

	/* Read the appropriate register, based on user given frequency.*/
	if (*fspNum == LPDDR4_FSP_0)
		*cycles = CPS_FLD_READ(LPDDR4__LPI_SRPD_LONG_MCCLK_GATE_WAKEUP_F0__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SRPD_LONG_MCCLK_GATE_WAKEUP_F0__REG)));
	else if (*fspNum == LPDDR4_FSP_1)
		*cycles = CPS_FLD_READ(LPDDR4__LPI_SRPD_LONG_MCCLK_GATE_WAKEUP_F1__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SRPD_LONG_MCCLK_GATE_WAKEUP_F1__REG)));
	else
		/* Default register (sanity function already confirmed the variable value) */
		*cycles = CPS_FLD_READ(LPDDR4__LPI_SRPD_LONG_MCCLK_GATE_WAKEUP_F2__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SRPD_LONG_MCCLK_GATE_WAKEUP_F2__REG)));

}

static void lpddr4_readlpiwakeuptime(LPDDR4_CtlRegs* ctlRegBase, const lpddr4_lpiwakeupparam* lpiWakeUpParam, const lpddr4_ctlfspnum* fspNum, uint32_t* cycles)
{

	/* Iterate through each of the Wake up parameter type */
	if (*lpiWakeUpParam == LPDDR4_LPI_PD_WAKEUP_FN)
		/* Calling appropriate function for register read */
		readpdwakeup(fspNum, ctlRegBase, cycles);
	else if (*lpiWakeUpParam == LPDDR4_LPI_SR_SHORT_WAKEUP_FN)
		readsrshortwakeup(fspNum, ctlRegBase, cycles);
	else if (*lpiWakeUpParam == LPDDR4_LPI_SR_LONG_WAKEUP_FN)
		readsrlongwakeup(fspNum, ctlRegBase, cycles);
	else if (*lpiWakeUpParam == LPDDR4_LPI_SR_LONG_MCCLK_GATE_WAKEUP_FN)
		readsrlonggatewakeup(fspNum, ctlRegBase, cycles);
	else if (*lpiWakeUpParam == LPDDR4_LPI_SRPD_SHORT_WAKEUP_FN)
		readsrdpshortwakeup(fspNum, ctlRegBase, cycles);
	else if (*lpiWakeUpParam == LPDDR4_LPI_SRPD_LONG_WAKEUP_FN)
		readsrdplongwakeup(fspNum, ctlRegBase, cycles);
	else
		/* Default function (sanity function already confirmed the variable value) */
		readsrdplonggatewakeup(fspNum, ctlRegBase, cycles);
}

uint32_t lpddr4_getlpiwakeuptime(const lpddr4_privatedata* pd, const lpddr4_lpiwakeupparam* lpiWakeUpParam, const lpddr4_ctlfspnum* fspNum, uint32_t* cycles)
{

	uint32_t result = 0U;

	/* Calling Sanity Function to verify the input variables*/
	result = lpddr4_getlpiwakeuptimeSF(pd, lpiWakeUpParam, fspNum, cycles);
	if (result == (uint32_t)CDN_EOK) {
		LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;
		lpddr4_readlpiwakeuptime(ctlRegBase, lpiWakeUpParam, fspNum, cycles);
	}
	return result;
}

static void writepdwakeup(const lpddr4_ctlfspnum* fspNum, LPDDR4_CtlRegs* ctlRegBase, const uint32_t* cycles)
{

	uint32_t regval = 0U;

	/* Write to appropriate register ,based on user given frequency.*/
	if (*fspNum == LPDDR4_FSP_0) {
		regval = CPS_FLD_WRITE(LPDDR4__LPI_PD_WAKEUP_F0__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_PD_WAKEUP_F0__REG)), *cycles);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__LPI_PD_WAKEUP_F0__REG), regval);
	} else if (*fspNum == LPDDR4_FSP_1) {
		regval = CPS_FLD_WRITE(LPDDR4__LPI_PD_WAKEUP_F1__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_PD_WAKEUP_F1__REG)), *cycles);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__LPI_PD_WAKEUP_F1__REG), regval);
	} else {
		/* Default register (sanity function already confirmed the variable value) */
		regval = CPS_FLD_WRITE(LPDDR4__LPI_PD_WAKEUP_F2__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_PD_WAKEUP_F2__REG)), *cycles);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__LPI_PD_WAKEUP_F2__REG), regval);
	}
}

static void writesrshortwakeup(const lpddr4_ctlfspnum* fspNum, LPDDR4_CtlRegs* ctlRegBase, const uint32_t* cycles)
{

	uint32_t regval = 0U;

	/* Write to appropriate register ,based on user given frequency.*/
	if (*fspNum == LPDDR4_FSP_0) {
		regval = CPS_FLD_WRITE(LPDDR4__LPI_SR_SHORT_WAKEUP_F0__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SR_SHORT_WAKEUP_F0__REG)), *cycles);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__LPI_SR_SHORT_WAKEUP_F0__REG), regval);
	} else if (*fspNum == LPDDR4_FSP_1) {
		regval = CPS_FLD_WRITE(LPDDR4__LPI_SR_SHORT_WAKEUP_F1__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SR_SHORT_WAKEUP_F1__REG)), *cycles);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__LPI_SR_SHORT_WAKEUP_F1__REG), regval);
	} else {
		/* Default register (sanity function already confirmed the variable value) */
		regval = CPS_FLD_WRITE(LPDDR4__LPI_SR_SHORT_WAKEUP_F2__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SR_SHORT_WAKEUP_F2__REG)), *cycles);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__LPI_SR_SHORT_WAKEUP_F2__REG), regval);
	}
}

static void writesrlongwakeup(const lpddr4_ctlfspnum* fspNum, LPDDR4_CtlRegs* ctlRegBase, const uint32_t* cycles)
{

	uint32_t regval = 0U;

	/* Write to appropriate register ,based on user given frequency.*/
	if (*fspNum == LPDDR4_FSP_0) {
		regval = CPS_FLD_WRITE(LPDDR4__LPI_SR_LONG_WAKEUP_F0__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SR_LONG_WAKEUP_F0__REG)), *cycles);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__LPI_SR_LONG_WAKEUP_F0__REG), regval);
	} else if (*fspNum == LPDDR4_FSP_1) {
		regval = CPS_FLD_WRITE(LPDDR4__LPI_SR_LONG_WAKEUP_F1__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SR_LONG_WAKEUP_F1__REG)), *cycles);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__LPI_SR_LONG_WAKEUP_F1__REG), regval);
	} else {
		/* Default register (sanity function already confirmed the variable value) */
		regval = CPS_FLD_WRITE(LPDDR4__LPI_SR_LONG_WAKEUP_F2__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SR_LONG_WAKEUP_F2__REG)), *cycles);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__LPI_SR_LONG_WAKEUP_F2__REG), regval);
	}
}

static void writesrlonggatewakeup(const lpddr4_ctlfspnum* fspNum, LPDDR4_CtlRegs* ctlRegBase, const uint32_t* cycles)
{

	uint32_t regval = 0U;

	/* Write to appropriate register ,based on user given frequency.*/
	if (*fspNum == LPDDR4_FSP_0) {
		regval = CPS_FLD_WRITE(LPDDR4__LPI_SR_LONG_MCCLK_GATE_WAKEUP_F0__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SR_LONG_MCCLK_GATE_WAKEUP_F0__REG)), *cycles);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__LPI_SR_LONG_MCCLK_GATE_WAKEUP_F0__REG), regval);
	} else if (*fspNum == LPDDR4_FSP_1) {
		regval = CPS_FLD_WRITE(LPDDR4__LPI_SR_LONG_MCCLK_GATE_WAKEUP_F1__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SR_LONG_MCCLK_GATE_WAKEUP_F1__REG)), *cycles);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__LPI_SR_LONG_MCCLK_GATE_WAKEUP_F1__REG), regval);
	} else {
		/* Default register (sanity function already confirmed the variable value) */
		regval = CPS_FLD_WRITE(LPDDR4__LPI_SR_LONG_MCCLK_GATE_WAKEUP_F2__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SR_LONG_MCCLK_GATE_WAKEUP_F2__REG)), *cycles);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__LPI_SR_LONG_MCCLK_GATE_WAKEUP_F2__REG), regval);
	}
}

static void writesrdpshortwakeup(const lpddr4_ctlfspnum* fspNum, LPDDR4_CtlRegs* ctlRegBase, const uint32_t* cycles)
{

	uint32_t regval = 0U;

	/* Write to appropriate register ,based on user given frequency.*/
	if (*fspNum == LPDDR4_FSP_0) {
		regval = CPS_FLD_WRITE(LPDDR4__LPI_SRPD_SHORT_WAKEUP_F0__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SRPD_SHORT_WAKEUP_F0__REG)), *cycles);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__LPI_SRPD_SHORT_WAKEUP_F0__REG), regval);
	} else if (*fspNum == LPDDR4_FSP_1) {
		regval = CPS_FLD_WRITE(LPDDR4__LPI_SRPD_SHORT_WAKEUP_F1__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SRPD_SHORT_WAKEUP_F1__REG)), *cycles);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__LPI_SRPD_SHORT_WAKEUP_F1__REG), regval);
	} else {
		/* Default register (sanity function already confirmed the variable value) */
		regval = CPS_FLD_WRITE(LPDDR4__LPI_SRPD_SHORT_WAKEUP_F2__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SRPD_SHORT_WAKEUP_F2__REG)), *cycles);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__LPI_SRPD_SHORT_WAKEUP_F2__REG), regval);
	}
}

static void writesrdplongwakeup(const lpddr4_ctlfspnum* fspNum, LPDDR4_CtlRegs* ctlRegBase, const uint32_t* cycles)
{

	uint32_t regval = 0U;

	/* Write to appropriate register ,based on user given frequency.*/
	if (*fspNum == LPDDR4_FSP_0) {
		regval = CPS_FLD_WRITE(LPDDR4__LPI_SRPD_LONG_WAKEUP_F0__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SRPD_LONG_WAKEUP_F0__REG)), *cycles);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__LPI_SRPD_LONG_WAKEUP_F0__REG), regval);
	} else if (*fspNum == LPDDR4_FSP_1) {
		regval = CPS_FLD_WRITE(LPDDR4__LPI_SRPD_LONG_WAKEUP_F1__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SRPD_LONG_WAKEUP_F1__REG)), *cycles);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__LPI_SRPD_LONG_WAKEUP_F1__REG), regval);
	} else {
		/* Default register (sanity function already confirmed the variable value) */
		regval = CPS_FLD_WRITE(LPDDR4__LPI_SRPD_LONG_WAKEUP_F2__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SRPD_LONG_WAKEUP_F2__REG)), *cycles);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__LPI_SRPD_LONG_WAKEUP_F2__REG), regval);
	}
}
static void writesrdplonggatewakeup(const lpddr4_ctlfspnum* fspNum, LPDDR4_CtlRegs* ctlRegBase, const uint32_t* cycles)
{

	uint32_t regval = 0U;

	/* Write to appropriate register ,based on user given frequency.*/
	if (*fspNum == LPDDR4_FSP_0) {
		regval = CPS_FLD_WRITE(LPDDR4__LPI_SRPD_LONG_MCCLK_GATE_WAKEUP_F0__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SRPD_LONG_MCCLK_GATE_WAKEUP_F0__REG)), *cycles);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__LPI_SRPD_LONG_MCCLK_GATE_WAKEUP_F0__REG), regval);
	} else if (*fspNum == LPDDR4_FSP_1) {
		regval = CPS_FLD_WRITE(LPDDR4__LPI_SRPD_LONG_MCCLK_GATE_WAKEUP_F1__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SRPD_LONG_MCCLK_GATE_WAKEUP_F1__REG)), *cycles);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__LPI_SRPD_LONG_MCCLK_GATE_WAKEUP_F1__REG), regval);
	} else {
		/* Default register (sanity function already confirmed the variable value) */
		regval = CPS_FLD_WRITE(LPDDR4__LPI_SRPD_LONG_MCCLK_GATE_WAKEUP_F2__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__LPI_SRPD_LONG_MCCLK_GATE_WAKEUP_F2__REG)), *cycles);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__LPI_SRPD_LONG_MCCLK_GATE_WAKEUP_F2__REG), regval);
	}
}

static void lpddr4_writelpiwakeuptime(LPDDR4_CtlRegs* ctlRegBase, const lpddr4_lpiwakeupparam* lpiWakeUpParam, const lpddr4_ctlfspnum* fspNum, const uint32_t* cycles)
{

	/* Iterate through each of the Wake up parameter type */
	if (*lpiWakeUpParam == LPDDR4_LPI_PD_WAKEUP_FN)
		/* Calling appropriate function for register write */
		writepdwakeup(fspNum, ctlRegBase, cycles);
	else if (*lpiWakeUpParam == LPDDR4_LPI_SR_SHORT_WAKEUP_FN)
		writesrshortwakeup(fspNum, ctlRegBase, cycles);
	else if (*lpiWakeUpParam == LPDDR4_LPI_SR_LONG_WAKEUP_FN)
		writesrlongwakeup(fspNum, ctlRegBase, cycles);
	else if (*lpiWakeUpParam == LPDDR4_LPI_SR_LONG_MCCLK_GATE_WAKEUP_FN)
		writesrlonggatewakeup(fspNum, ctlRegBase, cycles);
	else if (*lpiWakeUpParam == LPDDR4_LPI_SRPD_SHORT_WAKEUP_FN)
		writesrdpshortwakeup(fspNum, ctlRegBase, cycles);
	else if (*lpiWakeUpParam == LPDDR4_LPI_SRPD_LONG_WAKEUP_FN)
		writesrdplongwakeup(fspNum, ctlRegBase, cycles);
	else
		/* Default function (sanity function already confirmed the variable value) */
		writesrdplonggatewakeup(fspNum, ctlRegBase, cycles);
}

uint32_t lpddr4_setlpiwakeuptime(const lpddr4_privatedata* pd, const lpddr4_lpiwakeupparam* lpiWakeUpParam, const lpddr4_ctlfspnum* fspNum, const uint32_t* cycles)
{
	uint32_t result = 0U;

	/* Calling Sanity Function to verify the input variables*/
	result = lpddr4_setlpiwakeuptimeSF(pd, lpiWakeUpParam, fspNum, cycles);
	if (result == (uint32_t)CDN_EOK) {
		/* Return if the user given value is higher than the field width */
		if (*cycles > NIBBLE_MASK)
			result = CDN_EINVAL;
	}
	if (result == (uint32_t)CDN_EOK) {
		LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;
		lpddr4_writelpiwakeuptime(ctlRegBase, lpiWakeUpParam, fspNum, cycles);
	}
	return result;
}

uint32_t lpddr4_getreducmode(const lpddr4_privatedata* pd, lpddr4_reducmode* mode)
{
	uint32_t result = 0U;

	/* Calling Sanity Function to verify the input variables*/
	result = lpddr4_getreducmodeSF(pd, mode);
	if (result == (uint32_t)CDN_EOK) {
		LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;
		/* Read the value of reduc parameter. */
		if (CPS_FLD_READ(LPDDR4__MEM_DP_REDUCTION__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__MEM_DP_REDUCTION__REG))) == 0U)
			*mode = LPDDR4_REDUC_ON;
		else
			*mode = LPDDR4_REDUC_OFF;
	}
	return result;
}
uint32_t lpddr4_setreducmode(const lpddr4_privatedata* pd, const lpddr4_reducmode* mode)
{
	uint32_t result = 0U;
	uint32_t regval = 0U;

	/* Calling Sanity Function to verify the input variables*/
	result = lpddr4_setreducmodeSF(pd, mode);
	if (result == (uint32_t)CDN_EOK) {
		LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;
		/* Setting to enable Half data path. */
		regval = CPS_FLD_WRITE(LPDDR4__MEM_DP_REDUCTION__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__MEM_DP_REDUCTION__REG)), *mode);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__MEM_DP_REDUCTION__REG), regval);
	}
	return result;
}

uint32_t lpddr4_getdbireadmode(const lpddr4_privatedata* pd, bool *on_off)
{

	uint32_t result = 0U;

	/* Calling Sanity Function to verify the input variables*/
	result = lpddr4_getdbireadmodeSF(pd, on_off);

	if (result == (uint32_t)CDN_EOK) {
		LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;
		/* Reading the field value from the register.*/
		if (CPS_FLD_READ(LPDDR4__RD_DBI_EN__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__RD_DBI_EN__REG))) == 0U)
			*on_off = false;
		else
			*on_off = true;
	}
	return result;
}

uint32_t lpddr4_getdbiwritemode(const lpddr4_privatedata* pd, bool *on_off)
{

	uint32_t result = 0U;

	/* Calling Sanity Function to verify the input variables*/
	result = lpddr4_getdbireadmodeSF(pd, on_off);

	if (result == (uint32_t)CDN_EOK) {
		LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;
		/* Reading the field value from the register.*/
		if (CPS_FLD_READ(LPDDR4__WR_DBI_EN__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__WR_DBI_EN__REG))) == 0U)
			*on_off = false;
		else
			*on_off = true;
	}
	return result;
}

uint32_t lpddr4_setdbimode(const lpddr4_privatedata* pd, const lpddr4_dbimode* mode)
{

	uint32_t result = 0U;
	uint32_t regval = 0U;

	/* Calling Sanity Function to verify the input variables*/
	result = lpddr4_setdbimodeSF(pd, mode);

	if (result == (uint32_t)CDN_EOK) {
		LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;

		/* Updating the appropriate field value based on the user given mode*/
		if (*mode == LPDDR4_DBI_RD_ON)
			regval = CPS_FLD_WRITE(LPDDR4__RD_DBI_EN__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__RD_DBI_EN__REG)), 1U);
		else if (*mode == LPDDR4_DBI_RD_OFF)
			regval = CPS_FLD_WRITE(LPDDR4__RD_DBI_EN__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__RD_DBI_EN__REG)), 0U);
		else if (*mode == LPDDR4_DBI_WR_ON)
			regval = CPS_FLD_WRITE(LPDDR4__WR_DBI_EN__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__WR_DBI_EN__REG)), 1U);
		else
			/* Default field (Sanity function already confirmed the value to be in expected range.)*/
			regval = CPS_FLD_WRITE(LPDDR4__WR_DBI_EN__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__WR_DBI_EN__REG)), 0U);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__RD_DBI_EN__REG), regval);
	}
	return result;
}

uint32_t lpddr4_getrefreshrate(const lpddr4_privatedata* pd, const lpddr4_ctlfspnum* fspNum, uint32_t* tref, uint32_t* tras_max)
{
	uint32_t result = 0U;

	/* Calling Sanity Function to verify the input variables*/
	result = lpddr4_getrefreshrateSF(pd, fspNum, tref, tras_max);

	if (result == (uint32_t)CDN_EOK) {
		LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;

		/* Selecting the appropriate register for the user requested Frequency*/
		switch (*fspNum) {
		case LPDDR4_FSP_2:
			/* Setting for FSP2 */
			*tref = CPS_FLD_READ(LPDDR4__TREF_F2__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__TREF_F2__REG)));
			*tras_max = CPS_FLD_READ(LPDDR4__TRAS_MAX_F2__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__TRAS_MAX_F2__REG)));
			break;
		case LPDDR4_FSP_1:
			/* Setting for FSP1 */
			*tref = CPS_FLD_READ(LPDDR4__TREF_F1__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__TREF_F1__REG)));
			*tras_max = CPS_FLD_READ(LPDDR4__TRAS_MAX_F1__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__TRAS_MAX_F1__REG)));
			break;
		case LPDDR4_FSP_0:
			/* Setting for FSP0 */
			*tref = CPS_FLD_READ(LPDDR4__TREF_F0__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__TREF_F0__REG)));
			*tras_max = CPS_FLD_READ(LPDDR4__TRAS_MAX_F0__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__TRAS_MAX_F0__REG)));
			break;
		default:
			/* Invalid Frequency number */
			result = (uint32_t)CDN_EINVAL;
			break;
		}
	}
	return result;
}

/* Helper function to update TREF and TRAS MAX parameters for FSP 2*/
static void lpddr4_updatefsp2refrateparams(const lpddr4_privatedata* pd, const uint32_t* tref, const uint32_t* tras_max)
{
	uint32_t regval = 0U;
	LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;

	/* Update tref parameter */
	regval = CPS_FLD_WRITE(LPDDR4__TREF_F2__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__TREF_F2__REG)), *tref);
	CPS_REG_WRITE(&(ctlRegBase->LPDDR4__TREF_F2__REG), regval);
	/* Update tRAS MAX parameter */
	regval = CPS_FLD_WRITE(LPDDR4__TRAS_MAX_F2__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__TRAS_MAX_F2__REG)), *tras_max);
	CPS_REG_WRITE(&(ctlRegBase->LPDDR4__TRAS_MAX_F2__REG), regval);

}

/* Helper function to update TREF and TRAS MAX parameters for FSP 1*/
static void lpddr4_updatefsp1refrateparams(const lpddr4_privatedata* pd, const uint32_t* tref, const uint32_t* tras_max)
{
	uint32_t regval = 0U;
	LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;

	/* Update tref parameter */
	regval = CPS_FLD_WRITE(LPDDR4__TREF_F1__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__TREF_F1__REG)), *tref);
	CPS_REG_WRITE(&(ctlRegBase->LPDDR4__TREF_F1__REG), regval);
	/* Update tRAS MAX parameter */
	regval = CPS_FLD_WRITE(LPDDR4__TRAS_MAX_F1__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__TRAS_MAX_F1__REG)), *tras_max);
	CPS_REG_WRITE(&(ctlRegBase->LPDDR4__TRAS_MAX_F1__REG), regval);;

}

/* Helper function to update TREF and TRAS MAX parameters for FSP 0*/
static void lpddr4_updatefsp0refrateparams(const lpddr4_privatedata* pd, const uint32_t* tref, const uint32_t* tras_max)
{
	uint32_t regval = 0U;
	LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;

	/* Update tref parameter */
	regval = CPS_FLD_WRITE(LPDDR4__TREF_F0__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__TREF_F0__REG)), *tref);
	CPS_REG_WRITE(&(ctlRegBase->LPDDR4__TREF_F0__REG), regval);
	/* Update tRAS MAX parameter */
	regval = CPS_FLD_WRITE(LPDDR4__TRAS_MAX_F0__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__TRAS_MAX_F0__REG)), *tras_max);
	CPS_REG_WRITE(&(ctlRegBase->LPDDR4__TRAS_MAX_F0__REG), regval);

}

uint32_t lpddr4_setrefreshrate(const lpddr4_privatedata* pd, const lpddr4_ctlfspnum* fspNum, const uint32_t* tref, const uint32_t* tras_max)
{
	uint32_t result = 0U;

	/* Calling Sanity Function to verify the input variables*/
	result = lpddr4_setrefreshrateSF(pd, fspNum, tref, tras_max);

	if (result == (uint32_t)CDN_EOK) {

		/* Selecting the appropriate register for the user requested Frequency*/
		switch (*fspNum) {
		case LPDDR4_FSP_2:
			lpddr4_updatefsp2refrateparams (pd, tref, tras_max);
			break;
		case LPDDR4_FSP_1:
			lpddr4_updatefsp1refrateparams ( pd, tref, tras_max);
			break;
		default:
			/* FSP_0 is considered as the default (sanity check already confirmed it as valid FSP) */
			lpddr4_updatefsp0refrateparams ( pd, tref,  tras_max);
			break;
		}
	}
	return result;
}

uint32_t lpddr4_refreshperchipselect(const lpddr4_privatedata* pd, const uint32_t trefInterval)
{
	uint32_t result = 0U;
	uint32_t regval = 0U;

	/* Calling Sanity Function to verify the input variables*/
	result = lpddr4_refreshperchipselectSF(pd);

	if (result == (uint32_t)CDN_EOK) {
		LPDDR4_CtlRegs* ctlRegBase = (LPDDR4_CtlRegs*)pd->ctlBase;
		/* Setting tref_interval parameter to enable/disable Refresh per chip select. */
		regval = CPS_FLD_WRITE(LPDDR4__TREF_INTERVAL__FLD, CPS_REG_READ(&(ctlRegBase->LPDDR4__TREF_INTERVAL__REG)), trefInterval);
		CPS_REG_WRITE(&(ctlRegBase->LPDDR4__TREF_INTERVAL__REG), regval);
	}
	return result;
}
