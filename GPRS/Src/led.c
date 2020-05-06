#include "led.h"
#include "time.h"

static tsTimeType TimeLEDs[LED_NUMBER];
static teLEDTaskStatus LEDTaskStatus[LED_NUMBER];
const uint16_t LEDPins[LED_NUMBER] = 
{
	GPIO_PIN_4,
	GPIO_PIN_5,
	GPIO_PIN_6,
	//GPIO_PIN_13
};

void LEDOn(tsLEDNums LEDNums)
{
	HAL_GPIO_WritePin(GPIOA, LEDPins[LEDNums], GPIO_PIN_RESET);
}

void LEDOff(tsLEDNums LEDNums)
{
	HAL_GPIO_WritePin(GPIOA, LEDPins[LEDNums], GPIO_PIN_SET);
}


void LEDInit(void)
{
	int i;
	for(i=0;i<LED_NUMBER;i++)
	{
		LEDOff(i);
	}
}

void SetLEDRun(tsLEDNums LEDNums)
{
	LEDTaskStatus[LEDNums] = LED_RUN;
}

void LEDTask(void)
{
	int i;
	for(i=0;i<LED_NUMBER;i++)
	{
		if(LEDTaskStatus[i] == LED_RUN)
		{
			SetTime(&TimeLEDs[i], 2000);
			LEDOn(i);
			LEDTaskStatus[i] = LED_DELAY;
		}
		else if(LEDTaskStatus[i] == LED_DELAY)
		{
			if(CompareTime(&TimeLEDs[i]))
			{
				LEDOff(i);
				LEDTaskStatus[i] = LED_STOP;
			}
		}
	}
}