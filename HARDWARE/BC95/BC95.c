///AT+NSOCL=0
#include "BC95.h"
#include "string.h"
#include "usart.h"
#include "delay.h"
char *strx,*extstrx;
extern char  Rx2Buffer[100],Rx2Counter;
BC95 BC95_Status;

unsigned char bc35_send_data[50];

void Clear_Buffer(void)//清空缓存
{
    Uart1_SendStr(Rx2Buffer);
    Rx2Counter=0;
    memset(Rx2Buffer,'\0',sizeof(Rx2Buffer));
}

void CDP_Init(void)//COAP服务器配置初始化
{
	Uart1_SendStr("Begin to init BC95...\n");
    printf("AT\r\n");
    delay_ms(300);
	Uart1_SendStr("Wait for BC95 response...\n");
    strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//返回OK
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT\r\n");
        delay_ms(300);
		Uart1_SendStr("Wait for BC95 response...\n");
        strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//返回OK
    }
    printf("AT+NBAND?\r\n");//获取频段号
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"+NBAND:5");//返回5  电信  返回8是移动 移动的客户这地方注意一下修改
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT+NBAND?\r\n");//获取频段号
        delay_ms(300);
        strx=strstr((const char*)Rx2Buffer,(const char*)"+NBAND:5");//返回OK
    }
    printf("AT+CIMI\r\n");//获取卡号，类似是否存在卡的意思，比较重要。
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"46011");//返46011是电信，移动的要更改
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT+CIMI\r\n");//获取卡号，类似是否存在卡的意思，比较重要。
        delay_ms(300);
        strx=strstr((const char*)Rx2Buffer,(const char*)"46011");//返46011是电信，移动的要更改
    }
    printf("AT+CFUN=0\r\n");//关闭射频
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//返回OK
    while(strx==NULL)
    {
        strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//返回OK
    }
    Clear_Buffer();
    printf("AT+NCDP=iot-coaps.cn-north-4.myhuaweicloud.com,5683\r\n");//配置CDP服务器地址华为云平台的，这个COAP协议的IP地址是固定的
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//返回OK
    while(strx==NULL)
    {
        strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//返回OK
    }
    Clear_Buffer();
    printf("AT+NRB\r\n");//对模块进行重启
    delay_ms(2000);	//延时2s
    printf("AT+CIMI\r\n");//获取卡号，确认模块重启正常
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"460");//返460
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT+CIMI\r\n");//获取卡号，确认模块重启正常
        delay_ms(300);
        strx=strstr((const char*)Rx2Buffer,(const char*)"460");//返回OK,说明卡是存在的
    }
    ///这样NCDP的就确认好了,服务器连接设置被确认好.
}

