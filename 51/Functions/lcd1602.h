#ifndef _lcd1602_H
#define _lcd1602_H

#include "public.h"

//LCD1602数据口4位或8位模式，0为1602的8位数据口模式，1为4位
#define LCD1602_4OR8_DATA_INTERFACE	0	//默认使用8位数据口LCD1602

//引脚定义
sbit LCD1602_RS=P2^6;//寄存器选择
sbit LCD1602_RW=P2^5;//读写选择
sbit LCD1602_E=P2^7; //使能信号
#define LCD1602_DATAPORT P0	//定义LCD1602数据端口


//函数声明
void lcd1602_init(void);
void lcd1602_clear(void);
void lcd1602_show_string(u8 x,u8 y,u8 *str);

#endif