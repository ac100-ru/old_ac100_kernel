/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology	5th	Rd.
 * Science-based Industrial	Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2009, Ralink Technology, Inc.
 *
 * All rights reserved.	Ralink's source	code is	an unpublished work	and	the
 * use of a	copyright notice does not imply	otherwise. This	source code
 * contains	confidential trade secret material of Ralink Tech. Any attemp
 * or participation	in deciphering,	decoding, reverse engineering or in	any
 * way altering	the	source code	is stricitly prohibited, unless	the	prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	misc.c

	Abstract:

	Handling Misc Problem

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
	Sean Wang	2009-08-12		Create
	John Li		2009-11-30		Modified
*/

#include "rt_config.h"
#include "misc.h"

DECLARE_TIMER_FUNCTION(DetectExec);
BUILD_TIMER_FUNCTION(DetectExec);

#ifdef DOT11N_DRAFT3
BOOLEAN WifiThroughputOverLimit(
	IN	PRTMP_ADAPTER	pAd,
	IN  UCHAR WifiThroughputLimit)
{
	BOOLEAN  bIssue4020 = FALSE;
	ULONG tmpReceivedByteCount = 0;
	ULONG tmpTransmittedByteCount = 0;
	static ULONG TxByteCount = 0;
	static ULONG RxByteCount = 0;
	static ULONG TxRxThroughputPerSeconds = 0; //Unit: bytes
	LONG diffTX = 0;
	LONG diffRX = 0;

	bIssue4020 = FALSE;
	
	if (pAd == NULL)
	{
		return FALSE;
	}
	
	if (IS_ENABLE_40TO20_BY_TIMER(pAd))
	{	
		tmpReceivedByteCount = pAd->RalinkCounters.ReceivedByteCount; 
		tmpTransmittedByteCount = pAd->RalinkCounters.TransmittedByteCount;		
					
		if ((TxByteCount != 0) || (RxByteCount != 0 ))
		{				
			diffTX = (LONG)(((tmpTransmittedByteCount - TxByteCount)*5) >> 3);
			diffRX = (LONG)(((tmpReceivedByteCount - RxByteCount)* 5) >> 3);		
						
			if ((diffTX > 0) && (diffRX > 0 ))
			{
				TxRxThroughputPerSeconds = diffTX + diffRX ;//Unit: bytes 
			}
			else if ((diffTX < 0) && (diffRX > 0))
			{
				TxRxThroughputPerSeconds = diffRX;
			}
			else if ((diffTX > 0) && (diffRX < 0))
			{
				TxRxThroughputPerSeconds = diffTX;
			}
			else 
			{
				TxRxThroughputPerSeconds = 0;
			}

			DBGPRINT(RT_DEBUG_INFO,("TxRxThroughputPerSeconds = %ld Bps, %ld KBps, %ldKbps, %ldMbps", 
				TxRxThroughputPerSeconds,
				(TxRxThroughputPerSeconds >> 10),
				(TxRxThroughputPerSeconds >> 7),
				(TxRxThroughputPerSeconds >> 17)));
		}
	
		TxByteCount = tmpTransmittedByteCount;
		RxByteCount = tmpReceivedByteCount;

		DBGPRINT(RT_DEBUG_INFO,("btWifiThr = %d, TxByteCount = %ld, RxByteCount = %ld",
			WifiThroughputLimit,
			TxByteCount,
			RxByteCount));

		if ((TxRxThroughputPerSeconds >> 17) > WifiThroughputLimit)
		{
			bIssue4020 = TRUE;
		}
		else
		{
			bIssue4020 = FALSE;
		}
	}
	
	return bIssue4020;
}
#endif // DOT11N_DRAFT3 //

