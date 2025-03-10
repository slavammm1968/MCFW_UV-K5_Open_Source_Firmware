/* Copyright 2023 Dual Tachyon
 * https://github.com/DualTachyon
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */

#include <string.h>
#include "misc.h"

const uint32_t *gUpperLimitFrequencyBandTable;
const uint32_t *gLowerLimitFrequencyBandTable;

bool gSetting_350TX;
bool gSetting_200TX;
bool gSetting_500TX;
uint8_t gSetting_ALL_TX;
uint8_t gSetting_F_LOCK;
bool gSetting_ScrambleEnable;

const uint32_t gDefaultAesKey[4] = {
	0x4AA5CC60,
	0x0312CC5F,
	0xFFD2DABB,
	0x6BBA7F92,
};

uint32_t gCustomAesKey[4];
bool bHasCustomAesKey;
uint32_t gChallenge[4];
uint8_t gTryCount;

uint16_t gEEPROM_RSSI_CALIB[7][4];

uint16_t gEEPROM_1F8A;
uint16_t gEEPROM_1F8C;

uint8_t gMR_ChannelAttributes[FREQ_CHANNEL_LAST + 1];

volatile bool gNextTimeslice500ms;
volatile uint16_t gBatterySaveCountdown = 1000;
volatile uint16_t gDualWatchCountdown;
volatile uint16_t gTxTimerCountdown;
volatile uint16_t gTailNoteEliminationCountdown;
bool gEnableSpeaker;
uint8_t gKeyLockCountdown;
uint8_t gRTTECountdown;
bool bIsInLockScreen;
uint8_t gUpdateStatus;
uint8_t gFoundCTCSS;
uint8_t gFoundCDCSS;
bool gEndOfRxDetectedMaybe;
uint8_t gVFO_RSSI_Level[2];
uint8_t gReducedService;
uint8_t gBatteryVoltageIndex;
CssScanMode_t gCssScanMode;
bool gUpdateRSSI;
#if defined(ENABLE_TX1750)
AlarmState_t gAlarmState;
#endif
uint8_t gVoltageMenuCountdown;
bool gPttWasReleased;
bool gPttWasPressed;
uint8_t gKeypadLocked;
bool gFlagReconfigureVfos;
uint8_t gVfoConfigureMode;
bool gFlagResetVfos;
bool gRequestSaveVFO;
uint8_t gRequestSaveChannel;
bool gRequestSaveSettings;
#if defined(ENABLE_FMRADIO)
bool gRequestSaveFM;
#endif
bool gFlagPrepareTX;
bool gFlagAcceptSetting;
bool gFlagRefreshSetting;
bool gFlagSaveVfo;
bool gFlagSaveSettings;
bool gFlagSaveChannel;
#if defined(ENABLE_FMRADIO)
bool gFlagSaveFM;
#endif
uint8_t gDTMF_RequestPending;
bool g_CDCSS_Lost;
uint8_t gCDCSSCodeType;
bool g_CTCSS_Lost;
bool g_CxCSS_TAIL_Found;
bool g_VOX_Lost;
bool g_SquelchLost;
uint8_t gFlashLightState;
bool gVOX_NoiseDetected;
uint16_t gVoxResumeCountdown;
uint16_t gVoxPauseCountdown;
volatile uint16_t gFlashLightBlinkCounter;
bool gFlagEndTransmission;
uint16_t gLowBatteryCountdown;
uint8_t gNextMrChannel;
ReceptionMode_t gRxReceptionMode;
uint8_t gRestoreMrChannel;
uint8_t gCurrentScanList;
uint8_t gPreviousMrChannel;
uint32_t gRestoreFrequency;
uint8_t gRxVfoIsActive;
bool gKeyBeingHeld;
bool gPttIsPressed;
uint8_t gPttDebounceCounter;
uint8_t gMenuListCount;
uint8_t gBackupCROSS_BAND_RX_TX;
uint8_t gScanDelay;
#if defined(ENABLE_AIRCOPY)
uint8_t gAircopySendCountdown;
#endif
uint8_t gFSKWriteIndex;
uint8_t gNeverUsed;

volatile bool gNextTimeslice;
bool gUpdateDisplay;
bool gF_LOCK;
uint8_t gShowChPrefix;
volatile uint16_t gSystickCountdown2;
volatile uint8_t gFoundCDCSSCountdown;
volatile uint8_t gFoundCTCSSCountdown;
volatile uint16_t gVoxStopCountdown;
volatile bool gTxTimeoutReached;
volatile bool gNextTimeslice40ms;
volatile bool gSchedulePowerSave;
volatile bool gBatterySaveCountdownExpired;
volatile bool gScheduleDualWatch = true;
uint8_t gAbrTxRx;
volatile bool gFlagTteComplete;
#if defined(ENABLE_FMRADIO)
volatile bool gScheduleFM;
#endif

#if defined(ENABLE_MISSED_CALL_NOTIFICATION_AND_BLINKING_LED)
uint8_t           gMissedCalls;
volatile uint8_t  gFlashLightNotifCounter;
#endif

uint16_t gCurrentRSSI;

uint8_t gIsLocked = 0xFF;

#ifdef ENABLE_DOCK
	bool		  gSetting_Remote_UI = true;
#endif

// --------

void NUMBER_Get(char *pDigits, uint32_t *pInteger)
{
	uint32_t Value;
	uint32_t Multiplier;
	uint8_t i;

	Multiplier = 10000000;
	Value = 0;
	for (i = 0; i < 8; i++) {
		if (pDigits[i] > 9) {
			break;
		}
		Value += pDigits[i] * Multiplier;
		Multiplier /= 10U;
	}
	*pInteger = Value;
}

void NUMBER_ToDigits(uint32_t Value, char *pDigits)
{
	uint8_t i;

	for (i = 0; i < 9; i++) {
		uint32_t Result = Value / 10U;

		pDigits[8 - i] = Value - (Result * 10U);
		Value = Result;
	}
}

uint8_t NUMBER_AddWithWraparound(uint8_t Base, int8_t Add, uint8_t LowerLimit, uint8_t UpperLimit)
{
	Base += Add;
	if (Base == 0xFF || Base < LowerLimit) {
		return UpperLimit;
	}

	if (Base > UpperLimit) {
		return LowerLimit;
	}

	return Base;
}

