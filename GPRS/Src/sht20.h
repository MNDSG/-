#ifndef _SHT20_H
#define _SHT20_H

#include "stm32f1xx_hal.h"
#include "i2c.h"

#define POLYNOMIAL 0x131  //P(x)=x^8+x^5+x^4+1 = 100110001,生成多项式宏定义

/*
	SHT20命令枚举
*/
typedef enum
{
	TRI_T_MEA_HM = 0xE3,//触发温度测量(主机模式)
	TRI_RH_MEA_HM = 0xE5,//触发湿度测量(主机模式)
	TRI_T_MEA_NHM = 0xF3,//触发温度测量(非主机模式)
	TRI_RH_MEA_NUM = 0xF5,//触发湿度测量(非主机模式)
	USER_REG_W = 0xE6,//读寄存器
	USER_REG_R = 0xE7,//写寄存器
	SOFT_RESET = 0xFE//软件复位
}teSHT20Cmd;

/*
	SHT20作为从机的地址
*/
typedef enum
{
	Addr_R = 0x81,//从机地址(1000 000)+读(1)
	Addr_W = 0x80//从机地址(1000 000)+写(1)
}tySHT20Addr;

/*
	SHT20采样模式枚举
*/
//typedef enum
//{
//	SHT2x_RES_12_14BIT       = 0x00, // RH=12bit, T=14bit
//  SHT2x_RES_8_12BIT        = 0x01, // RH= 8bit, T=12bit
//  SHT2x_RES_10_13BIT       = 0x80, // RH=10bit, T=13bit
//  SHT2x_RES_11_11BIT       = 0x81, // RH=11bit, T=11bit
//  SHT2x_RES_MASK           = 0x81  
//}teSHT20Mode;

/*
	SHT20采样数据类型
*/
typedef enum
{
  HUMIDITY,//读取湿度
  TEMP//读取温度
}etSHT20MeasureType;

void SHT20Ret(void);
uint8_t Red_Value(etSHT20MeasureType SHT20MeasureType, float* Value);
//void SetMode(teSHT20Mode SHT20Mode);
//uint8_t CheckCrc(uint8_t *data, uint8_t nbrOfBytes, uint8_t checksum);
uint8_t SHT20Calc2FloatTemp(uint8_t *data, float * Temp);
uint8_t SHT20Calc2FloatHum(uint8_t *data, float * Hum);

#endif


