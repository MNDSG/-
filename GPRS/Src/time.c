#include "time.h"

void SetTime(tsTimeType *TimeType,uint32_t TimInter)
{
	TimeType->TimeStart = HAL_GetTick();//获取系统运行时间
	TimeType->TimeInter = TimInter;//设定定时时间间隔
}
uint8_t CompareTime(tsTimeType *TimeType)
{
	return ((HAL_GetTick() - TimeType->TimeStart) >= TimeType->TimeInter);//返回1则到达定时时间
}


