#ifndef _TIME_H
#define _TIME_H
#include "stm32f1xx.h"
typedef struct
{
	uint32_t TimeStart;//��ʼʱ��
	uint32_t TimeInter;//���ʱ��
}tsTimeType;
void SetTime(tsTimeType *TimeType,uint32_t TimInter);//����ʱ��������
uint8_t CompareTime(tsTimeType *TimeType);//�ȽϺ���,�ж϶�ʱʱ���Ƿ񵽴�

#endif


