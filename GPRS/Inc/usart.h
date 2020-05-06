/**
  ******************************************************************************
  * File Name          : USART.h
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

/* USER CODE BEGIN Private defines */
#define USART2_DMA_REC_SIZE 256
#define USART2_REC_SIZE 1024
	 
#define USART3_DMA_REC_SIZE 256
#define USART3_REC_SIZE 1024
extern void EnableUartIT(void);
typedef struct
{
	uint8_t Usart2RecFlag;//数据接收到标志位
	uint16_t Usart2DMARecLen;//DMA接受长度
	uint16_t Usart2RecLen;//处理数据长度
	uint8_t Usart2DMARecBuff[USART2_DMA_REC_SIZE];//DMAbuffer
	uint8_t Usart2RecBuff[USART2_REC_SIZE];//数据处理buffer
}tsUsart2Type;
extern tsUsart2Type Usart2Type;

typedef struct
{
	uint8_t Usart3RecFlag;//数据接收到标志位
	uint16_t Usart3DMARecLen;//DMA接受长度
	uint16_t Usart3RecLen;//处理数据长度
	uint8_t Usart3DMARecBuff[USART3_DMA_REC_SIZE];//DMAbuffer
	uint8_t Usart3RecBuff[USART3_REC_SIZE];//数据处理buffer
}tsUsart3Type;
extern tsUsart3Type Usart3Type;

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
void MX_USART3_UART_Init(void);

/* USER CODE BEGIN Prototypes */
void MyUsartTransmit(char *Buff, uint16_t len);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
