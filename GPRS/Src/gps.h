#ifndef _GPS_H
#define _GPS_H
#include "usart.h"
#include <string.h>
#include <stdlib.h>

typedef struct
{
    char GNGGABuf[255];
    char GNRMCBuf[255];
}tsMessageBuff;//����Э��֡������Ϣ

typedef struct
{
    char UTCTime[11];//ʱ��
    double Latitude;//γ��
    char N_S[2];//γ�ȷ���
    double Longitude;//����
    char E_W[2];//���ȷ���
    char Elevation[5];//����ں�ƽ��߶�
}tsMessage;//���������Э������

typedef enum
{
    $GNGGA,
    $GNRMC,
}teRedType;//ָ��������֡����



extern tsMessage Message;//����������
extern tsMessageBuff GPSMessageBuff;//֡����


double FormatChange(char *Data_Char);
void RedBuff(teRedType RedType);
void ParseMessageBuf(teRedType RedType);
#endif

