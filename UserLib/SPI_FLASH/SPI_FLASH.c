/*
 * SPI_FLASH.c
 *
 *  Created on: 2020年5月22日
 *      Author: WangXiang
 */
#include "SPI_FLASH.h"
#include "UserConfig.h"
#include <stdarg.h>
__attribute__((alias("SPI_iprintf"))) int SPI_printf(uint32_t page, const char *fmt, ...);

void SPI_INIT()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;					   //设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;				   //设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;						   //选择了串行时钟的稳态:时钟悬空高
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;					   //数据捕获于第二个时钟沿
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;						   //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; //定义波特率预分频的值:波特率预分频值为8-9M
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				   //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;						   //CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);								   //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

	//	SPI_NSSInternalSoftwareConfig(SPI1,SPI_NSSInternalSoft_Reset);
	pinModeA(GPIO_Pin_4, OUTPUT);
//	SPI_SSOutputCmd(SPI1, ENABLE);
	SPI_Cmd(SPI1, ENABLE); //使能SPI外设
	CS_HIGH;

	u8 retry=0;
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
		{
		retry++;
		if(retry>200)return ;
		}
	SPI_I2S_SendData(SPI1, 0xFF); //通过外设SPIx发送一个数据
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)//检查指定的SPI标志位设置与否:接受缓存非空标志位
		{
		retry++;
		if(retry>200)return ;
		}
	SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据
}

void SPI_write(u8 TxData)
{
	u8 retry = 0;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
	{
		retry++;
		if (retry > 200)
			return;
	}
	SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个数据
}

void SPI_writeStr(uint32_t page, char *str)
{
	SPI_write((page >> 16) & (0xFF));
	SPI_write((page >> 8) & (0xFF));
	SPI_write((page) & (0xFF));
	uint16_t j = 0;
	while (str[j] != '\0') //遍历字符串，直到字符串为空
	{
		if (j < 256)
		{
			SPI_write(str[j]);
			j++;
		}
		else
			break;
	}

	if (j >= 256)
	{
		SPI_write(((page + 256) >> 16) & (0xFF));
		SPI_write(((page + 256) >> 8) & (0xFF));
		SPI_write(((page + 256)) & (0xFF));
		while (str[j] != '\0') //遍历字符串，直到字符串为空
		{
			SPI_write(str[j]);
			j++;
		}
	}
}

void WriteEN()
{
	CS_LOW;
	SPI_write(FLASH_WRITE_ENABLE_CMD);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
	CS_HIGH;
}

void CheckBusy()
{
	uint8_t flag;

	while(flag)
	{
		CS_LOW;
		SPI_write(FLASH_READ_SR_CMD);
		flag=SPI_MasterSendReceiveByte(0x00);

		delay_us(1);
		CS_HIGH;
	}


}
void ERDI()
{
	//	delay_us(1);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
	CS_HIGH;
	CS_LOW;
	SPI_write(0x20);
	//	delay_us(1);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
	CS_HIGH;
}

int SPI_iprintf(uint32_t page, const char *fmt, ...)
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
		//		WREN();
		CS_LOW;
		SPI_write(0x02);

		SPI_writeStr(page, buf);
		CS_HIGH;
		//		ERDI();

		va_end(va);
	}
	return length;
}





uint8_t SPI_MasterSendReceiveByte(uint8_t spi_byte)
{
	uint8_t count=0;
	SPI_write(spi_byte);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)//检查指定的SPI标志位设置与否:接受缓存非空标志位
	{
		count++;
		if(count>200)
			return false;
	}
	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据
}
