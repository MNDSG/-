#ifndef _TIME_H
#define _TIME_H
#include "stm32f1xx.h"
typedef struct
{
	uint32_t TimeStart;//开始时间
	uint32_t TimeInter;//间隔时间
}tsTimeType;
void SetTime(tsTimeType *TimeType,uint32_t TimInter);//设置时间间隔函数
uint8_t CompareTime(tsTimeType *TimeType);//比较函数,判断定时时间是否到达

#endif