BUSY_DEGREE CheckBusy(
	IN PLONG History, 
	IN UCHAR HistorySize)
{
	if (History == NULL)
	{
		return BUSY_0;
	}

	DBGPRINT(RT_DEBUG_INFO,(" ---> Check Busy %ld %ld %ld %ld %ld",
		*History,
		*(History+1),
		*(History+2),
		*(History+3),
		*(History+4)));

#ifdef RTMP_USB_SUPPORT
	if ((*History > 200000) || 
		(*(History+1) > 200000) || 
		(*(History+2) > 200000) ||
		(*(History+3) > 200000) ||
		(*(History+4) > 200000))
	{
		return BUSY_5; 
	}
	
	if (((*History >= 20000) || 
		(*(History+1) >= 20000) || 
		(*(History+2) >= 20000) || 
		(*(History+3) >= 20000) || 
		(*(History+4) >= 20000))&&
		(*History >= 10000) && 
		(*(History+1) >= 10000) && 
		(*(History+2) >= 10000) && 
		(*(History+3) >= 10000) && 
		(*(History+4) >= 10000))	
	{
		return BUSY_4;	
	}
	
	if (((*History >= 12000) || 
		(*(History+1) >= 12000) || 
		(*(History+2) >= 12000) || 
		(*(History+3) >= 12000) || 
		(*(History+4) >= 12000))&&
		(*History >= 5000) && 
		(*(History+1) >= 5000) && 
		(*(History+2) >= 5000) && 
		(*(History+3) >= 5000) && 
		(*(History+4) >= 5000))	
	{
		return BUSY_3;	
	}
	
	if (((*History >= 12000) || 
		(*(History+1) >= 12000) || 
		(*(History+2) >= 12000) || 
		(*(History+3) >= 12000) || 
		(*(History+4) >= 12000))&&
		(*History >= 2000) && 
		(*(History+1) >= 2000) && 
		(*(History+2) >= 2000) && 
		(*(History+3) >= 2000) && 
		(*(History+4) >= 2000))	
	{
		return BUSY_2;
	}

	if (((*History >= 10000) || 
		(*(History+1) >= 10000) || 
		(*(History+2) >= 10000) || 
		(*(History+3) >= 10000) || 
		(*(History+4) >= 10000)) &&
		((*History > 0) && 
		(*(History+1) > 0) && 
		(*(History+2) > 0) &&
		(*(History+3) > 0) &&
		(*(History+4) > 0)))
	{
		return BUSY_1;
	}	
#endif // RTMP_USB_SUPPORT //

	return BUSY_0;
}

