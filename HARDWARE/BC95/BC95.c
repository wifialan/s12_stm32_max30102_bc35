///AT+NSOCL=0
#include "BC95.h"
#include "string.h"
#include "usart.h"
#include "delay.h"
char *strx,*extstrx;
extern char  Rx2Buffer[100],Rx2Counter;
BC95 BC95_Status;

unsigned char bc35_send_data[50];

void Clear_Buffer(void)//��ջ���
{
    Uart1_SendStr(Rx2Buffer);
    Rx2Counter=0;
    memset(Rx2Buffer,'\0',sizeof(Rx2Buffer));
}

void CDP_Init(void)//COAP���������ó�ʼ��
{
	Uart1_SendStr("Begin to init BC95...\n");
    printf("AT\r\n");
    delay_ms(300);
	Uart1_SendStr("Wait for BC95 response...\n");
    strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//����OK
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT\r\n");
        delay_ms(300);
		Uart1_SendStr("Wait for BC95 response...\n");
        strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//����OK
    }
    printf("AT+NBAND?\r\n");//��ȡƵ�κ�
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"+NBAND:5");//����5  ����  ����8���ƶ� �ƶ��Ŀͻ���ط�ע��һ���޸�
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT+NBAND?\r\n");//��ȡƵ�κ�
        delay_ms(300);
        strx=strstr((const char*)Rx2Buffer,(const char*)"+NBAND:5");//����OK
    }
    printf("AT+CIMI\r\n");//��ȡ���ţ������Ƿ���ڿ�����˼���Ƚ���Ҫ��
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"46011");//��46011�ǵ��ţ��ƶ���Ҫ����
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT+CIMI\r\n");//��ȡ���ţ������Ƿ���ڿ�����˼���Ƚ���Ҫ��
        delay_ms(300);
        strx=strstr((const char*)Rx2Buffer,(const char*)"46011");//��46011�ǵ��ţ��ƶ���Ҫ����
    }
    printf("AT+CFUN=0\r\n");//�ر���Ƶ
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//����OK
    while(strx==NULL)
    {
        strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//����OK
    }
    Clear_Buffer();
    printf("AT+NCDP=iot-coaps.cn-north-4.myhuaweicloud.com,5683\r\n");//����CDP��������ַ��Ϊ��ƽ̨�ģ����COAPЭ���IP��ַ�ǹ̶���
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//����OK
    while(strx==NULL)
    {
        strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//����OK
    }
    Clear_Buffer();
    printf("AT+NRB\r\n");//��ģ���������
    delay_ms(2000);	//��ʱ2s
    printf("AT+CIMI\r\n");//��ȡ���ţ�ȷ��ģ����������
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"460");//��460
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT+CIMI\r\n");//��ȡ���ţ�ȷ��ģ����������
        delay_ms(300);
        strx=strstr((const char*)Rx2Buffer,(const char*)"460");//����OK,˵�����Ǵ��ڵ�
    }
    ///����NCDP�ľ�ȷ�Ϻ���,�������������ñ�ȷ�Ϻ�.
}

