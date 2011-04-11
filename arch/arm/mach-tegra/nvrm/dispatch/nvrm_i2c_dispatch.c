/*
 * Copyright (c) 2009 NVIDIA Corporation.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of the NVIDIA Corporation nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
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
 *
 */

#define NV_IDL_IS_DISPATCH

#include "nvcommon.h"
#include "nvos.h"
#include "nvassert.h"
#include "nvreftrack.h"
#include "nvidlcmd.h"
#include "nvrm_i2c.h"

#define OFFSET( s, e ) (NvU32)(void *)(&(((s*)0)->e))


typedef struct NvRmI2cTransaction_in_t
{
    NvU32 package_;
    NvU32 function_;
    NvRmI2cHandle hI2c;
    NvU32 I2cPinMap;
    NvU32 WaitTimeoutInMilliSeconds;
    NvU32 ClockSpeedKHz;
    NvU8  * Data;
    NvU32 DataLen;
    NvRmI2cTransactionInfo  * Transaction;
    NvU32 NumOfTransactions;
} NV_ALIGN(4) NvRmI2cTransaction_in;

typedef struct NvRmI2cTransaction_inout_t
{
    NvU32 dummy_;
} NV_ALIGN(4) NvRmI2cTransaction_inout;

typedef struct NvRmI2cTransaction_out_t
{
    NvError ret_;
} NV_ALIGN(4) NvRmI2cTransaction_out;

typedef struct NvRmI2cTransaction_params_t
{
    NvRmI2cTransaction_in in;
    NvRmI2cTransaction_inout inout;
    NvRmI2cTransaction_out out;
} NvRmI2cTransaction_params;

typedef struct NvRmI2cClose_in_t
{
    NvU32 package_;
    NvU32 function_;
    NvRmI2cHandle hI2c;
} NV_ALIGN(4) NvRmI2cClose_in;

typedef struct NvRmI2cClose_inout_t
{
    NvU32 dummy_;
} NV_ALIGN(4) NvRmI2cClose_inout;

typedef struct NvRmI2cClose_out_t
{
    NvU32 dummy_;
} NV_ALIGN(4) NvRmI2cClose_out;

typedef struct NvRmI2cClose_params_t
{
    NvRmI2cClose_in in;
    NvRmI2cClose_inout inout;
    NvRmI2cClose_out out;
} NvRmI2cClose_params;

typedef struct NvRmI2cOpen_in_t
{
    NvU32 package_;
    NvU32 function_;
    NvRmDeviceHandle hDevice;
    NvU32 IoModule;
    NvU32 instance;
} NV_ALIGN(4) NvRmI2cOpen_in;

typedef struct NvRmI2cOpen_inout_t
{
    NvU32 dummy_;
} NV_ALIGN(4) NvRmI2cOpen_inout;

typedef struct NvRmI2cOpen_out_t
{
    NvError ret_;
    NvRmI2cHandle phI2c;
} NV_ALIGN(4) NvRmI2cOpen_out;

typedef struct NvRmI2cOpen_params_t
{
    NvRmI2cOpen_in in;
    NvRmI2cOpen_inout inout;
    NvRmI2cOpen_out out;
} NvRmI2cOpen_params;

