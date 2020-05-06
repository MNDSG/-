#ifndef _NBIOTDRIVER_H
#define _NBIOTDRIVER_H
#include "stm32f1xx.h"
#include <ctype.h>
#include <stdlib.h>
#include "usart.h"


typedef enum
{
	
	ATI = 0,											//��ѯģ����Ϣ
	AT_CGRET,									//�鿴GPRSע��״̬
	AT_CGATT,									//�鿴GPRS����״̬
	
	AT_MCONFIG,   						//����client ID   
	AT_MIPSTART, 							//����IP�˿�
	AT_MCONNECT,   						//����MQTT�Ự
	AT_MSUB,    							//��������
	AT_MQTTMODE,							//������Ϣ��ʽΪASCII
	AT_CSQ,   								//��ѯ�ź�
	AT_MPUB, 	 								//������Ϣ
	AT_MDISCONNECT,   				//�ر�MQTT����
	AT_MIPCLOSE,   						//�ر�TCP����
	
	AT_RESET, 						//��λģ��
	AT_IDIE,
}teATCmdNum;

typedef enum	//ATָ������״̬
{
	NB_IDIE = 0,//����
	NB_SEND,//����
	NB_WAIT,//�ȴ�
	NB_ACCESS//�������
}teNBTaskStatus;

typedef enum
{
	SUCCESS_REC = 0,
	TIME_OUT,
	NO_REC
}teATStatus;
typedef struct
{
	char *ATSendStr;//ATָ��͵��ַ���
	char *ATRecStr;//ATָ����ܵ��ַ���
	uint16_t TimeOut;//��ʱ
	teATStatus ATStatus;//ATָ��״̬
	uint8_t RtyNum;//�ط�����
}tsATCmds;

void NB_RESET(void);//NB��λ
void DealData(void);//���ݴ�����
void NB_Init(void);
void NB_Task(void);
int Find_string(char *pcBuf,char *left,char *right, char *pcRes);//��һ���ַ������ҵ�ָ���ַ���
#endif

