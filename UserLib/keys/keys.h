/*
 * keys.h
 *
 *  Created on: 2020年4月13日
 *      Author: WangXiang
 *  LastWrite:2020/4/19
 *  Version:V1.0
 */

#ifndef KEYS_KEYS_H_
#define KEYS_KEYS_H_
#include "UserConfig.h"

void EC11_init();
void E1_turn_left(void (*e1_left_function));
void E1_turn_right(void (*e1_right_function));

void E2_turn_left(void (*e2_left_function));
void E2_turn_right(void (*e2_right_function));

void it_init(uint16_t pin);
void EC11_1_CHECK();

#endif /* KEYS_KEYS_H_ */