static u16 wm8903_regs[] = {
	0x8903,     /* R0   - SW Reset and ID */
	0x0000,     /* R1   - Revision Number */
	0x0000,     /* R2 */
	0x0000,     /* R3 */
	0x0018,     /* R4   - Bias Control 0 */
	0x0000,     /* R5   - VMID Control 0 */
	0x0000,     /* R6   - Mic Bias Control 0 */
	0x0000,     /* R7 */
	0x0001,     /* R8   - Analogue DAC 0 */
	0x0000,     /* R9 */
	0x0001,     /* R10  - Analogue ADC 0 */
	0x0000,     /* R11 */
	0x0000,     /* R12  - Power Management 0 */
	0x0000,     /* R13  - Power Management 1 */
	0x0000,     /* R14  - Power Management 2 */
	0x0000,     /* R15  - Power Management 3 */
	0x0000,     /* R16  - Power Management 4 */
	0x0000,     /* R17  - Power Management 5 */
	0x0000,     /* R18  - Power Management 6 */
	0x0000,     /* R19 */
	0x0400,     /* R20  - Clock Rates 0 */
	0x0D07,     /* R21  - Clock Rates 1 */
	0x0000,     /* R22  - Clock Rates 2 */
	0x0000,     /* R23 */
	0x0050,     /* R24  - Audio Interface 0 */
	0x0242,     /* R25  - Audio Interface 1 */
	0x0008,     /* R26  - Audio Interface 2 */
	0x0022,     /* R27  - Audio Interface 3 */
	0x0000,     /* R28 */
	0x0000,     /* R29 */
	0x00C0,     /* R30  - DAC Digital Volume Left */
	0x00C0,     /* R31  - DAC Digital Volume Right */
	0x0000,     /* R32  - DAC Digital 0 */
	0x0000,     /* R33  - DAC Digital 1 */
	0x0000,     /* R34 */
	0x0000,     /* R35 */
	0x00C0,     /* R36  - ADC Digital Volume Left */
	0x00C0,     /* R37  - ADC Digital Volume Right */
	0x0000,     /* R38  - ADC Digital 0 */
	0x0073,     /* R39  - Digital Microphone 0 */
	0x09BF,     /* R40  - DRC 0 */
	0x3241,     /* R41  - DRC 1 */
	0x0020,     /* R42  - DRC 2 */
	0x0000,     /* R43  - DRC 3 */
	0x0085,     /* R44  - Analogue Left Input 0 */
	0x0085,     /* R45  - Analogue Right Input 0 */
	0x0044,     /* R46  - Analogue Left Input 1 */
	0x0044,     /* R47  - Analogue Right Input 1 */
	0x0000,     /* R48 */
	0x0000,     /* R49 */
	0x0008,     /* R50  - Analogue Left Mix 0 */
	0x0004,     /* R51  - Analogue Right Mix 0 */
	0x0000,     /* R52  - Analogue Spk Mix Left 0 */
	0x0000,     /* R53  - Analogue Spk Mix Left 1 */
	0x0000,     /* R54  - Analogue Spk Mix Right 0 */
	0x0000,     /* R55  - Analogue Spk Mix Right 1 */
	0x0000,     /* R56 */
	0x002D,     /* R57  - Analogue OUT1 Left */
	0x002D,     /* R58  - Analogue OUT1 Right */
	0x0039,     /* R59  - Analogue OUT2 Left */
	0x0039,     /* R60  - Analogue OUT2 Right */
	0x0100,     /* R61 */
	0x0139,     /* R62  - Analogue OUT3 Left */
	0x0139,     /* R63  - Analogue OUT3 Right */
	0x0000,     /* R64 */
	0x0000,     /* R65  - Analogue SPK Output Control 0 */
	0x0000,     /* R66 */
	0x0010,     /* R67  - DC Servo 0 */
	0x0100,     /* R68 */
	0x00A4,     /* R69  - DC Servo 2 */
	0x0807,     /* R70 */
	0x0000,     /* R71 */
	0x0000,     /* R72 */
	0x0000,     /* R73 */
	0x0000,     /* R74 */
	0x0000,     /* R75 */
	0x0000,     /* R76 */
	0x0000,     /* R77 */
	0x0000,     /* R78 */
	0x000E,     /* R79 */
	0x0000,     /* R80 */
	0x0000,     /* R81 */
	0x0000,     /* R82 */
	0x0000,     /* R83 */
	0x0000,     /* R84 */
	0x0000,     /* R85 */
	0x0000,     /* R86 */
	0x0006,     /* R87 */
	0x0000,     /* R88 */
	0x0000,     /* R89 */
	0x0000,     /* R90  - Analogue HP 0 */
	0x0060,     /* R91 */
	0x0000,     /* R92 */
	0x0000,     /* R93 */
	0x0000,     /* R94  - Analogue Lineout 0 */
	0x0060,     /* R95 */
	0x0000,     /* R96 */
	0x0000,     /* R97 */
	0x0000,     /* R98  - Charge Pump 0 */
	0x1F25,     /* R99 */
	0x2B19,     /* R100 */
	0x01C0,     /* R101 */
	0x01EF,     /* R102 */
	0x2B00,     /* R103 */
	0x0000,     /* R104 - Class W 0 */
	0x01C0,     /* R105 */
	0x1C10,     /* R106 */
	0x0000,     /* R107 */
	0x0000,     /* R108 - Write Sequencer 0 */
	0x0000,     /* R109 - Write Sequencer 1 */
	0x0000,     /* R110 - Write Sequencer 2 */
	0x0000,     /* R111 - Write Sequencer 3 */
	0x0000,     /* R112 - Write Sequencer 4 */
	0x0000,     /* R113 */
	0x0000,     /* R114 - Control Interface */
	0x0000,     /* R115 */
	0x00A8,     /* R116 - GPIO Control 1 */
	0x00A8,     /* R117 - GPIO Control 2 */
	0x00A8,     /* R118 - GPIO Control 3 */
	0x0220,     /* R119 - GPIO Control 4 */
	0x01A0,     /* R120 - GPIO Control 5 */
	0x0000,     /* R121 - Interrupt Status 1 */
	0xFFFF,     /* R122 - Interrupt Status 1 Mask */
	0x0000,     /* R123 - Interrupt Polarity 1 */
	0x0000,     /* R124 */
	0x0003,     /* R125 */
	0x0000,     /* R126 - Interrupt Control */
	0x0000,     /* R127 */
	0x0005,     /* R128 */
	0x0000,     /* R129 - Control Interface Test 1 */
	0x0000,     /* R130 */
	0x0000,     /* R131 */
	0x0000,     /* R132 */
	0x0000,     /* R133 */
	0x0000,     /* R134 */
	0x03FF,     /* R135 */
	0x0007,     /* R136 */
	0x0040,     /* R137 */
	0x0000,     /* R138 */
	0x0000,     /* R139 */
	0x0000,     /* R140 */
	0x0000,     /* R141 */
	0x0000,     /* R142 */
	0x0000,     /* R143 */
	0x0000,     /* R144 */
	0x0000,     /* R145 */
	0x0000,     /* R146 */
	0x0000,     /* R147 */
	0x4000,     /* R148 */
	0x6810,     /* R149 - Charge Pump Test 1 */
	0x0004,     /* R150 */
	0x0000,     /* R151 */
	0x0000,     /* R152 */
	0x0000,     /* R153 */
	0x0000,     /* R154 */
	0x0000,     /* R155 */
	0x0000,     /* R156 */
	0x0000,     /* R157 */
	0x0000,     /* R158 */
	0x0000,     /* R159 */
	0x0000,     /* R160 */
	0x0000,     /* R161 */
	0x0000,     /* R162 */
	0x0000,     /* R163 */
	0x0028,     /* R164 - Clock Rate Test 4 */
	0x0004,     /* R165 */
	0x0000,     /* R166 */
	0x0060,     /* R167 */
	0x0000,     /* R168 */
	0x0000,     /* R169 */
	0x0000,     /* R170 */
	0x0000,     /* R171 */
	0x0000,     /* R172 - Analogue Output Bias 0 */
};

