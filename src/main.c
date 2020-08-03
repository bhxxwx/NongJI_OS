/*		江苏大学
 * 农业装备学院工程学院
 *
 * 收割机项目,使用FreeRTOS操作系统
 * 使用 Github 进行版本控制
 *
 * 本项目的UserLib将作为独立驱动项目<已完成与Servers文件的解耦工作>,请注意保持该文件夹的独立性!!
 *
 * 本项目所有标点均为英文标点!
 *
 * 所有非驱动服务函数请在Servers文件夹内开发,
 * 		其中:DATAstruct文件夹内为自定义的数据结构体,
 * 			 Functions文件夹内为服务函数.
 *
 * UserLib为底层驱动文件夹.
 *
 * Author: WangXiang
 * 20200610
 */
/*include*/
#include "stm32f10x.h"
#include <stdio.h>
#include <stddef.h>
#include "UserConfig.h"//用户库调用
#include "IOT/IOT.h"
#include "stm32f10x_spi.h"

/*操作系统库函数调用*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

char Deviceerr = 0;													//4G设备故障
char error_handling = 0;				//标志位——错误处理，防止处理函数多重嵌套

void Send_save_message()
{
	while (1)
	{
		delay_us(5000000);
//		vTaskDelay(5000);    //单位2ms
		send_cmd("AT+QMTPUB=0,0,0,1,\"/a1f2CH9BSx7/ZRH_4G/user/put\" \r\n");
		delay_us(100000); //0.1s
		send_cmd("{\"adc\":5} \r\n");

		delay_us(300000); //0.3s
//		vTaskDelay(400);    //单位2ms

		if (check_receives("+QMTPUB: 0,0,0"))
		{
			Deviceerr = 0;
			digitalWriteC(GPIO_Pin_14, LOW);		 //连网指示灯亮
		} else if (check_receives("ERROR"))
		{
			Deviceerr = 1;
			digitalWriteC(GPIO_Pin_14, HIGH);		 //连网指示灯灭
		} else if (Message_again() == 0)			 //检测不到就重发三次，还不成功就重连
		{
			Deviceerr = 1;
			digitalWriteC(GPIO_Pin_14, HIGH);		 //连网指示灯灭
		}

	}
}

void Exception_handler()
{
	while (1)
	{
		vTaskDelay(500);    //单位2ms
		if (Deviceerr == 1 && error_handling == 0)
		{
			error_handling = 1;
			Connect_MQTT_again(0);
			error_handling = 0;
		}
	}
}

int main(void)
{
	nvic_init();
	pinModeB(GPIO_Pin_5, OUTPUT);			 //IOT设备复位引脚
	pinModeC(GPIO_Pin_14, OUTPUT);			 //连网成功指示灯

	digitalWriteB(GPIO_Pin_5, LOW);
	delay_us(500000);					//0.5s
	digitalWriteB(GPIO_Pin_5, HIGH);
	delay_us(10000000);
	digitalWriteC(GPIO_Pin_14, HIGH);					//连网成功指示灯——低电平点亮

	usart_1_init(115200);
	delay_us(100000); 		//延时0.1s
	printf("AT+QIACT=1 \r\n");		 //设置一次即可，用于ping网检测使用
	delay_us(100000); 		//延时0.1s
	printf("ATE0 \r\n"); 		//设置一次即可，用于ping网检测使用
	delay_us(100000); 		//延时0.1s
	IOT_Init(1);

	xTaskCreate(Send_save_message, "sendtext", 1024, NULL, 2, NULL); 	//上传数据到阿里云且缓存数据
//	xTaskCreate(Exception_handler, "handler", 3000, NULL, 4, NULL); 	//异常处理线程
	vTaskStartScheduler();												//开启多线程

	while (1)
	{
		;
	}
}
