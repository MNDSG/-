#ifndef _SHT20_H
#define _SHT20_H

#include "stm32f1xx_hal.h"
#include "i2c.h"

#define POLYNOMIAL 0x131  //P(x)=x^8+x^5+x^4+1 = 100110001,���ɶ���ʽ�궨��

/*
	SHT20����ö��
*/
typedef enum
{
	TRI_T_MEA_HM = 0xE3,//�����¶Ȳ���(����ģʽ)
	TRI_RH_MEA_HM = 0xE5,//����ʪ�Ȳ���(����ģʽ)
	TRI_T_MEA_NHM = 0xF3,//�����¶Ȳ���(������ģʽ)
	TRI_RH_MEA_NUM = 0xF5,//����ʪ�Ȳ���(������ģʽ)
	USER_REG_W = 0xE6,//���Ĵ���
	USER_REG_R = 0xE7,//д�Ĵ���
	SOFT_RESET = 0xFE//�����λ
}teSHT20Cmd;

/*
	SHT20��Ϊ�ӻ��ĵ�ַ
*/
typedef enum
{
	Addr_R = 0x81,//�ӻ���ַ(1000 000)+��(1)
	Addr_W = 0x80//�ӻ���ַ(1000 000)+д(1)
}tySHT20Addr;

/*
	SHT20����ģʽö��
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
	SHT20������������
*/
typedef enum
{
  HUMIDITY,//��ȡʪ��
  TEMP//��ȡ�¶�
}etSHT20MeasureType;

void SHT20Ret(void);
uint8_t Red_Value(etSHT20MeasureType SHT20MeasureType, float* Value);
//void SetMode(teSHT20Mode SHT20Mode);
//uint8_t CheckCrc(uint8_t *data, uint8_t nbrOfBytes, uint8_t checksum);
uint8_t SHT20Calc2FloatTemp(uint8_t *data, float * Temp);
uint8_t SHT20Calc2FloatHum(uint8_t *data, float * Hum);

#endif