VOID Adjust(
	IN PRTMP_ADAPTER	pAd, 
	IN BOOLEAN			bIssue4020, 
	IN ULONG			NoBusyTimeCount)
{
	CHAR	Rssi;

	 if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS))
      		{
                     /*
                      * Still scanning, ignore this scanning.
                      */
                     DBGPRINT(RT_DEBUG_TRACE, ("IN Adjust StaSiteSurvey:: Scanning now\n"));
                     return;
       		}

	Rssi = RTMPMaxRssi(pAd, 
					   pAd->StaCfg.RssiSample.AvgRssi0, 
					   pAd->StaCfg.RssiSample.AvgRssi1, 
					   pAd->StaCfg.RssiSample.AvgRssi2);
	
	DBGPRINT(RT_DEBUG_TRACE,("RSSI = %d\n", Rssi));

	if (IS_ENABLE_LNA_MID_GAIN_DOWN_BY_TIMER(pAd))
	{
		UCHAR BbpR65 = 0;
		if (pAd->BusyDegree >= BUSY_1)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("Lower LNA Middle Gain at HT20 or HT40\n"));
			pAd->bPermitLnaGainDown = TRUE;

			DBGPRINT(RT_DEBUG_TRACE,("RSSI = %d\n", Rssi));

			if (Rssi <= -35)
			{
			/* if RSSI is smaller than -80, then set R65 to High Gain to fix long distance issue */
				if (Rssi <= -80)
				{
					BBP_IO_READ8_BY_REG_ID(pAd, BBP_R65, &BbpR65);

					if (BbpR65 == 0x29 || BbpR65 == 0x00)
					{
						DBGPRINT(RT_DEBUG_TRACE,("Set R65 to 0x2C from 0x29 (Highest LNA)\n"));
						BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, 0x2C); /* Highest LNA Gain */
					}
				}
				else
				{
					BBP_IO_READ8_BY_REG_ID(pAd, BBP_R65, &BbpR65);

					if (BbpR65 == 0x2C || BbpR65 == 0x00)
					{
						DBGPRINT(RT_DEBUG_TRACE,("Set R65 to 0x29 from 0x2C (Middle LNA)\n"));
						BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, 0x29); /* Middle LNA Gain */
					}
				}
			}
			else
			{
				BBP_IO_READ8_BY_REG_ID(pAd, BBP_R65, &BbpR65);

				if (BbpR65 == 0x29 || BbpR65 == 0x00)
				{
					DBGPRINT(RT_DEBUG_TRACE,("Set R65 to 0x2C from 0x29 (Highest LNA)\n"));
					BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, 0x2C); /* Highest LNA Gain */
				}
			}
		}
		else
		{
			if (NoBusyTimeCount > IDLE_STATE_THRESHOLD)
			{	
			DBGPRINT(RT_DEBUG_TRACE, ("Lower LNA Middle Gain at HT20 or HT40 (Highest LNA)\n"));

			pAd->bPermitLnaGainDown = FALSE;
				BBP_IO_READ8_BY_REG_ID(pAd, BBP_R65, &BbpR65);
				if (BbpR65 == 0x29 || BbpR65 == 0x00)
				{
					DBGPRINT(RT_DEBUG_TRACE,("Set R65 to 0x2C from 0x29 (Highest LNA)\n"));
					BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, 0x2C); /* Highest LNA Gain */
				}
			}
			else
			{
				DBGPRINT(RT_DEBUG_TRACE, ("Lower LNA Middle Gain at HT20 or HT40\n"));
			}
		}
	}

	if (IS_ENABLE_TX_POWER_DOWN_BY_TIMER(pAd))
	{
		if (pAd->BusyDegree >= BUSY_1)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("Lower Tx Power at HT20 or HT40\n"));
				pAd->bPermitTxPowerDown= TRUE;
		}
		else
		{
			if (NoBusyTimeCount > IDLE_STATE_THRESHOLD)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("Higher Tx Power at HT20 or HT40\n"));
					pAd->bPermitTxPowerDown= FALSE;
			}
			else
			{
				DBGPRINT(RT_DEBUG_TRACE, ("Lower Tx Power at HT20 or HT40\n"));
			}
		}
	}

#ifdef DOT11_N_SUPPORT
	if (IS_ENABLE_TXWI_AMPDU_SIZE_BY_TIMER(pAd))
	{
	/* Fixed long distance issue */
		if ((pAd->BusyDegree >= BUSY_2) && 
		((pAd->CommonCfg.BBPCurrentBW == BW_40) || 
		(pAd->CommonCfg.BBPCurrentBW == BW_20)) && 
		(Rssi <= -80))
		{
			pAd->bPermitTxBaSizeDown = TRUE;
		}
		else
		{
			pAd->bPermitTxBaSizeDown = FALSE;
		}
	}

	if (IS_ENABLE_TXWI_AMPDU_DENSITY_BY_TIMER(pAd))
	{
	/* Fixed long distance issue */
		if ((pAd->BusyDegree >= BUSY_2) && 
		((pAd->CommonCfg.BBPCurrentBW == BW_40) || 
		(pAd->CommonCfg.BBPCurrentBW == BW_20)) && 
		(Rssi <= -80))
		{
			pAd->bPermitTxBaDensityDown = TRUE;
		}
		else
		{
			pAd->bPermitTxBaDensityDown = FALSE;
		}
	}
		
	if (IS_ENABLE_RATE_ADAPTIVE_BY_TIMER(pAd))
	{
		if ((pAd->BusyDegree >= BUSY_2) && 
				(pAd->CommonCfg.BBPCurrentBW == BW_40))
		{
			pAd->bPermitMcsDown = TRUE;
		}
		else
		{
			pAd->bPermitMcsDown = FALSE;
		}
	}

	if (IS_ENABLE_REJECT_ORE_BA_BY_TIMER(pAd))
	{	
	/* Fixed long distance issue */
		if ((pAd->BusyDegree >= BUSY_2) && 
		((pAd->CommonCfg.BBPCurrentBW == BW_40) || 
		(pAd->CommonCfg.BBPCurrentBW == BW_20)) && 
		(Rssi <= -80))
		{
			BASessionTearDownALL(pAd, BSSID_WCID);
			pAd->bPermitRecBaDown = TRUE;
		}
		else
		{
			pAd->bPermitRecBaDown = FALSE;
		}
	}
	
