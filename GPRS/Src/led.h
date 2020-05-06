#ifndef _LED_H
#define _LED_H
#include "stm32f1xx.h"
#define LED_NUMBER 3
typedef enum
{
	LED_NET = 0,
	LED_RX,//��������
	LED_TX//��������
}tsLEDNums;

typedef enum
{
	LED_STOP = 0,
	LED_RUN,
	LED_DELAY
}teLEDTaskStatus;

void SetLEDRun(tsLEDNums LEDNums);
void LEDInit(void);
void LEDTask(void);
#endif

