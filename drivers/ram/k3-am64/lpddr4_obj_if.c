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

#include "lpddr4_obj_if.h"

/* parasoft suppress item METRICS-41-3 "Number of blocks of comments per statement" */

LPDDR4_OBJ *lpddr4_getinstance(void)
{
	static LPDDR4_OBJ driver =
	{
		.probe				= lpddr4_probe,
		.init				= lpddr4_init,
		.start				= lpddr4_start,
		.readreg			= lpddr4_readreg,
		.writereg			= lpddr4_writereg,
		.getmmrregister			= lpddr4_getmmrregister,
		.setmmrregister			= lpddr4_setmmrregister,
		.writectlconfig			= lpddr4_writectlconfig,
		.writephyconfig			= lpddr4_writephyconfig,
		.writephyindepconfig		= lpddr4_writephyindepconfig,
		.readctlconfig			= lpddr4_readctlconfig,
		.readphyconfig			= lpddr4_readphyconfig,
		.readphyindepconfig		= lpddr4_readphyindepconfig,
		.getctlinterruptmask		= lpddr4_getctlinterruptmask,
		.setctlinterruptmask		= lpddr4_setctlinterruptmask,
		.checkctlinterrupt		= lpddr4_checkctlinterrupt,
		.ackctlinterrupt		= lpddr4_ackctlinterrupt,
		.getphyindepinterruptmask	= lpddr4_getphyindepinterruptmask,
		.setphyindepinterruptmask	= lpddr4_setphyindepinterruptmask,
		.checkphyindepinterrupt		= lpddr4_checkphyindepinterrupt,
		.ackphyindepinterrupt		= lpddr4_ackphyindepinterrupt,
		.getdebuginitinfo		= lpddr4_getdebuginitinfo,
		.getlpiwakeuptime		= lpddr4_getlpiwakeuptime,
		.setlpiwakeuptime		= lpddr4_setlpiwakeuptime,
		.getreducmode			= lpddr4_getreducmode,
		.setreducmode			= lpddr4_setreducmode,
		.getdbireadmode			= lpddr4_getdbireadmode,
		.getdbiwritemode		= lpddr4_getdbiwritemode,
		.setdbimode			= lpddr4_setdbimode,
		.getrefreshrate			= lpddr4_getrefreshrate,
		.setrefreshrate			= lpddr4_setrefreshrate,
		.refreshperchipselect		= lpddr4_refreshperchipselect,
	};

	return &driver;
}