#ifdef DOT11N_DRAFT3
	if (IS_ENABLE_40TO20_BY_TIMER(pAd))
	{
	if (((pAd->BusyDegree >= WIFI_2040_SWITCH_THRESHOLD) && 
			(pAd->BusyDegree != BUSY_5)) && 
			(pAd->CommonCfg.BBPCurrentBW == BW_40) && 
			(OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_SCAN_2040)) &&
			(bIssue4020 == TRUE))
		{
			BSS_2040_COEXIST_IE OldValue;

		DBGPRINT(RT_DEBUG_TRACE, ("HT40 --> HT20\n"));
		DBGPRINT(RT_DEBUG_TRACE,("ACT - Update2040CoexistFrameAndNotify. BSSCoexist2040 = %x. EventANo = %d. \n", pAd->CommonCfg.BSSCoexist2040.word, pAd->CommonCfg.TriggerEventTab.EventANo));
			OldValue.word = pAd->CommonCfg.BSSCoexist2040.word;
			pAd->CommonCfg.BSSCoexist2040.word = 0;

			//if (pAd->CommonCfg.TriggerEventTab.EventBCountDown > 0)
			pAd->CommonCfg.BSSCoexist2040.field.BSS20WidthReq = 1;

		/*
			Need to check !!!!
			How STA will set Intolerant40 if implementation dependent. Now we don't set this bit first!!!!!
			So Only check BSS20WidthReq change.
		*/
			//if (OldValue.field.BSS20WidthReq != pAd->CommonCfg.BSSCoexist2040.field.BSS20WidthReq)
			{
				Send2040CoexistAction(pAd, BSSID_WCID, TRUE);
			}
		}
	}
#endif // DOT11N_DRAFT3 //
#endif // DOT11_N_SUPPORT //
}

VOID TxPowerDown(
	IN PRTMP_ADAPTER	pAd, 
	IN CHAR				Rssi,
	INOUT CHAR			*pDeltaPowerByBbpR1, 
	INOUT CHAR			*pDeltaPwr)
{
	if ((pAd->bHWCoexistenceInit == TRUE) && 
		IS_ENABLE_TX_POWER_DOWN_BY_TIMER(pAd) &&
		(pAd->bPermitTxPowerDown== TRUE) && 
		(pAd->CommonCfg.TxPowerPercentage == 0xffffffff))
	{
		CHAR DeltaPowerByBbpR1 = *pDeltaPowerByBbpR1;
		CHAR DeltaPwr = *pDeltaPwr;

		DBGPRINT(RT_DEBUG_INFO, (">> ++ DeltaPwr =%d, DeltaPowerByBbpR1= %d, RSSI = %d\n",
			DeltaPwr, DeltaPowerByBbpR1, Rssi));

		if (Rssi <= -80)
		{	
			DBGPRINT(RT_DEBUG_INFO, (">> -1 75% power dbm\n"));
			DeltaPowerByBbpR1 = 0;
			DeltaPwr -= 1;
		}
		else if ((Rssi <= -50) && (Rssi >= -60))
		{
			DeltaPowerByBbpR1 = -6;
			DeltaPwr -= 1;

			DBGPRINT(RT_DEBUG_INFO, (">> -7 18% power dbm\n"));
		}
		else if ((Rssi <= -60) && (Rssi >= -70))
		{
			DeltaPowerByBbpR1 = -6;
			DBGPRINT(RT_DEBUG_INFO, (">> -6 25% power dbm\n"));
		}
		else if ((Rssi <= -70) && (Rssi >= -80)) 
		{
			DBGPRINT(RT_DEBUG_INFO, (">> -3 50% dbm\n"));
			DeltaPowerByBbpR1 = 0;
			DeltaPwr -= 3;
		}
		else // drop into lowest power
		{
			DeltaPowerByBbpR1 = 0;
			DeltaPowerByBbpR1 -= 12;
		}
		
		*pDeltaPowerByBbpR1 = DeltaPowerByBbpR1;
		*pDeltaPwr = DeltaPwr;
			
		DBGPRINT(RT_DEBUG_INFO, (">> -- DeltaPwr =%d, DeltaPowerByBbpR1= %d, RSSI = %d\n", 
			DeltaPwr, DeltaPowerByBbpR1, Rssi));
	}
}