void BC95_Init(void)
{
    memset(Rx2Buffer,'\0',sizeof(Rx2Buffer));
    Clear_Buffer();
    printf("AT\r\n");
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//����OK
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT\r\n");
        delay_ms(300);
        strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//����OK
    }
    printf("AT+CMEE=1\r\n"); //�������ֵ
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//����OK
    Clear_Buffer();
    printf("AT+NBAND?\r\n");//��ȡƵ�κ�
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"+NBAND:5");//����5  ����  �ƶ��Ƿ���8
    Clear_Buffer();
    printf("AT+CIMI\r\n");//��ȡ���ţ������Ƿ���ڿ�����˼���Ƚ���Ҫ��
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"460");//��460
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT+CIMI\r\n");//��ȡ���ţ������Ƿ���ڿ�����˼���Ƚ���Ҫ��
        delay_ms(300);
        strx=strstr((const char*)Rx2Buffer,(const char*)"460");//����OK,˵�����Ǵ��ڵ�
    }
    printf("AT+CGATT=1\r\n");//�������磬PDP
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//��OK
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT+CGATT=1\r\n");//
        delay_ms(100);
        strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//
    }
    Uart1_SendStr("��������...�ɹ�\r\n");
    printf("AT+CGATT?\r\n");//��ѯ����״̬
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"+CGATT:1");//��1 ��������ɹ� ��ȡ��IP��ַ��
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT+CGATT?\r\n");//��ȡ����״̬
        delay_ms(3000);
        Uart1_SendStr("wait for connect net...\n");

        strx=strstr((const char*)Rx2Buffer,(const char*)"+CGATT:1");//����1,����ע���ɹ�
    }
    Uart1_SendStr("��ѯ��������״̬...�ɹ�\r\n");
    printf("AT+CSQ\r\n");//�鿴��ȡCSQֵ
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"+CSQ");//����CSQ
    Uart1_SendStr("\r\n");
    Uart1_SendStr(strx);
    Uart1_SendStr("\r\n");
    if(strx)
    {
        BC95_Status.CSQ=(strx[5]-0x30)*10+(strx[6]-0x30);//��ȡCSQ
        if(BC95_Status.CSQ==99)//˵��ɨ��ʧ��
        {
            while(1)
            {
                Uart1_SendStr("�ź�����ʧ�ܣ���鿴ԭ��!\r\n");
                delay_ms(300);
                BC95_Init();
            }
        }
    }
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT+CSQ\r\n");//�鿴��ȡCSQֵ
        delay_ms(300);
        strx=strstr((const char*)Rx2Buffer,(const char*)"+CSQ");//
        if(strx)
        {
            BC95_Status.CSQ=(strx[5]-0x30)*10+(strx[6]-0x30);//��ȡCSQ
            if(BC95_Status.CSQ==99)//˵��ɨ��ʧ��
            {
                while(1)
                {
                    Uart1_SendStr("�ź�����ʧ�ܣ���鿴ԭ��!\r\n");
                    delay_ms(300);
                }
            }
        }
    }
    Clear_Buffer();
    printf("AT+CEREG?\r\n");
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"+CEREG:0,1");//����ע��״̬
    extstrx=strstr((const char*)Rx2Buffer,(const char*)"+CEREG:1,1");//����ע��״̬
    Clear_Buffer();
    while(strx==NULL&&extstrx==NULL)
    {
        Clear_Buffer();
        printf("AT+CEREG?\r\n");//�ж���Ӫ��
        delay_ms(300);
        strx=strstr((const char*)Rx2Buffer,(const char*)"+CEREG:0,1");//����ע��״̬
        extstrx=strstr((const char*)Rx2Buffer,(const char*)"+CEREG:1,1");//����ע��״̬
    }
    printf("AT+CEREG=1\r\n");
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//����OK
    Clear_Buffer();
    while(strx==NULL&&extstrx==NULL)
    {
        Clear_Buffer();
        printf("AT+CEREG=1\r\n");//�ж���Ӫ��
        delay_ms(300);
        strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//����OK
    }
    Uart1_SendStr("��ʼ��BC95�ɹ�\r\n");
}

void BC95_SendCOAPdata(uint8_t len,uint8_t *data)
{
    printf("AT+NMGS=%d,%s\r\n",len,data);//����COAP����
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//����OK
    if(strx)//����������ȷ��ƽ̨�յ�����
    {
        Uart1_SendStr("SEND DATA OK!\r\n");//���ô���1��ӡ���ͳɹ���ʾ
        Clear_Buffer();
    }
    else//�������error һ���ǵ�һ����ƽ̨�������������CDP�������������⡣�û����Բ�ѯ�� AT+NMSTATUS? ��ӡ����ʾ�˽��в鿴
    {
        printf("AT+NMSTATUS?\r\n");//�жϵ�ǰģ����ƽ̨֮������ӹ�ϵ��һ��δ������֮ǰ����INIITIALISED������������͵����ݵ�ƽ̨֮����MO_DATA_ENABLED
        delay_ms(300);
        strx=strstr((const char*)Rx2Buffer,(const char*)"+NMSTATUS:MO_DATA_ENABLED");//�鿴�Ƿ񷵻���������ݣ�
        if(strx)
        {
            Uart1_SendStr("Connect OK!\r\n");  //���������������ģ����´��ٷ����ݾͿ�����
        }
        Clear_Buffer();	//��ӡƽ̨�ķ���ֵ
    }
    Clear_Buffer();

}
//����ƽ̨�˶��豸���ݵ��·�
void BC95_RECCOAPData(void)
{
    char i;
	strx = NULL;
	delay_ms(10);
    strx=strstr((const char*)Rx2Buffer,(const char*)"+NNMI:");//����+NSONMI:���������յ�UDP���������ص�����
    if(strx)
    {
        Clear_Buffer();
        for(i=0; i<100; i++)
            Rx2Buffer[i]=0;
    }
}
