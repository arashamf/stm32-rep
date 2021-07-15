/*
 * led_matrix.h
 *
 *  Created on: 18 мар. 2020 г.
 *      Author: ALEXEY
 */

#ifndef LED_MATRIX_H_
#define LED_MATRIX_H_

uint8_t char_R [8] = {0x78, 0x44, 0x44, 0x78, 0x50, 0x48, 0x44, 0x0}; //буква R

uint8_t led_digit [10][8] =
{
		{0x3C, 0x24, 0x24, 0x24, 0x24, 0x24, 0x3C, 0x00}, //цифра 0
		{0x08, 0x18, 0x28, 0x08, 0x08, 0x08, 0x08, 0x00}, //цифра 1
		{0x3C, 0x04, 0x04, 0x3C, 0x20, 0x20, 0x3C, 0x00}, //цифра 2
		{0x3C, 0x04, 0x04, 0x3C, 0x04, 0x04, 0x3C, 0x00}, //цифра 3
		{0x24, 0x24, 0x24, 0x3C, 0x04, 0x04, 0x04, 0x00}, //цифра 4
		{0x3C, 0x20, 0x20, 0x3C, 0x04, 0x04, 0x3C, 0x00}, //цифра 5
		{0x3C, 0x20, 0x20, 0x3C, 0x24, 0x24, 0x3C, 0x00}, //цифра 6
		{0x3C, 0x04, 0x08, 0x10, 0x20, 0x20, 0x20, 0x00}, //цифра 7
		{0x3C, 0x24, 0x24, 0x3C, 0x24, 0x24, 0x3C, 0x00}, //цифра 8
		{0x3C, 0x24, 0x24, 0x3C, 0x04, 0x04, 0x3C, 0x00} //цифра 9*/
};

/*uint8_t numb_0 [8] = {0x3C, 0x24, 0x24, 0x24, 0x24, 0x24, 0x3C, 0x00}; //цифра 0
uint8_t numb_1 [8] = {0x08, 0x18, 0x28, 0x08, 0x08, 0x08, 0x08, 0x00}; //цифра 1
uint8_t numb_2 [8] = {0x3C, 0x04, 0x04, 0x3C, 0x20, 0x20, 0x3C, 0x00}; //цифра 2
uint8_t numb_3 [8] = {0x3C, 0x04, 0x04, 0x3C, 0x04, 0x04, 0x3C, 0x00}; //цифра 3
uint8_t numb_4 [8] = {0x24, 0x24, 0x24, 0x3C, 0x04, 0x04, 0x04, 0x00}; //цифра 4
uint8_t numb_5 [8] = {0x3C, 0x20, 0x20, 0x3C, 0x04, 0x04, 0x3C, 0x00}; //цифра 5
uint8_t numb_6 [8] = {0x3C, 0x20, 0x20, 0x3C, 0x24, 0x24, 0x3C, 0x00}; //цифра 6
uint8_t numb_7 [8] = {0x3C, 0x04, 0x08, 0x10, 0x20, 0x20, 0x20, 0x00}; //цифра 7
uint8_t numb_8 [8] = {0x3C, 0x24, 0x24, 0x3C, 0x24, 0x24, 0x3C, 0x00}; //цифра 8
uint8_t numb_9 [8] = {0x3C, 0x24, 0x24, 0x3C, 0x04, 0x04, 0x3C, 0x00}; //цифра 9*/
#endif /* LED_MATRIX_H_ */
