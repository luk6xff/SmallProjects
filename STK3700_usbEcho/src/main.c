/**************************************************************************//**
 * @file main.c
 * @brief USB Composite Device example.
 * @version 3.20.5
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_timer.h"
#include "em_emu.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "utils.h"
#include "../drivers/segmentlcd.h"
#include "../drivers/sleep.h"
#include "../drivers/usb/cdc.h"


/**************************************************************************//**
 * @brief main - the entrypoint after reset.
 *****************************************************************************/
int main(
   )
{
	BSP_TraceProfilerSetup();
	CHIP_Init();
	CMU_OscillatorEnable(cmuOsc_HFXO, true, true);
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO); //32MHZ

	/* Initialize LCD driver */
	SegmentLCD_Init(false);
	SegmentLCD_Write("usbcomp");
	SegmentLCD_Symbol(LCD_SYMBOL_GECKO, true);

	/* Initialize LED driver */
	BSP_LedsInit();

	if (CDC_Init()) { /* Initialize the communication class device. */
		SegmentLCD_Write("done");
	}

	else {
		SegmentLCD_Write("fail");
		while (1)
		;
	}
	/*
	 // When using a debugger it is practical to uncomment the following three
	 // lines to force host to re-enumerate the device.

	 USBD_Disconnect();
	 USBTIMER_DelayMs(1000);
	 USBD_Connect();
	 */
	uint8_t buf[255] = {0};
    uint32_t counter=0;
	for (;;) {
		if (usbReadData(buf, 0)) {

			USB_DEBUG_PUTS((char*)buf);
			//SegmentLCD_Number(counter++);
		}
	}
}