VOID McsDown(
	IN PRTMP_ADAPTER	pAd, 
	IN CHAR				CurrRateIdx, 
	IN PRTMP_TX_RATE_SWITCH	pCurrTxRate, 
	INOUT CHAR			*pUpRateIdx, 
	INOUT CHAR			*pDownRateIdx)
{
	if ((pAd->bHWCoexistenceInit == TRUE) && 
		IS_ENABLE_RATE_ADAPTIVE_BY_TIMER(pAd) && 
		(pAd->bPermitMcsDown == TRUE))
	{
		UCHAR	btMCSThreshold = 0x00;
		UCHAR	UpRateIdx = *pUpRateIdx;
		UCHAR	DownRateIdx = *pDownRateIdx;

		btMCSThreshold = (UCHAR)(GET_PARAMETER_OF_MCS_THRESHOLD(pAd));
		//0,1,2,3 => MCS= 3, 4, 5, 6
		if (btMCSThreshold <= 0x3)
		{
			btMCSThreshold = btMCSThreshold + 0x3;
		}
		else
		{
			btMCSThreshold = 0x03;
		}
		
		if (CurrRateIdx > 0)
		{
			// Rate is equeal to btMCSThreshold
			if ((pCurrTxRate->CurrMCS == btMCSThreshold)) 
			{
				UpRateIdx = CurrRateIdx;
			}
	
			// Rate be decreased to btMCSThreshold
			else if (pCurrTxRate->CurrMCS > btMCSThreshold)
			{
				UpRateIdx = CurrRateIdx - 1;
			}
			//Rate is under btMCSThreshold
			//else
			//{
			//	UpRateIdx = CurrRateIdx + 1;
			//}
			//DownRateIdx = CurrRateIdx - 1;	
	
			DBGPRINT(RT_DEBUG_INFO,("CurrRateIdx=%d, UpRateIdx=%d, DownRateIdx=%d\n",
				CurrRateIdx, UpRateIdx, DownRateIdx));
		}

		*pUpRateIdx = UpRateIdx;
		*pDownRateIdx = DownRateIdx;
	}
}

VOID McsDown2(
	IN PRTMP_ADAPTER	pAd, 
	IN UCHAR			MCS3, 
	IN UCHAR			MCS4, 
	IN UCHAR			MCS5, 
	IN UCHAR			MCS6, 
	INOUT UCHAR			*pTxRateIdx)
{
	if ((pAd->bHWCoexistenceInit == TRUE) && 
		IS_ENABLE_RATE_ADAPTIVE_BY_TIMER(pAd) && 
		(pAd->bPermitMcsDown == TRUE))
	{
		UCHAR	btMCSThreshold = 0x00;
		UCHAR	TxRateIdx = *pTxRateIdx;

		btMCSThreshold = (UCHAR)(GET_PARAMETER_OF_MCS_THRESHOLD(pAd));
		//0,1,2,3 => MCS= 3, 4, 5, 6
		if (btMCSThreshold <= 0x3)
		{
			btMCSThreshold = btMCSThreshold + 0x3;
		}
		else
		{
			btMCSThreshold = 0x03;
		}
	
		if (btMCSThreshold == 0x03)
		{
			btMCSThreshold = MCS3;
		}
		else if (btMCSThreshold == 0x04)
		{
			btMCSThreshold = MCS4;
		}
		else if (btMCSThreshold == 0x05)
		{
			btMCSThreshold = MCS5;
		}
		else if (btMCSThreshold == 0x06)
		{
			btMCSThreshold = MCS6;
		}
		else
		{
			btMCSThreshold = MCS3;
		}
	
		if (TxRateIdx > btMCSThreshold)
		{
			TxRateIdx = btMCSThreshold; 
		}

		*pTxRateIdx = TxRateIdx;
	}
}

