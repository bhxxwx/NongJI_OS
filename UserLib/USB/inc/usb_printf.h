/*
 * usb_printf.h
 *
 *  Created on: 2020年6月15日
 *      Author: wxujs
 */

#ifndef USB_INC_USB_PRINTF_H_
#define USB_INC_USB_PRINTF_H_
#include "UserConfig.h"
#include <stdarg.h>


extern int USB_printf(const char *fmt, ...);
void ts_itoa(char **buf, unsigned int d, int base);
int ts_formatstring(char *buf, const char *fmt, va_list va);
int ts_formatlength(const char *fmt, va_list va);


#endif /* USB_INC_USB_PRINTF_H_ */