static NvError NvRmI2cTransaction_dispatch_( void *InBuffer, NvU32 InSize, void *OutBuffer, NvU32 OutSize, NvDispatchCtx* Ctx )
{
    NvError err_ = NvSuccess;
    NvRmI2cTransaction_in *p_in;
    NvRmI2cTransaction_out *p_out;
    NvU8  *Data = NULL;
    NvRmI2cTransactionInfo *Transaction = NULL;

    p_in = (NvRmI2cTransaction_in *)InBuffer;
    p_out = (NvRmI2cTransaction_out *)((NvU8 *)OutBuffer + OFFSET(NvRmI2cTransaction_params, out) - OFFSET(NvRmI2cTransaction_params, inout));

    if( p_in->DataLen && p_in->Data )
    {
        Data = (NvU8  *)NvOsAlloc( p_in->DataLen * sizeof( NvU8  ) );
        if( !Data )
        {
            err_ = NvError_InsufficientMemory;
            goto clean;
        }
        if( p_in->Data )
        {
            err_ = NvOsCopyIn( Data, p_in->Data, p_in->DataLen * sizeof( NvU8  ) );
            if( err_ != NvSuccess )
            {
                err_ = NvError_BadParameter;
                goto clean;
            }
        }
    }
    if( p_in->NumOfTransactions && p_in->Transaction )
    {
        Transaction = (NvRmI2cTransactionInfo  *)NvOsAlloc( p_in->NumOfTransactions * sizeof( NvRmI2cTransactionInfo  ) );
        if( !Transaction )
        {
            err_ = NvError_InsufficientMemory;
            goto clean;
        }
        if( p_in->Transaction )
        {
            err_ = NvOsCopyIn( Transaction, p_in->Transaction, p_in->NumOfTransactions * sizeof( NvRmI2cTransactionInfo  ) );
            if( err_ != NvSuccess )
            {
                err_ = NvError_BadParameter;
                goto clean;
            }
        }
    }

    p_out->ret_ = NvRmI2cTransaction( p_in->hI2c, p_in->I2cPinMap, p_in->WaitTimeoutInMilliSeconds, p_in->ClockSpeedKHz, Data, p_in->DataLen, Transaction, p_in->NumOfTransactions );
    {
	    int i;
	    NvU8  *pData = Data;

	    uint8_t reg;
	    for (i = 0; i < p_in->NumOfTransactions; i++)
	    {
		    //WM8903 faker
		    if(Transaction[i].Address==0x34) {
			    if (Transaction[i].Flags & NVRM_I2C_WRITE) {
				    reg=pData[0];
				    if(Transaction[i].NumBytes==3) {
					    if(reg==0) {
						    //SW RESET and ID
					    } else if(reg==1) {
						    //Revision number
					    } else if(reg==0x79) {
						    //Interrupt status 1
					    } else if(reg==0x70) {
						    //Write sequencer 4
					    } else {
						    wm8903_regs[reg]=(pData[1]<<8)+pData[0];
						    reg=-1;
					    }
				    }
			    } else {
				    pData[0]=wm8903_regs[reg]>>8;
				    pData[1]=wm8903_regs[reg]&0xff;
			    }
			    p_out->ret_=NvSuccess;
		    }
	    }
    }

    if(p_in->Data && Data)
    {
        err_ = NvOsCopyOut( p_in->Data, Data, p_in->DataLen * sizeof( NvU8  ) );
        if( err_ != NvSuccess )
        {
            err_ = NvError_BadParameter;
        }
    }
clean:
    NvOsFree( Data );
    NvOsFree( Transaction );
    return err_;
}

