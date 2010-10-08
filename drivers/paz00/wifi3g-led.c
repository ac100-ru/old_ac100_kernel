#include <linux/dmi.h>
#include <linux/sched.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include "nvec.h"
#include "nvodm_query_discovery.h"
#include "nvodm_query_gpio.h"
#include "nvodm_services.h"


#if ((defined EVT_HARDWARE_LAYOUT && defined DVT_HARDWARE_LAYOUT) || \
	 (defined EVT_HARDWARE_LAYOUT && defined PVT_HARDWARE_LAYOUT) || \
	 (defined DVT_HARDWARE_LAYOUT && defined PVT_HARDWARE_LAYOUT))
#error multi defined hardware layout
#endif

#define	WLAN_SHUTDOWN_PIN	0
#define	WLAN_RESET_PIN		1
#define	WLAN_LED_PIN		2


static NvOdmServicesGpioHandle s_hGpio;
static const NvOdmGpioPinInfo *s_gpioPinTable;

static NvOdmGpioPinHandle s_hWlanShutdownPin;
static NvOdmGpioPinHandle s_hWlanResetPin;
static NvOdmGpioPinHandle s_hWlanLedPin;

unsigned char gWlanLedStatus = 0;


void set_wlan_led(unsigned mode)
{
	NvU32 gpioPinCount;

	if( gWlanLedStatus == mode )
	{
		return;
	}
	else
	{
		gWlanLedStatus = mode;
	}

	if( !s_hGpio )
	{
		s_hGpio = NvOdmGpioOpen();
		if( !s_hGpio )
		{
			return;
		}
	}

	s_gpioPinTable = NvOdmQueryGpioPinMap(NvOdmGpioPinGroup_Wlan, 0, &gpioPinCount);
	if( gpioPinCount <= 0 )
	{
		return;
	}

#ifdef	PVT_HARDWARE_LAYOUT
	s_hWlanLedPin = NvOdmGpioAcquirePinHandle(s_hGpio,
		s_gpioPinTable[WLAN_LED_PIN].Port,
		s_gpioPinTable[WLAN_LED_PIN].Pin);
		NvOdmGpioConfig(s_hGpio, s_hWlanLedPin, NvOdmGpioPinMode_Output);
#endif
	s_hWlanShutdownPin = NvOdmGpioAcquirePinHandle(s_hGpio,
		s_gpioPinTable[WLAN_SHUTDOWN_PIN].Port,
		s_gpioPinTable[WLAN_SHUTDOWN_PIN].Pin);
		NvOdmGpioConfig(s_hGpio, s_hWlanShutdownPin, NvOdmGpioPinMode_Output);

	// enable or disable WLAN led
#ifdef	PVT_HARDWARE_LAYOUT
	NvOdmGpioSetState(s_hGpio, s_hWlanLedPin, mode);
#endif
	NvOdmGpioSetState(s_hGpio, s_hWlanShutdownPin, mode);
	NvOdmOsWaitUS(300);

    return;
}

