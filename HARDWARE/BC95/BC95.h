#include "usart.h"
void Clear_Buffer(void);//��ջ���
void BC95_Init(void);
void CDP_Init(void);
void BC95_SendCOAPdata(uint8_t len,uint8_t *data);
void BC95_RECCOAPData(void);
typedef struct
{
    uint8_t CSQ;
    uint8_t Socketnum;   //���
    uint8_t reclen;   //��ȡ�����ݵĳ���
    uint8_t res;
    uint8_t recdatalen[10];
    uint8_t recdata[100];
} BC95;
