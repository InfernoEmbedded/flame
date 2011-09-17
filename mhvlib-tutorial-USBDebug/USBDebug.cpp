/*
 * Copyright (c) 2011, Make, Hack, Void Inc
 * All rights reserved.
 *
 * License: GNU GPL v2 (see License.txt), GNU GPL v3
 */

/* Demonstrate the USB debugging facilities
 * Written for the AVR Stick http://www.sparkfun.com/commerce/product_info.php?products_id=9147
 */

#include <avr/pgmspace.h>
#include <MHV_USBDebug.h>

// Taken from usb_debug_only.c from Teensy Blinky code: http://www.pjrc.com/teensy/gcc.html
static uint8_t PROGMEM usbHidReportDescriptor[] = {
	0x06, 0x31, 0xFF,			// Usage Page 0xFF31 (vendor defined)
	0x09, 0x74,					// Usage 0x74
	0xA1, 0x53,					// Collection 0x53
	0x75, 0x08,					// report size = 8 bits
	0x15, 0x00,					// logical minimum = 0
	0x26, 0xFF, 0x00,			// logical maximum = 255
	0x95, MHV_USBDEBUG_TX_SIZE, // report count
	0x09, 0x75,					// usage
	0x81, 0x02,					// Input (array)
	0xC0						// end collection
};

