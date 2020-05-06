#include "gps.h"

char RecBuff[255];
char LatBuf[100];
char LngBuf[100];

double FormatChange(char *Data_Char)
{
	double Data_Float = atof(Data_Char);
  int left = Data_Float/100;//整数部分
  double right = Data_Float - left*100;//剩余部分
  return left+right/60;
}

void RedBuff(teRedType RedType)
{
	char *Head;
  char *Tail;
  memset(RecBuff, 0, sizeof(RecBuff));
	memcpy(RecBuff, Usart3Type.Usart3RecBuff, Usart3Type.Usart3RecLen);	
	memset(Usart3Type.Usart3RecBuff, 0, USART3_REC_SIZE);
  switch(RedType)
  {
		case $GNGGA:
			Head = strstr(RecBuff, "$GNGGA");
			if(Head != NULL)
			{
				Tail = strstr(Head, "\r\n");
				if(Tail != NULL && Tail > Head)
				{
					memset(GPSMessageBuff.GNGGABuf, 0x00, sizeof(GPSMessageBuff.GNGGABuf));
					memcpy(GPSMessageBuff.GNGGABuf, Head, Tail - Head);
				}
			}
			break;
    case $GNRMC:
      Head = strstr(RecBuff, "$GNRMC");
      if(Head != NULL)
      {
				Tail = strstr(Head, "\r\n");
        if(Tail != NULL && Tail > Head)
        {
					memset(GPSMessageBuff.GNRMCBuf, 0x00, sizeof(GPSMessageBuff.GNRMCBuf));
          memcpy(GPSMessageBuff.GNRMCBuf, Head, Tail - Head);
        }
      }   
    break;
		default:
			break;
  }
}

void ParseMessageBuf(teRedType RedType)
{
    char *FirstString;
    char *NextString;
    switch(RedType)
    {
        case $GNGGA:
					for(int i = 0; i <= 9; i++)
          {
						if(i == 0)
            {
							FirstString = strstr(GPSMessageBuff.GNGGABuf, "$GNGGA");
              NextString = strstr(FirstString, ",");
            }
            else
            {
              NextString++;
              FirstString = NextString;
              NextString = strstr(FirstString, ",");
              switch(i)
              {
								case 1:
									memcpy(Message.UTCTime, FirstString, NextString - FirstString);
                  break;
                case 2:
									memset(LatBuf, 0, sizeof(LatBuf));
                  memcpy(LatBuf, FirstString, NextString - FirstString);
									Message.Latitude = FormatChange(LatBuf);
                  break;
                case 3:
                  memcpy(Message.N_S, FirstString, NextString - FirstString);
                  break;
                case 4:
									memset(LngBuf, 0, sizeof(LngBuf));
                  memcpy(LngBuf, FirstString, NextString - FirstString);
									Message.Longitude = FormatChange(LngBuf);
                  break;
                case 5:
                  memcpy(Message.E_W, FirstString, NextString - FirstString);
                  break;
                case 9:
                  memcpy(Message.Elevation, FirstString, NextString - FirstString);
                  break;
                default:
                  break;
              }
            }
          }
          break;
				case $GNRMC:
					break;
				default:
					break;
    }
}



