#ifndef _GPS_H
#define _GPS_H
#include "usart.h"
#include <string.h>
#include <stdlib.h>

typedef struct
{
    char GNGGABuf[255];
    char GNRMCBuf[255];
}tsMessageBuff;//保存协议帧内容信息

typedef struct
{
    char UTCTime[11];//时间
    double Latitude;//纬度
    char N_S[2];//纬度方向
    double Longitude;//经度
    char E_W[2];//经度方向
    char Elevation[5];//相对于海平面高度
}tsMessage;//保存解析后协议数据

typedef enum
{
    $GNGGA,
    $GNRMC,
}teRedType;//指定解析的帧类型



extern tsMessage Message;//解析后内容
extern tsMessageBuff GPSMessageBuff;//帧内容


double FormatChange(char *Data_Char);
void RedBuff(teRedType RedType);
void ParseMessageBuf(teRedType RedType);
#endif

