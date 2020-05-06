#include "nbiotdriver.h"
#include "stm32f1xx.h"
#include "usart.h"
#include <string.h>
#include "time.h"
#include "led.h"
#include "stdio.h"
#include "sht20.h"
#include "gps.h"

static uint8_t CurrentRty;//��¼��ǰָ��ִ�д���
static tsTimeType TimeNB;//����ʱ��״̬�ṹ��,�����жϽ���ָ���Ƿ�ʱ
static tsTimeType TimeSend;//��ʱ��������
static teATCmdNum ATRecCmdNum;
static teATCmdNum ATCmdNumCurrent;//��ǰ����
static teATCmdNum ATCmdNumNext;//��һ��ָ��
static teNBTaskStatus NBTaskStatus;
tsMessage Message;
tsMessageBuff GPSMessageBuff;

float heigth;
float temperature;
float humidity;
uint8_t IMEI_Flag = 0;
uint8_t find_string_flag =0;
uint8_t my_sleep = 0; //���߿���(0�ر�)
uint8_t NB_reset_time = 5;                                                 //��λ����


char CSQ_buffer[3] = {1};
char topic_msg[44];
char IMEI_Buff[22];
char NB_SendDataBuff[1024];


tsATCmds ATCmds[] = 
{
	  
		{"ATI\r\n","OK",200,NO_REC,3},																			 //��ѯģ����Ϣ	
		{"AT+CGREG?\r\n","OK",200,NO_REC,10},                                 //�鿴GPRSע��״̬
    {"AT+CGATT?\r\n","OK",200,NO_REC,10},                                 //�鿴GPRS����״̬
		
    {"AT+MCONFIG=GPRS,,\r\n","OK",200,NO_REC,3},                       		 //����client ID
		{"AT+MIPSTART=47.105.70.108,1883\r\n","CONNECT",3000,NO_REC,3},      //����IP�˿�
    {"AT+MCONNECT=1,500\r\n","CONNACK",500,NO_REC,2},                      //����MQTT�Ự

    {"AT+MSUB=/MY_PC,1\r\n","SUBACK",2000,NO_REC,2},                       //��������
    {"AT+MQTTMODE=0\r\n","OK",1000,NO_REC,2},                              //������Ϣ��ʽΪASCII
		{"AT+CSQ\r\n","+CSQ:",200,NO_REC,10},                                 //��ѯ�ź�
    {"AT+MPUB=/My_android,1,0","PUBACK",10000,NO_REC,2},                         //������Ϣ
    {"AT+MDISCONNECT\r\n","OK",3000,NO_REC,3},                             //�ر�MQTT����

    {"AT+MIPCLOSE\r\n","OK",200,NO_REC,3},                                 //�ر�TCP����
    
		{"AT+RESET\r\n","OK",3000,NO_REC,3},                                   //��λģ��
    
};

