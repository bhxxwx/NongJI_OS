/*
 * SPI_FLASH.h
 *
 *  Created on: 2020年5月22日
 *      Author: WangXiang
 */

#ifndef SPI_FLASH_SPI_FLASH_H_
#define SPI_FLASH_SPI_FLASH_H_
#include "UserConfig.h"
#include <stdarg.h>



#define  FLASH_WRITE_ENABLE_CMD 		0x06
#define  FLASH_WRITE_DISABLE_CMD		0x04
#define  FLASH_READ_SR_CMD				0x05
#define  FLASH_WRITE_SR_CMD				0x01
#define  FLASH_READ_DATA				0x03
#define  FLASH_FASTREAD_DATA			0x0b
#define  FLASH_WRITE_PAGE				0x02
#define  FLASH_ERASE_PAGE      			0x81
#define  FLASH_ERASE_SECTOR       		0x20
#define	 FLASH_ERASE_BLOCK				0xd8
#define	 FLASH_ERASE_CHIP				0xc7
#define  FLASH_POWER_DOWN				0xb9
#define  FLASH_RELEASE_POWER_DOWN       0xab
#define  FLASH_READ_DEVICE_ID      		0x90
#define  FLASH_READ_JEDEC_ID      		0x9f

#define 	FLASH_SIZE	 (1*1024*1024)	// 1M字节
#define		PAGE_SIZE			8192	// 256 bytes
#define 	SECTOR_SIZE		512	 // 4-Kbyte
#define		BLOCK_SIZE		32	// 64-Kbyte

#define PAGE_LEN		255	 //一页256字节

#define CS_HIGH digitalWriteA(GPIO_Pin_4, HIGH)
#define CS_LOW digitalWriteA(GPIO_Pin_4, LOW)

extern int SPI_printf(uint32_t page, const char *fmt, ...);

void SPI_INIT();
void SPI_write(u8 TxData);
void SPI_writeStr(uint32_t page, char *str);

void Flash_WriteEnable(void);

uint8_t SPI_MasterSendReceiveByte(uint8_t spi_byte);

void SPI_MasterSendReceiveMultipeByte(uint8_t *spi_byte);

void WriteEN();
void SPI_WriteFlash();
void ERDI();

void CheckBusy();

void ts_itoa(char **buf, unsigned int d, int base);
int ts_formatstring(char *buf, const char *fmt, va_list va);
int ts_formatlength(const char *fmt, va_list va);

#endif /* SPI_FLASH_SPI_FLASH_H_ */
