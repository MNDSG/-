#include "time.h"

void SetTime(tsTimeType *TimeType,uint32_t TimInter)
{
	TimeType->TimeStart = HAL_GetTick();//��ȡϵͳ����ʱ��
	TimeType->TimeInter = TimInter;//�趨��ʱʱ����
}
uint8_t CompareTime(tsTimeType *TimeType)
{
	return ((HAL_GetTick() - TimeType->TimeStart) >= TimeType->TimeInter);//����1�򵽴ﶨʱʱ��
}


