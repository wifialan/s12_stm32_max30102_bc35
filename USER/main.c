/*************************************************************************************
STM32F103-30102:
	VCC<->3.3V
	GND<->GND
	SCL<->PB10
	SDA<->PB11
	IM<->PB9
0.96inch OLED :
	VCC<->3.3V
	GND<->GND
	SCL<->PA5
	SDA<->PA6
	RST<->PA3
	DC<->PA4
	CS<->PA2
USB-TTL:
	5V<->5V
	GND<->GND
	RXD<->PA9
	TXD<->PA10
**************************************************************************************/
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "max30102.h"
#include "myiic.h"
#include "algorithm.h"
#include "OLED_I2C.h"
#include "BC95.h"
#include "string.h"

uint32_t aun_ir_buffer[500]; //IR LED sensor data
int32_t n_ir_buffer_length;    //data length
uint32_t aun_red_buffer[500];    //Red LED sensor data
int32_t n_sp02; //SPO2 value
int8_t ch_spo2_valid;   //indicator to show if the SP02 calculation is valid
int32_t n_heart_rate;   //heart rate value
int8_t  ch_hr_valid;    //indicator to show if the heart rate calculation is valid
uint8_t uch_dummy;

#define MAX_BRIGHTNESS 255

extern unsigned char bc35_send_data[20];

