/*
 * IOT.h
 *
 *  Created on: 2020年7月31日
 *      Author: ZRH
 */

#ifndef IOT_IOT_H_
#define IOT_IOT_H_
#include "UserConfig.h"

void IOT_Reset(int mode);
void FullSystemReset(int mode);
void Connect_MQTT_again(int mode);
void Clear_receives();
void Clear_receives_newline(char line);
void send_cmd(char *str);
void choice_delay(int tim,int mode);
void IOT_Init(int mode);
bool check_Ping();
bool Ping_NET(char N,char time,int mode);
bool iscontants(char *str, char *cmd);
bool check_receives(char *cmd);


#endif /* IOT_IOT_H_ */