void BC95_Init(void)
{
    memset(Rx2Buffer,'\0',sizeof(Rx2Buffer));
    Clear_Buffer();
    printf("AT\r\n");
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//返回OK
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT\r\n");
        delay_ms(300);
        strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//返回OK
    }
    printf("AT+CMEE=1\r\n"); //允许错误值
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//返回OK
    Clear_Buffer();
    printf("AT+NBAND?\r\n");//获取频段号
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"+NBAND:5");//返回5  电信  移动是返回8
    Clear_Buffer();
    printf("AT+CIMI\r\n");//获取卡号，类似是否存在卡的意思，比较重要。
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"460");//返460
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT+CIMI\r\n");//获取卡号，类似是否存在卡的意思，比较重要。
        delay_ms(300);
        strx=strstr((const char*)Rx2Buffer,(const char*)"460");//返回OK,说明卡是存在的
    }
    printf("AT+CGATT=1\r\n");//激活网络，PDP
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//返OK
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT+CGATT=1\r\n");//
        delay_ms(100);
        strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//
    }
    Uart1_SendStr("激活网络...成功\r\n");
    printf("AT+CGATT?\r\n");//查询激活状态
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"+CGATT:1");//返1 表明激活成功 获取到IP地址了
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT+CGATT?\r\n");//获取激活状态
        delay_ms(3000);
        Uart1_SendStr("wait for connect net...\n");

        strx=strstr((const char*)Rx2Buffer,(const char*)"+CGATT:1");//返回1,表明注网成功
    }
    Uart1_SendStr("查询激活网络状态...成功\r\n");
    printf("AT+CSQ\r\n");//查看获取CSQ值
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"+CSQ");//返回CSQ
    Uart1_SendStr("\r\n");
    Uart1_SendStr(strx);
    Uart1_SendStr("\r\n");
    if(strx)
    {
        BC95_Status.CSQ=(strx[5]-0x30)*10+(strx[6]-0x30);//获取CSQ
        if(BC95_Status.CSQ==99)//说明扫网失败
        {
            while(1)
            {
                Uart1_SendStr("信号搜索失败，请查看原因!\r\n");
                delay_ms(300);
                BC95_Init();
            }
        }
    }
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT+CSQ\r\n");//查看获取CSQ值
        delay_ms(300);
        strx=strstr((const char*)Rx2Buffer,(const char*)"+CSQ");//
        if(strx)
        {
            BC95_Status.CSQ=(strx[5]-0x30)*10+(strx[6]-0x30);//获取CSQ
            if(BC95_Status.CSQ==99)//说明扫网失败
            {
                while(1)
                {
                    Uart1_SendStr("信号搜索失败，请查看原因!\r\n");
                    delay_ms(300);
                }
            }
        }
    }
    Clear_Buffer();
    printf("AT+CEREG?\r\n");
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"+CEREG:0,1");//返回注册状态
    extstrx=strstr((const char*)Rx2Buffer,(const char*)"+CEREG:1,1");//返回注册状态
    Clear_Buffer();
    while(strx==NULL&&extstrx==NULL)
    {
        Clear_Buffer();
        printf("AT+CEREG?\r\n");//判断运营商
        delay_ms(300);
        strx=strstr((const char*)Rx2Buffer,(const char*)"+CEREG:0,1");//返回注册状态
        extstrx=strstr((const char*)Rx2Buffer,(const char*)"+CEREG:1,1");//返回注册状态
    }
    printf("AT+CEREG=1\r\n");
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//返回OK
    Clear_Buffer();
    while(strx==NULL&&extstrx==NULL)
    {
        Clear_Buffer();
        printf("AT+CEREG=1\r\n");//判断运营商
        delay_ms(300);
        strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//返回OK
    }
    Uart1_SendStr("初始化BC95成功\r\n");
}

void BC95_SendCOAPdata(uint8_t len,uint8_t *data)
{
    printf("AT+NMGS=%d,%s\r\n",len,data);//发送COAP数据
    delay_ms(300);
    strx=strstr((const char*)Rx2Buffer,(const char*)"OK");//返回OK
    if(strx)//表明发送正确，平台收到数据
    {
        Uart1_SendStr("SEND DATA OK!\r\n");//就让串口1打印发送成功提示
        Clear_Buffer();
    }
    else//如果返回error 一般是第一次与平台握手问题或者是CDP服务器配置问题。用户可以查询下 AT+NMSTATUS? 打印到显示端进行查看
    {
        printf("AT+NMSTATUS?\r\n");//判断当前模块与平台之间的连接关系，一般未发数据之前都是INIITIALISED，如果正常发送到数据到平台之后是MO_DATA_ENABLED
        delay_ms(300);
        strx=strstr((const char*)Rx2Buffer,(const char*)"+NMSTATUS:MO_DATA_ENABLED");//查看是否返回是这个数据，
        if(strx)
        {
            Uart1_SendStr("Connect OK!\r\n");  //表明连接上正常的，待下次再发数据就可以了
        }
        Clear_Buffer();	//打印平台的返回值
    }
    Clear_Buffer();

}
//接收平台端对设备数据的下发
void BC95_RECCOAPData(void)
{
    char i;
	strx = NULL;
	delay_ms(10);
    strx=strstr((const char*)Rx2Buffer,(const char*)"+NNMI:");//返回+NSONMI:，表明接收到UDP服务器发回的数据
    if(strx)
    {
        Clear_Buffer();
        for(i=0; i<100; i++)
            Rx2Buffer[i]=0;
    }
}