VOID TxBaSizeDown(
	IN PRTMP_ADAPTER	pAd, 
	INOUT PTXWI_STRUC 	pTxWI)
{
	if ((pAd->bHWCoexistenceInit == TRUE) && 
		INFRA_ON(pAd) && 
		(pAd->OpMode == OPMODE_STA) && 
		(pTxWI->BAWinSize != 0) && 
		IS_ENABLE_TXWI_AMPDU_SIZE_BY_TIMER(pAd) && 
		(pAd->bPermitTxBaSizeDown == TRUE))
	{
		UCHAR				btAMPDUSize = 0;

		/* When Bluetooh is busy, we set the BA Size to smaller */
		btAMPDUSize = (UCHAR)(GET_PARAMETER_OF_AMPDU_SIZE(pAd));
		if (btAMPDUSize <= 3)
		{
			//0,1,2,3 => ba size=1, 3, 5, 7
			if (btAMPDUSize == 0)
			{
				pTxWI->BAWinSize = 1;
			}
			else
			{
				pTxWI->BAWinSize = 1 + (btAMPDUSize << 1);
			}
		}
		else 
		{
			pTxWI->BAWinSize = 1;
		}
		
		DBGPRINT(RT_DEBUG_INFO, (">>>>> dynamic BAWinSize = %d, profile AMPDU size=%ld\n", 
		pTxWI->BAWinSize, GET_PARAMETER_OF_AMPDU_SIZE(pAd)));	
	}
}

VOID TxBaDensityDown(
	IN PRTMP_ADAPTER	pAd, 
	INOUT PTXWI_STRUC 	pTxWI)
{
	if ((pAd->bHWCoexistenceInit == TRUE) && 
		INFRA_ON(pAd) && 
		(pAd->OpMode == OPMODE_STA) && 
		IS_ENABLE_TXWI_AMPDU_DENSITY_BY_TIMER(pAd) && 
		(pAd->bPermitTxBaDensityDown == TRUE))
	{
		UCHAR				btAMPDUDensity = 0;

		/* When Bluetooh is busy, we set the BA density to larger */
		btAMPDUDensity = (UCHAR)(GET_PARAMETER_OF_AMPDU_DENSITY(pAd));
		if (btAMPDUDensity <= 3)
		{
			//0,1,2,3 => BA density=0x01, 0x03, 0x05, 0x07 => 0.25u, 1u, 4u, 16u
			if (btAMPDUDensity == 0)
			{
				pTxWI->MpduDensity = 0x01;
			}
			pTxWI->MpduDensity = 1 + (btAMPDUDensity << 1);
		}
		else 
		{
			pTxWI->MpduDensity = 0x07;
		}
		
		DBGPRINT(RT_DEBUG_INFO, (">>>>> dynamic BA density= %d, profile AMPDU density=%ld\n", 
			pTxWI->MpduDensity, GET_PARAMETER_OF_AMPDU_DENSITY(pAd)));
	}
}

