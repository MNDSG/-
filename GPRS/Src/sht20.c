#include "sht20.h"
#include "stm32f1xx_hal.h"
#include "usart.h"
#include <string.h>




/**
 * @brief    ��λSHT20
 * @param    none
 * @retval    none
*/
void SHT20Ret(void)
{
	HAL_I2C_Master_Transmit(&hi2c1, Addr_W, (uint8_t *)SOFT_RESET, 1, 0xFFFF);
	HAL_Delay(20);
}

/**
 * @brief    ����SHT20����ģʽ
 * @param    ���ö��ָ��
 * @retval   HAL_OK
*/
//void SetMode(teSHT20Mode SHT20Mode)
//{
//	HAL_I2C_Master_Transmit(&hi2c1, Addr_W, (uint8_t *)USER_REG_R, 1, 0xFFFF);
//	HAL_I2C_Master_Transmit(&hi2c1, Addr_W, (uint8_t *)SHT20Mode, 1, 0xFFFF);
//}


/**
 * @brief    CRCУ��
 * @param    data--��Ŵ������������ݵ�����ָ��,nbrOfBytes--����Ԫ�ظ���,checksum--���������ص�У����
 * @retval   0or1
*/
//uint8_t CheckCrc(uint8_t *data, uint8_t nbrOfBytes, uint8_t checksum)
//{
//	uint8_t crc;
//	uint8_t i;
//	uint8_t j;
//	for(i = 0;i < nbrOfBytes; i++)
//	{
//		crc ^= data[i];
//		for(j = 0;j < 8; j++)
//		{
//			if(crc & 0x80)
//			{
//				crc = (crc<<1)^ POLYNOMIAL;
//			}
//			else
//			{
//				crc = (crc<<1);
//			}
//		}
//	}
//	if(crc != checksum)
//	{
//		return 1;
//	}
//	else
//	{
//		return 0;
//	}
//}

/**
 * @brief    ���¶�����ת��Ϊ��������
 * @param    data--��Ŵ������������ݵ�����ָ��,Temp--ָ��ת�������ݵ�ָ��
 * @retval   0
*/
uint8_t SHT20Calc2FloatTemp(uint8_t *data, float * Temp)
{
	uint16_t RecTemp;
	data[1] &= ~0x03;//��2λ��0
	RecTemp = (((uint16_t)data[0])<<8)|data[1];//�ϲ�data����
	*Temp = -46.85 + 175.72 / 65536 * RecTemp;
	return 0;
}

/**
 * @brief    ��ʪ������ת��Ϊ��������
 * @param    data--��Ŵ������������ݵ�����ָ��,Hum--ָ��ת�������ݵ�ָ��
 * @retval   0
*/
uint8_t SHT20Calc2FloatHum(uint8_t *data, float * Hum)
{
	uint16_t RecHum;
	data[1] &= ~0x03;//��2λ��0
	RecHum = (((uint16_t)data[0])<<8)|data[1];
	//*Hum = -6 + 125 / 65536 * RecHum;
	*Hum = ((float)RecHum * 0.00190735) - 6;
	return 0;
}


/**
 * @brief    ��ȡ��������ʪ��ֵ
 * @param    SHT20MeasureType--��Ҫ��ȡ����������(�¶�orʪ��),Value--ָ��ת�������ݵ�ָ��
 * @retval   �ɹ�����0,���򷵻�1
*/
uint8_t Red_Value(etSHT20MeasureType SHT20MeasureType, float* Value)
{
	uint8_t resultnum[2];//����¶Ȼ�ʪ�ȶ�����ֵ
//	uint8_t checksum = 0;//���У���ֽ�
	switch(SHT20MeasureType)
	{
		case HUMIDITY:
			//HAL_I2C_Master_Transmit(&hi2c1, Addr_W, (uint8_t *)TRI_RH_MEA_NUM, 1, 0xFFFF);
			HAL_I2C_Mem_Read(&hi2c1, Addr_R, TRI_RH_MEA_HM, I2C_MEMADD_SIZE_8BIT,resultnum,2, 0xFFFF);
			break;
		case TEMP:
			//HAL_I2C_Master_Transmit(&hi2c1, Addr_W, (uint8_t *)TRI_T_MEA_NHM, 1, 0xFFFF);
			HAL_I2C_Mem_Read(&hi2c1, Addr_R, TRI_T_MEA_HM, I2C_MEMADD_SIZE_8BIT,resultnum,2, 0xFFFF);
			break;
		default:
			HAL_UART_Transmit(&huart1,(uint8_t *)"undefine measure type",strlen("undefine measure type"),0xFFFF);
	}
//	HAL_I2C_Master_Receive(&hi2c1, Addr_R, resultnum, 2, 0xFFFF);
//	HAL_I2C_Master_Receive(&hi2c1, Addr_R, &checksum, 1, 0xFFFF);
//	if(CheckCrc(data, 2, checksum))
//	{
//		HAL_UART_Transmit(&huart1,(uint8_t *)"check error",strlen("check error"),0xFFFF);
//		return 0;
//	}
	switch(SHT20MeasureType)
	{
		case HUMIDITY:
			SHT20Calc2FloatHum(resultnum, Value);
			break;
		case TEMP:
			SHT20Calc2FloatTemp(resultnum, Value);
			break;
	}
	return 0;
}