static NvError NvRmI2cClose_dispatch_( void *InBuffer, NvU32 InSize, void *OutBuffer, NvU32 OutSize, NvDispatchCtx* Ctx )
{
    NvError err_ = NvSuccess;
    NvRmI2cClose_in *p_in;

    p_in = (NvRmI2cClose_in *)InBuffer;

    NvRtFreeObjRef(Ctx, NvRtObjType_NvRm_NvRmI2cHandle, p_in->hI2c);

    NvRmI2cClose( p_in->hI2c );

    return err_;
}

static NvError NvRmI2cOpen_dispatch_( void *InBuffer, NvU32 InSize, void *OutBuffer, NvU32 OutSize, NvDispatchCtx* Ctx )
{
    NvError err_ = NvSuccess;
    NvRmI2cOpen_in *p_in;
    NvRmI2cOpen_out *p_out;
    NvRtObjRefHandle ref_phI2c = 0;

    p_in = (NvRmI2cOpen_in *)InBuffer;
    p_out = (NvRmI2cOpen_out *)((NvU8 *)OutBuffer + OFFSET(NvRmI2cOpen_params, out) - OFFSET(NvRmI2cOpen_params, inout));

    p_out->ret_ = NvRmI2cOpen( p_in->hDevice, p_in->IoModule, p_in->instance, &p_out->phI2c );
    if (p_out->ret_ != NvSuccess)
    {
        err_ = p_out->ret_;
        goto clean;
    }
    err_ = NvRtAllocObjRef(Ctx, &ref_phI2c);
    if (err_ == NvSuccess)
    {
        NvRtStoreObjRef(Ctx, ref_phI2c, NvRtObjType_NvRm_NvRmI2cHandle, p_out->phI2c);
    }
    else
    {
        NvRmI2cClose(p_out->phI2c);
    }

clean:
    return err_;
}

NvError nvrm_i2c_Dispatch( NvU32 function, void *InBuffer, NvU32 InSize, void *OutBuffer, NvU32 OutSize, NvDispatchCtx* Ctx );
NvError nvrm_i2c_Dispatch( NvU32 function, void *InBuffer, NvU32 InSize, void *OutBuffer, NvU32 OutSize, NvDispatchCtx* Ctx )
{
    NvError err_ = NvSuccess;

    switch( function ) {
    case 2:
        err_ = NvRmI2cTransaction_dispatch_( InBuffer, InSize, OutBuffer, OutSize, Ctx );
        break;
    case 1:
        err_ = NvRmI2cClose_dispatch_( InBuffer, InSize, OutBuffer, OutSize, Ctx );
        break;
    case 0:
        err_ = NvRmI2cOpen_dispatch_( InBuffer, InSize, OutBuffer, OutSize, Ctx );
        break;
    default:
        err_ = NvError_BadParameter;
        break;
    }

    return err_;
}
