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
***********************************************************************
* Cadence Core Driver for LPDDR4.
**********************************************************************/

#ifndef LPDDR4_PRIV_H
#define LPDDR4_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif

#define PRODUCT_ID (0x1046U)
#define VERSION_0  (0x5d1af3c3U)
#define VERSION_1  (0x0171a610U)

#define BIT_MASK    (0x1U)
#define BYTE_MASK   (0xffU)
#define NIBBLE_MASK (0xfU)

#define WORD_SHIFT (32U)
#define WORD_MASK (0xffffffffU)
#define SLICE_WIDTH (0x100)
/* Number of Data slices */
#define DSLICE_NUM (2U)
/*Number of Address Slices */
#define ASLICE_NUM (3U)

/* Number of accessible registers in each slice */
#define DSLICE0_REG_COUNT  (126U)
#define DSLICE1_REG_COUNT  (126U)
#define DSLICE2_REG_COUNT  (140U)
#define DSLICE3_REG_COUNT  (140U)
#define ASLICE0_REG_COUNT  (42U)
#define PHY_CORE_REG_COUNT (126U)

#define CTL_OFFSET 0
#define PI_OFFSET (((uint32_t)1) <<  11)
#define PHY_OFFSET (((uint32_t)1) << 12)

/* BIT[17] on INT_MASK_1 register. */
#define CTL_INT_MASK_ALL ((uint32_t)LPDDR4_LOR_BITS - WORD_SHIFT)

/* Init Error information bits */
#define PLL_READY (0x3U)
#define IO_CALIB_DONE ((uint32_t)0x1U << 23U)
#define IO_CALIB_FIELD ((uint32_t)NIBBLE_MASK << 28U)
#define IO_CALIB_STATE ((uint32_t)0xBU << 28U)
#define RX_CAL_DONE ((uint32_t)BIT_MASK << 4U)
#define CA_TRAIN_RL (((uint32_t)BIT_MASK << 5U) | ((uint32_t)BIT_MASK << 4U))
#define WR_LVL_STATE (((uint32_t)NIBBLE_MASK) << 13U)
#define GATE_LVL_ERROR_FIELDS (((uint32_t)BIT_MASK << 7U) | ((uint32_t)BIT_MASK << 6U))
#define READ_LVL_ERROR_FIELDS ((((uint32_t)NIBBLE_MASK) << 28U) | (((uint32_t)BYTE_MASK) << 16U))
#define DQ_LVL_STATUS (((uint32_t)BIT_MASK << 26U) | (((uint32_t)BYTE_MASK) << 18U))

#ifdef __cplusplus
}
#endif

#endif  /* LPDDR4_PRIV_H */