VOID MiscInit(
	IN PRTMP_ADAPTER pAd)
{
#ifdef RTMP_USB_SUPPORT
	if ((pAd->NicConfig3.field.CoexMethod == TRUE) && (IS_ENABLE_THREE_WIRE_BY_FORCE(pAd)))
	{
		pAd->bCoexMethod = TRUE;
		pAd->StaCfg.bHardwareRadio = FALSE;
	}
	else
	{
		pAd->bCoexMethod = FALSE;
	}
#endif // RTMP_USB_SUPPORT //
//	if (IS_ENABLE_MISC_TIMER(pAd))
//	{
//		RTMPInitTimer(pAd, &pAd->Mlme.MiscDetectTimer, GET_TIMER_FUNCTION(DetectExec), pAd, TRUE);
//		RTMPSetTimer(&pAd->Mlme.MiscDetectTimer, DETECT_TIMEOUT);
//	}

	DBGPRINT(RT_DEBUG_TRACE,("nicConfig2 =0x%04x\n", pAd->NicConfig2.word));
	DBGPRINT(RT_DEBUG_TRACE,("nicConfig3 =0x%04x\n", pAd->NicConfig3.word));
}

VOID MiscUserCfgInit(
	IN PRTMP_ADAPTER pAd)
{
	pAd->bMiscOn = TRUE;
#ifdef RTMP_USB_SUPPORT
	pAd->bCoexMethod = FALSE;
#endif // RTMP_USB_SUPPORT //
#ifdef DOT11_N_SUPPORT
	pAd->bPermitRecBaDown = FALSE;
	pAd->bPermitMcsDown = FALSE;
	pAd->bPermitTxBaSizeDown = FALSE;
	pAd->bPermitTxBaDensityDown = FALSE;
#endif // DOT11_N_SUPPORT //
	pAd->bPermitTxPowerDown = FALSE;
	pAd->bPermitLnaGainDown = FALSE;
#ifdef RTMP_USB_SUPPORT
	pAd->ulConfiguration = 0x380004FD;
#endif // RTMP_USB_SUPPORT //
	pAd->ulActiveCountPastPeriod = 0;
	pAd->BusyDegree = BUSY_0;
}

