/**
 * @author Kai
 * @date 2025/11/20
 * encode: utf-8
 * 注意DHT11的数据引脚
 * * VCC: 3.3V-5V
 * * DATA: 数据引脚，用于DHT11与微控制器通信，在对应h文件中定义为P2^0
 * * GND: 0V
 * ---------------
 * 当然不全是这样的，有些只标了"S"和"-"，那么"S"表示数据引脚，"-"表示地线;
 * * 
 */
#include "dht11.h"
#include "intrins.h"


//DHT11初始化 
//返回0表示初始化成功，返回1表示失败
u8 DHT11_Init(void)
{
	DHT11_DQ=1;
	DHT11_Rst();	  
	return DHT11_Check();	
}

//复位DHT11
void DHT11_Rst(void)	   
{                 
	DHT11_DQ=1;
	delay_10us(1);
	DHT11_DQ=0; 	//拉低DQ
    delay_ms(25);   //保持至少18ms
    DHT11_DQ=1; 	//DQ=1 
	delay_10us(3);  //保持20~40us
}

//等待DHT11的响应
//返回1:未检测到DHT11的响应
//返回0:检测到响应
u8 DHT11_Check(void)	   
{   
	u8 retry=0;
	
	while (!DHT11_DQ&&retry<100)//判断是否接收到80us的低电平响应信号
	{
		retry++;
		_nop_();
	};
	if(retry>=100)return 1;
	else retry=0;
    while (DHT11_DQ&&retry<100)//判断是否接收到80us的高电平，之后进入数据传输状态
	{
		retry++;
		_nop_();
	};	 
	if(retry>=100)return 1;	    
	return 0;
}

//从DHT11读取一个字节
//返回值:读取到的数据
u8 DHT11_Read_Byte(void)    
{        
    u8 i,temp;
	u8 data_byte=0; 
	u8 retry=0;

  	for(i=0;i<8;i++)//读取8bit数据 
  	{ 
		while (!DHT11_DQ&&retry<50)//等待50us的低电平起始信号结束
		{
			retry++;
			_nop_();
		};
		retry=0; 
		delay_10us(3);//等待40us 
		temp=0;//时间为26us-28us时，表示接收到的是数据'0' 
		if(DHT11_DQ==1) 
			temp=1; //在26us-28us之后仍为高电平，则表示接收到的数据为'1' 
		while (DHT11_DQ&&retry<100)//等待数据信号高电平结束，'0'为26us-28us，'1'为70us
		{
			retry++;
			_nop_();
		};
		data_byte<<=1;//将接收到的数据向左移一位 
		data_byte|=temp; 
  	} 

  	return data_byte;
}

//从DHT11读取一次数据
//temp:温度值(范围:0~50℃)
//humi:湿度值(范围:20%~90%)
//返回值:0,成功;1,读取失败
u8 DHT11_Read_Data(u8 *temp,u8 *humi)    
{        
 	u8 buf[5];
	u8 i;
	DHT11_Rst();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//读取40位数据
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humi=buf[0];
			*temp=buf[2];
		}
		
	}else return 1;
	return 0;	    
}