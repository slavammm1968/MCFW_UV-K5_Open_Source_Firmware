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

#include "welcome.h"
#include "../driver/eeprom.h"
#include "../driver/st7565.h"
#include "../external/printf/printf.h"
#include "../helper/battery.h"
#include "../settings.h"
#include "helper.h"
#include "../version.h"
#include <string.h>
#ifdef ENABLE_DOCK
	#include "app/uart.h"
#endif

void UI_DisplayWelcome(void) {
  memset(gStatusLine, 0, sizeof(gStatusLine));
  UI_DisplayClear();
	#ifdef ENABLE_DOCK
		UART_SendUiElement(5, 0, 0, 0, 0, NULL);
	#endif
  
  if (gEeprom.POWER_ON_DISPLAY_MODE == POWER_ON_DISPLAY_MODE_FULL_SCREEN) {
   // ST7565_FillScreen(0xFF);
  } else {
    char WelcomeString0[16];
    char WelcomeString1[16];

    memset(WelcomeString0, 0, sizeof(WelcomeString0));
    memset(WelcomeString1, 0, sizeof(WelcomeString1));
    if (gEeprom.POWER_ON_DISPLAY_MODE == POWER_ON_DISPLAY_MODE_VOLTAGE) {
      sprintf(WelcomeString0, "VOLTAGE");
	  #ifdef ENABLE_VOLTAGE_PERCENTAGE_WELCOME_MESSAGE
			sprintf(WelcomeString1, "%d.%02dV %d%%", gBatteryVoltageAverage / 100,
              gBatteryVoltageAverage % 100,	BATTERY_VoltsToPercent(gBatteryVoltageAverage));
	  #else
			sprintf(WelcomeString1, "%d.%02dV", gBatteryVoltageAverage / 100,
              gBatteryVoltageAverage % 100);
	  #endif		  
    } else {
      EEPROM_ReadBuffer(0x0EB0, WelcomeString0, 16);
      EEPROM_ReadBuffer(0x0EC0, WelcomeString1, 16);
    }
    UI_PrintString(WelcomeString0, 0, 127, 1, 10, true);
    UI_PrintString(WelcomeString1, 0, 127, 3, 10, true);
          UI_PrintStringSmall(Version, 0, 127, 5);
    UI_PrintStringSmallest(__DATE__ " " __TIME__, 24, 50, false, true);

    ST7565_BlitStatusLine();
    ST7565_BlitFullScreen();
  }
}