int main(void)
{
    //variables to calculate the on-board LED brightness that reflects the heartbeats
    uint32_t un_min, un_max, un_prev_data;
    int i;
    int32_t n_brightness;
    float f_temp;
    u8 temp[6];
    u8 str[100];
    u8 dis_hr=0,dis_spo2=0;

    NVIC_Configuration();
    delay_init();	    	 //延时函数初始化
    uart_init(115200);	 	//串口1初始化为115200
	usart2_init(9600);	 	//串口2初始化为115200

    LED_Init();
	
	CDP_Init();
	BC95_Init();
	
	Uart1_SendStr("\r\n MAX30102  init  \r\n");

    //OLED
	I2C_Configuration();
	OLED_Init();
	OLED_CLS();
	OLED_ShowCN(0,0,0);		//心
	OLED_ShowCN(16*1,0,1);	//率
	OLED_ShowCN(16*2,0,2);	//监
	OLED_ShowCN(16*3,0,3);	//测
	OLED_ShowCN(16*4,0,4);	//装
	OLED_ShowCN(16*5,0,5);	//置
	
	OLED_ShowCN(0,2,0);		//心
	OLED_ShowCN(16*1,2,1);	//率
	
	OLED_ShowCN(0,4,6);		//血
	OLED_ShowCN(16*1,4,7);	//氧
	
//	OLED_ShowStr(0,0,"Heart Rate",2);
	//OLED_ShowCHinese(0,0,0);//中
//	OLED_Refresh_Gram();//更新显示到OLED

	
    max30102_init();

    Uart1_SendStr("\r\n MAX30102  init  \r\n");
	
//	while(1)
//	{
//		delay_ms(200);
//		max30102_Bus_Write(0x21, 0x01); //SET   TEMP_EN
//		temp_num = max30102_Bus_Read(0x1f);
//		printf("当前温度 = %d\r\n",temp_num);
//	}

    un_min=0x3FFFF;
	un_max=0;
	
	n_ir_buffer_length=500; //buffer length of 100 stores 5 seconds of samples running at 100sps
	//read the first 500 samples, and determine the signal range
    for(i=0;i<n_ir_buffer_length;i++)
    {
        while(MAX30102_INT==1);   //wait until the interrupt pin asserts
        
		max30102_FIFO_ReadBytes(REG_FIFO_DATA,temp);
		aun_red_buffer[i] =  (long)((long)((long)temp[0]&0x03)<<16) | (long)temp[1]<<8 | (long)temp[2];    // Combine values to get the actual number
		aun_ir_buffer[i] = (long)((long)((long)temp[3] & 0x03)<<16) |(long)temp[4]<<8 | (long)temp[5];   // Combine values to get the actual number
            
        if(un_min>aun_red_buffer[i])
            un_min=aun_red_buffer[i];    //update signal min
        if(un_max<aun_red_buffer[i])
            un_max=aun_red_buffer[i];    //update signal max
    }
	un_prev_data=aun_red_buffer[i];
	//calculate heart rate and SpO2 after first 500 samples (first 5 seconds of samples)
    maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid); 
	
	while(1)
	{
		i=0;
        un_min=0x3FFFF;
        un_max=0;
		
		//dumping the first 100 sets of samples in the memory and shift the last 400 sets of samples to the top
        for(i=100;i<500;i++)
        {
            aun_red_buffer[i-100]=aun_red_buffer[i];
            aun_ir_buffer[i-100]=aun_ir_buffer[i];
            
            //update the signal min and max
            if(un_min>aun_red_buffer[i])
            un_min=aun_red_buffer[i];
            if(un_max<aun_red_buffer[i])
            un_max=aun_red_buffer[i];
        }
		//take 100 sets of samples before calculating the heart rate.
        for(i=400;i<500;i++)
        {
            un_prev_data=aun_red_buffer[i-1];
            while(MAX30102_INT==1);
            max30102_FIFO_ReadBytes(REG_FIFO_DATA,temp);
			aun_red_buffer[i] =  (long)((long)((long)temp[0]&0x03)<<16) | (long)temp[1]<<8 | (long)temp[2];    // Combine values to get the actual number
			aun_ir_buffer[i] = (long)((long)((long)temp[3] & 0x03)<<16) |(long)temp[4]<<8 | (long)temp[5];   // Combine values to get the actual number
        
            if(aun_red_buffer[i]>un_prev_data)
            {
                f_temp=aun_red_buffer[i]-un_prev_data;
                f_temp/=(un_max-un_min);
                f_temp*=MAX_BRIGHTNESS;
                n_brightness-=(int)f_temp;
                if(n_brightness<0)
                    n_brightness=0;
            }
            else
            {
                f_temp=un_prev_data-aun_red_buffer[i];
                f_temp/=(un_max-un_min);
                f_temp*=MAX_BRIGHTNESS;
                n_brightness+=(int)f_temp;
                if(n_brightness>MAX_BRIGHTNESS)
                    n_brightness=MAX_BRIGHTNESS;
            }
			//send samples and calculation result to terminal program through UART
			if(ch_hr_valid == 1 && n_heart_rate<120)//**/ ch_hr_valid == 1 && ch_spo2_valid ==1 && n_heart_rate<120 && n_sp02<101
			{
				dis_hr = n_heart_rate;
				dis_spo2 = n_sp02;
			}
			else
			{
				dis_hr = 0;
				dis_spo2 = 0;
			}
//			printf("red_buffer: %ld ",aun_red_buffer[i]);
//			printf("ir_buffer: %ld ",aun_ir_buffer[i]);
//			printf("HR=%i, ", n_heart_rate); 
//			printf("HRvalid=%i, ", ch_hr_valid);
//			printf("SpO2=%i, ", n_sp02);
//			printf("SPO2Valid=%i\r\n", ch_spo2_valid);
		}
        maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);
		
		//显示刷新
		LED0=0;
		
		if(dis_hr == 0 || dis_hr > 120)  //**dis_hr == 0 && dis_spo2 == 0
		{
			memset(str,'\0',sizeof(str));
			sprintf((char *)str,":%-3d",0);//**HR:%3d SpO2:%3d 
			OLED_ShowStr(16*2,2,str,2);
			sprintf((char *)bc35_send_data,"%02x",0);
		}else{
			memset(str,'\0',sizeof(str));
			sprintf((char *)str,":%-3d",dis_hr);//**HR:%3d SpO2:%3d 
			OLED_ShowStr(16*2,2,str,2);
			sprintf((char *)bc35_send_data,"%02x",dis_hr);
			delay_ms(100);
		}
		if(dis_spo2 == 0)  //**dis_hr == 0 && dis_spo2 == 0
		{
			memset(str,'\0',sizeof(str));
			sprintf((char *)str,":%-3d",0);//**HR:%3d SpO2:%3d 
			OLED_ShowStr(16*2,4,str,2);
			sprintf((char *)bc35_send_data+2,"%02x",0);
		}else{
			memset(str,'\0',sizeof(str));
			sprintf((char *)str,":%-3d",dis_spo2);//**HR:%3d SpO2:%3d 
			OLED_ShowStr(16*2,4,str,2);
			sprintf((char *)bc35_send_data+2,"%02x",dis_spo2);
		}
		
		if(!(dis_hr == 0 || dis_hr > 120))  //**dis_hr == 0 && dis_spo2 == 0
		{
			Uart1_SendStr("Send data: ");
			Uart1_SendStr((char*)bc35_send_data);
			Uart1_SendStr("\r\n");
			BC95_SendCOAPdata(2,bc35_send_data);
		}
		
		
		//OLED_ShowString(0,0,str,16);
		//OLED_Fill(0,23,127,63,0);
		//红光在上，红外在下
		//dis_DrawCurve(aun_red_buffer,20);
		//dis_DrawCurve(aun_ir_buffer,0);
		//OLED_Refresh_Gram();//更新显示到//OLED	 
	}
}