#ifdef RTMP_USB_SUPPORT
NTSTATUS DetectExecAtCmdThread(
	IN PRTMP_ADAPTER pAd)
{
	BOOLEAN  					bIssue4020 = FALSE;
	UCHAR	 					j = 0;
#ifdef DOT11N_DRAFT3
	UCHAR 						btWifiThr = 0;
#endif // DOT11N_DRAFT3 //
	ULONG 						data = 0;
	ULONG 						data_switch=0;	
	static ULONG 				TimeCount = 0; // per 0.01 second, count it.
	static ULONG 				NoBusyTimeCount = 0; // per second, count it.
	static ULONG 				History[HISTORY_RECORD_NUM] = {0};
	static ULONG 				BtHardKeyOffTimeCount = 0;
	BUSY_DEGREE  		BusyDegree = BUSY_0;

	TimeCount++;
		
	if (BtHardKeyOffTimeCount == 0)
	{
		if(pAd->bCoexMethod == TRUE)
		{
			AsicSendCommandToMcu(pAd, MCU_SET_ACTIVE_POLLING, 0xff , 0x01, 0x01);
		}
		else
		{
			AsicSendCommandToMcu(pAd, MCU_SET_ACTIVE_POLLING, 0xff , 0x01, 0x00); 
		}
	}
	else
	{
		AsicSendCommandToMcu(pAd, MCU_SET_ACTIVE_POLLING, 0xff , 0x01, 0x02); 	
	}

	RTMP_IO_READ32(pAd, ACT_CNT, &data);
	data_switch &= 0;
	data_switch |= ((data & 0x000000FF) << 8);
	data_switch |= ((data & 0x0000FF00) >> 8);
	pAd->ulActiveCountPastPeriod += data_switch;

	DBGPRINT(RT_DEBUG_INFO, (">>>>> data = 0x%08lx, data_switch=0x%08lx", (data & 0x0000FFFF), data_switch));

	/* About 1 second to check busy from Buletooth history active counter */
	if (TimeCount >= CHECK_TIME_INTERVAL)
	{
		DBGPRINT(RT_DEBUG_INFO,("<--- WATCH TIME"));	
		DBGPRINT(RT_DEBUG_INFO,("BW=%d, bt active per sec=%ld, No Busy Time Count =%ld", pAd->CommonCfg.BBPCurrentBW,
				pAd->ulActiveCountPastPeriod, NoBusyTimeCount));

		if (pAd->ulActiveCountPastPeriod >= 250000)
		{	
			if (BtHardKeyOffTimeCount == 0)
			{
				DBGPRINT(RT_DEBUG_INFO,("Hardware key OFF!!!!"));

				AsicSendCommandToMcu(pAd, MCU_SET_ACTIVE_POLLING, 0xff , 0x01, 0x02);	
			}
			BtHardKeyOffTimeCount++;
			pAd->ulActiveCountPastPeriod = 0;
			DBGPRINT(RT_DEBUG_INFO,("ulActiveCountPastPeriod >= 250000, force ulActiveCountPastPeriod = 0"));
		}
		else
		{
			if (BtHardKeyOffTimeCount > 0)
			{
				BtHardKeyOffTimeCount = 0;
				DBGPRINT(RT_DEBUG_INFO,("Hardware key ON!!!!"));

				if(pAd->bCoexMethod == TRUE)
				{
					//8070=>3 wire
					AsicSendCommandToMcu(pAd, MCU_SET_ACTIVE_POLLING, 0xff , 0x01, 0x01);
				}
				else
				{
					//3070,3870 =>2wire
					AsicSendCommandToMcu(pAd, MCU_SET_ACTIVE_POLLING, 0xff , 0x01, 0x00); 
				}
			}	
		}

		for(j=HISTORY_RECORD_NUM-1 ; j>=1 ; j--)
		{
			History[j]=History[j-1];
		}

		History[0]=pAd->ulActiveCountPastPeriod;
		BusyDegree = CheckBusy(&History[0],HISTORY_RECORD_NUM);
		pAd->BusyDegree = BusyDegree;

		if (pAd->BusyDegree == BUSY_0)
		{	
			NoBusyTimeCount++;
		}
		else
		{
			NoBusyTimeCount = 0;
		}
		
#ifdef DOT11N_DRAFT3
		btWifiThr = (UCHAR)(GET_PARAMETER_OF_TXRX_THR_THRESHOLD(pAd));
		/* 0,1,2,3 => 0, 6, 12, 18 Mbps */
		if ((btWifiThr <= 3)&& IS_ENABLE_40TO20_BY_TIMER(pAd))
		{
			bIssue4020 = WifiThroughputOverLimit(pAd,(btWifiThr*6));
		}
		else 
#endif // DOT11N_DRAFT3 //
		bIssue4020 = FALSE;
		
		pAd->ulActiveCountPastPeriod=0;
		TimeCount = 0;
	}

	if (TimeCount == 0)
	{
		Adjust(pAd, bIssue4020, NoBusyTimeCount);
	}	
	
	
	return NDIS_STATUS_SUCCESS;
}

VOID DetectExec(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3)
{
	PRTMP_ADAPTER	pAd = (RTMP_ADAPTER *)FunctionContext;
		
	if ((RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RESET_IN_PROGRESS))||
		(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS))||
		(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF))||
		(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
	{
				DBGPRINT(RT_DEBUG_INFO,("error RESET, HALT, RADIO_OFF, NIC_NOT_EXIST"));
				return;
	}

	if ((pAd->bHWCoexistenceInit == TRUE) && 
		(!IDLE_ON(pAd))&& 
		(pAd->OpMode == OPMODE_STA))
	{
		RTEnqueueInternalCmd(pAd,RT_CMD_COEXISTENCE_DETECTION, NULL, 0);
	}
	else
	{
#ifdef DOT11_N_SUPPORT
		pAd->bPermitRecBaDown = FALSE;
		pAd->bPermitMcsDown = FALSE;
		pAd->bPermitTxBaSizeDown = FALSE;
		pAd->bPermitTxBaDensityDown = FALSE;
#endif // DOT11_N_SUPPORT //
		pAd->bPermitTxPowerDown = FALSE;
		pAd->bPermitLnaGainDown = FALSE;
	}	
}
#endif // RTMP_USB_SUPPORT //



