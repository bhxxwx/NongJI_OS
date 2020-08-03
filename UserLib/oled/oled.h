/*
 * oled.h
 *
 *  Created on: 2020年4月11日
 *      Author: WangXiang
 *  LastWrite:2020/4/19
 *  Version:V2.0
 */

#ifndef OLED_OLED_H_
#define OLED_OLED_H_
#include "UserConfig.h"
#include <stdarg.h>
//#include "Gen/GenLib.h"

#define XLevelL 0x00 //低位地址
#define XLevelH 0x10 //高位地址
#define XLevel ((XLevelH & 0x0F) * 16 + XLevelL)
#define Max_Column 128  //最大屏幕像素宽度
#define Max_Row 64      //最大屏幕像素宽度
#define Brightness 0x78 //IIC起始地址
#define X_WIDTH 128     //屏幕像素宽度
#define Y_WIDTH 64      //屏幕像素高度

void IIC_delayus(unsigned int i); //IIC通讯延时(一般用不到)

void IIC_start(void);                         //开始IIC通信
void IIC_stop(void);                          //结束IIC通信
void IIC_writebyte(unsigned char IIC_byte);   //写字节
void IIC_writecmd(unsigned char IIC_command); //写命令
void IIC_writedata(unsigned char IIC_data);   //写数据

void oled_Set_Pos(unsigned char x, unsigned char y);                                                            //设置显示位置
void oled_CLS(void);                                                                                            //清屏
void oled_Init(void);                                                                                           //OLED屏初始化
void oled_ShowChar(unsigned char x, unsigned char y, unsigned char chr);                                        //显示字符
void oled_ShowString(unsigned char x, unsigned char y, unsigned char *chr);                                     //显示字符串
void oled_ShowNum(unsigned char x, unsigned char y, unsigned long num, unsigned char len, unsigned char size2); //显示数字
unsigned long oled_pow(unsigned char m, unsigned char n);                                                       //字符求幂函数

void ts_itoa(char **buf, unsigned int d, int base);
int ts_formatstring(char *buf, const char *fmt, va_list va);
int ts_formatlength(const char *fmt, va_list va);

#endif /* OLED_OLED_H_ */