void ATSend(teATCmdNum ATCmdNum)
{
	char NBSendMsg[500];//���͵��ַ�����Ϣ
	memset(Usart2Type.Usart2RecBuff,0,USART2_REC_SIZE);//���2���ܻ�����
	memset(topic_msg,0,sizeof(topic_msg));
	ATCmds[ATCmdNum].ATStatus = NO_REC;//���ý���״̬Ϊδ����
	ATRecCmdNum = ATCmdNum;
	if(ATCmdNum == AT_MPUB)
	{
		RedBuff($GNGGA);
		ParseMessageBuf($GNGGA);
		heigth = atof(Message.Elevation);
		
		Red_Value(HUMIDITY, &humidity);
		Red_Value(TEMP, &temperature);
		memset(NBSendMsg, 0, sizeof(NBSendMsg));
		memset(NB_SendDataBuff, 0, sizeof(NB_SendDataBuff));
		//sprintf(NBSendMsg, "{%f,%f}", lat, lng);
		//sprintf(NBSendMsg,"{%s\\2C%.2f\\2C%.2f\\2C%f\\2C%f\\2C%f}",CSQ_buffer,temperature,humidity,lat,lng,heigth);
		sprintf(NBSendMsg,"{\\22CSQ\\22:%s\\2C\\22temp\\22:%.2f\\2C\\22hum\\22:%.2f\\2C\\22lat\\22:%.6f\\2C\\22lng\\22:%.6f\\2C\\22ele\\22:%.2f}",CSQ_buffer,temperature,humidity,Message.Latitude,Message.Longitude,heigth);
		//printf("##:SEND_buf:%s\r\n",NBSendMsg);
		
		sprintf(NB_SendDataBuff,"%s,%s\r\n",ATCmds[ATCmdNum].ATSendStr,NBSendMsg);
		MyUsartTransmit(NB_SendDataBuff, strlen(NB_SendDataBuff));
		//HAL_UART_Transmit_IT(&huart2, (uint8_t *)NB_SendDataBuff, strlen(NB_SendDataBuff));
		//while(HAL_UART_Transmit_IT(&huart2, (uint8_t *)NB_SendDataBuff, strlen(NB_SendDataBuff)) == HAL_OK);
		printf("##Buff:%s\r\n",NB_SendDataBuff);
		NBTaskStatus = NB_WAIT;
	}else
	{
		HAL_UART_Transmit(&huart2, (uint8_t *)ATCmds[ATCmdNum].ATSendStr, strlen(ATCmds[ATCmdNum].ATSendStr), 0xFF);
		printf("Send:%s\r\n", ATCmds[ATCmdNum].ATSendStr);
	}
	
	SetTime(&TimeNB, ATCmds[ATCmdNum].TimeOut);//ÿ����һ����ָ��ʹ򿪶�ʱ��
}
void ATRec(void)
{
	if(Usart2Type.Usart2RecFlag)
	{
		if(strstr((const char*)Usart2Type.Usart2RecBuff, ATCmds[ATRecCmdNum].ATRecStr) != NULL)//�жϽ��ܵ��ַ����Ƿ��Ԥ�ڽ��ܵ���ͬ
		{
			ATCmds[ATRecCmdNum].ATStatus = SUCCESS_REC;
		}
		HAL_UART_Transmit(&huart1,Usart2Type.Usart2RecBuff,Usart2Type.Usart2RecLen,0xFFFF);//����1��ӡ���ܵ�������
		Usart2Type.Usart2RecFlag = 0;//��ս��ܱ�־λ
		Usart2Type.Usart2RecLen = 0;//��ս��ܳ���
	}
	else if(Usart3Type.Usart3RecFlag)
	{
		//HAL_UART_Transmit(&huart1,Usart3Type.Usart3RecBuff,Usart3Type.Usart3RecLen,0xFFFF);//����1��ӡ���ܵ�������
		Usart3Type.Usart3RecFlag = 0;//��ս��ܱ�־λ
		Usart3Type.Usart3RecLen = 0;//��ս��ܳ���
		//printf("%s\r\n", "no data receive");
	}
}

void NB_Init(void)
{
		HAL_UART_Transmit(&huart1,(uint8_t *)"NB Init...\r\n",strlen("NB Init...\r\n"),0xFFFF);                 
    NBTaskStatus = NB_SEND;           //���뷢��ģʽ
    ATCmdNumCurrent = ATI;              //��ǰָ��
    ATCmdNumNext = (teATCmdNum)(ATCmdNumCurrent+1);        //��һ��ָ��
}

