/*
 * usb_printf.c
 *
 *  Created on: 2020年6月15日
 *      Author: wxujs
 */
#include "USB/inc/usb_printf.h"
#include "USB/inc/hw_config.h"
#include "UserConfig.h"
#include <stdarg.h>
__attribute__((alias("USB_iprintf"))) int USB_printf(const char *fmt, ...);

int USB_iprintf(const char *fmt, ...)
{
	int length = 0;
	va_list va;
	va_start(va, fmt);
	length = ts_formatlength(fmt, va);
	va_end(va);
	{
		char buf[length];
		va_start(va, fmt);
		length = ts_formatstring(buf, fmt, va);

		 USB_TxWrite((uint8_t *)buf, length);
		va_end(va);
	}
	return length;
}