void NB_Task(void)
{
	while(1)
	{
		switch(NBTaskStatus)
		{
			case NB_IDIE://����״̬
				if(CompareTime(&TimeSend))
				{
					ATCmdNumCurrent = AT_CSQ;
					ATCmdNumNext = AT_IDIE;
					NBTaskStatus = NB_SEND;
					SetTime(&TimeSend, 5000);
					break;
				}
				return;
			case NB_SEND://����״̬
				if(ATCmdNumCurrent != ATCmdNumNext)//�����ǰָ�������һ��ָ������Ϊ�ǵ�һ��ִ��,����Ϊ�ظ�ִ��
				{
					CurrentRty = ATCmds[ATCmdNumCurrent].RtyNum;//��ȡ��ǰָ���趨���ط�����
				}
				ATSend(ATCmdNumCurrent); //��������
				NBTaskStatus = NB_WAIT;
				return;
			case NB_WAIT://�ȴ�����״̬
				ATRec();//��������
				DealData();
				return;
			case NB_ACCESS://�ɹ����ӷ�����
				ATCmdNumCurrent = AT_MPUB;
				ATCmdNumNext = AT_IDIE;
				NBTaskStatus = NB_SEND;
				SetTime(&TimeSend, 5000);
				return;
			default:
				return;
		}
	}
}

void DealData(void)
{
	if(ATCmds[ATCmdNumCurrent].ATStatus == SUCCESS_REC)//�ɹ����ݰ�
	{
		switch (ATCmdNumCurrent)
		{
			case AT_CSQ:
				memset(CSQ_buffer, 0, sizeof(CSQ_buffer));
				find_string_flag = Find_string((char *)Usart2Type.Usart2RecBuff, ": ", ",", CSQ_buffer);
				ATCmdNumCurrent = (teATCmdNum)(ATCmdNumCurrent+1);
				ATCmdNumNext = (teATCmdNum)(ATCmdNumCurrent+1);
				NBTaskStatus = NB_SEND;
				break;
			case AT_MIPSTART:
				ATCmdNumCurrent = AT_MCONNECT;
				ATCmdNumNext = (teATCmdNum)(ATCmdNumCurrent+1);
				NBTaskStatus = NB_SEND;
				break;
			case AT_MCONNECT:
				NBTaskStatus = NB_ACCESS;
				break;
			case AT_MPUB:
				NBTaskStatus = NB_IDIE;
				break;
			default:
				ATCmdNumCurrent = (teATCmdNum)(ATCmdNumCurrent+1);
				ATCmdNumNext = (teATCmdNum)(ATCmdNumCurrent+1);
				NBTaskStatus = NB_SEND;
		}
	}
	else if(CompareTime(&TimeNB))//��û�յ�Ԥ������  ���ҳ�ʱ
	{
		ATCmds[ATCmdNumCurrent].ATStatus = TIME_OUT;
		if(CurrentRty > 0)
		{
			CurrentRty--;
			ATCmdNumNext = ATCmdNumCurrent;
			NBTaskStatus = NB_SEND;
		}
		else
		{
			ATCmdNumCurrent = AT_RESET;
			NBTaskStatus = NB_SEND;
		}
	}
}
/*
����:��һ���ַ�����ȡ��ָ���ַ���
����:
	pcBuf:һ���ַ���ָ��
	left:��߱�ʶָ��
	right:�ұ߱�ʶָ��
	QpcRes:�洢ȡ�����ַ���
*/
int Find_string(char *pcBuf,char *left,char *right, char *pcRes)
{
    char *pcBegin = NULL;
    char *pcEnd = NULL;
    pcBegin = strstr(pcBuf, left);
    pcEnd = strstr(pcBegin+strlen(left), right);
    if(pcBegin == NULL || pcEnd == NULL || pcBegin > pcEnd)
    {
        HAL_UART_Transmit(&huart1,(uint8_t *)"string name not found!\r\n",strlen("string name not found!\r\n"),0xFFFF);
				return 0;
    }
    else
    {
        pcBegin += strlen(left);
        memcpy(pcRes, pcBegin, pcEnd-pcBegin);
				HAL_UART_Transmit(&huart1,(uint8_t *)"found string ok\r\n",strlen("string name not found!\r\n"),0xFFFF);
				return 1;
    }
		
}
