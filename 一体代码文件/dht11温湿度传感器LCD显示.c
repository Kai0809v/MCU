/**
 * @author Kai
 * @date 2025/11/20
 * encode: utf-8
*/
/**
 * 注意DHT11的数据引脚
 * * VCC: 3.3V-5V
 * * DATA: 数据引脚，用于DHT11与微控制器通信，在对应h文件中定义为P2^0
 * * GND: 0V
 * ---------------
 * 当然不全是这样的，有些只标了"S"和"-"，那么"S"表示数据引脚，"-"表示地线
 * * 
 */
#include <reg52.h>
#include "intrins.h"
typedef unsigned int u16;
typedef unsigned char u8;

//*****************************延时 delay time */
void delay_10us(u16 ten_us)
{
	while(ten_us--);	
}
void delay_ms(u16 ms)
{
	u16 i,j;
	for(i=ms;i>0;i--)
		for(j=110;j>0;j--);
}


//*****************************DHT11
sbit DHT11_DQ=P2^0;
//返回0表示初始化成功，返回1表示失败
void DHT11_Rst(void)	   
{                 
	DHT11_DQ=1;
	delay_10us(1);
	DHT11_DQ=0; 	//拉低DQ
    delay_ms(25);   //保持至少18ms
    DHT11_DQ=1; 	//DQ=1 
	delay_10us(3);  //保持20~40us
}

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

u8 DHT11_Init(void)
{
	DHT11_DQ=1;
	DHT11_Rst();	  
	return DHT11_Check();	
}


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


//*****************************LCD
//LCD1602数据口4位或8位模式，0为1602的8位数据口模式，1为4位
#define LCD1602_4OR8_DATA_INTERFACE	0	//默认使用8位数据口LCD1602

//引脚定义
sbit LCD1602_RS=P2^6;//寄存器选择
sbit LCD1602_RW=P2^5;//读写选择
sbit LCD1602_E=P2^7; //使能信号
#define LCD1602_DATAPORT P0	//定义LCD1602数据端口

#if (LCD1602_4OR8_DATA_INTERFACE==0)//8位LCD
void lcd1602_write_cmd(u8 cmd)
{
	LCD1602_RS=0;//选择命令
	LCD1602_RW=0;//选择写
	LCD1602_E=0;
	LCD1602_DATAPORT=cmd;//准备命令
	delay_ms(1);
	LCD1602_E=1;//使能端E高电平写入
	delay_ms(1);
	LCD1602_E=0;//使能端E低电平完成写入	
}
#else	//4位LCD
void lcd1602_write_cmd(u8 cmd)
{
	LCD1602_RS=0;//选择命令
	LCD1602_RW=0;//选择写
	LCD1602_E=0;
	LCD1602_DATAPORT=cmd;//准备命令
	delay_ms(1);
	LCD1602_E=1;//使能端E高电平写入
	delay_ms(1);
	LCD1602_E=0;//使能端E低电平完成写入
	
	LCD1602_DATAPORT=cmd<<4;//准备命令
	delay_ms(1);
	LCD1602_E=1;//使能端E高电平写入
	delay_ms(1);
	LCD1602_E=0;//使能端E低电平完成写入	
}
#endif

/*******************************************************************************
* 函数名       : lcd1602_write_data
* 函数功能		 : LCD1602写数据
* 输入         : dat：数据
* 输出       	 : 无
*******************************************************************************/
#if (LCD1602_4OR8_DATA_INTERFACE==0)//8位LCD
void lcd1602_write_data(u8 dat) 
{
	LCD1602_RS=1;//选择数据
	LCD1602_RW=0;//选择写
	LCD1602_E=0;
	LCD1602_DATAPORT=dat;//准备数据
	delay_ms(1);
	LCD1602_E=1;//使能端E高电平写入
	delay_ms(1);
	LCD1602_E=0;//使能端E低电平完成写入		
}
#else
void lcd1602_write_data(u8 dat) 
{
	LCD1602_RS=1;//选择数据
	LCD1602_RW=0;//选择写
	LCD1602_E=0;
	LCD1602_DATAPORT=dat;//准备数据
	delay_ms(1);
	LCD1602_E=1;//使能端E高电平写入
	delay_ms(1);
	LCD1602_E=0;//使能端E低电平完成写入
	
	LCD1602_DATAPORT=dat<<4;//准备数据
	delay_ms(1);
	LCD1602_E=1;//使能端E高电平写入
	delay_ms(1);
	LCD1602_E=0;//使能端E低电平完成写入		
}
#endif

/*******************************************************************************
* 函数名       : lcd1602_init
* 函数功能		 : LCD1602初始化
* 输入         : 无
* 输出       	 : 无
*******************************************************************************/
#if (LCD1602_4OR8_DATA_INTERFACE==0)//8位LCD
void lcd1602_init(void)
{
	lcd1602_write_cmd(0x38);//设置显示8位数据，显示2行，5*7点阵/字符
	lcd1602_write_cmd(0x0c);//显示开，光标关，光标不闪烁
	lcd1602_write_cmd(0x06);//写数据后光标右移，显示不移动
	lcd1602_write_cmd(0x01);//清屏	
}
#else
void lcd1602_init(void)
{
	lcd1602_write_cmd(0x28);//设置显示4位数据，显示2行，5*7点阵/字符
	lcd1602_write_cmd(0x0c);//显示开，光标关，光标不闪烁
	lcd1602_write_cmd(0x06);//写数据后光标右移，显示不移动
	lcd1602_write_cmd(0x01);//清屏	
}
#endif

/*******************************************************************************
* 函数名       : lcd1602_clear
* 函数功能		 : LCD1602清屏
* 输入         : 无
* 输出       	 : 无
*******************************************************************************/
void lcd1602_clear(void)
{
	lcd1602_write_cmd(0x01);	
}

/*******************************************************************************
* 函数名       : lcd1602_show_string
* 函数功能		 : LCD1602显示字符串
* 输入         : x,y：显示位置，x=0~15，y=0~1;
				   str：显示字符串
* 输出       	 : 无
*******************************************************************************/
void lcd1602_show_string(u8 x,u8 y,u8 *str)
{
	u8 i=0;

	if(y>1||x>15)return;//超出范围强制退出

	if(y<1)	//第1行显示
	{	
		while(*str!='\0')//字符串以'\0'为结束，只要前面有数据就显示
		{
			if(i<16-x)//如果字符串长度在第一行显示范围内，在第一行显示
			{
				lcd1602_write_cmd(0x80+i+x);//第一行显示地址设置	
			}
			else
			{
				lcd1602_write_cmd(0x40+0x80+i+x-16);//第二行显示地址设置	
			}
			lcd1602_write_data(*str);//显示数据
			str++;//指针后移
			i++;	
		}	
	}
	else	//第2行显示
	{
		while(*str!='\0')
		{
			if(i<16-x) //如果字符串长度在第二行显示范围内，在第二行显示
			{
				lcd1602_write_cmd(0x80+0x40+i+x);	
			}
			else
			{
				lcd1602_write_cmd(0x80+i+x-16);	
			}
			lcd1602_write_data(*str);
			str++;
			i++;	
		}	
	}				
}
void main()
{	
	u8 temp=0,humi=0;
	u8 i=0;
	u8 temp_buf[3],humi_buf[3];

	lcd1602_init();
	while(DHT11_Init())	//检测DHT11是否存在
	{
		lcd1602_show_string(0,0,"Error");		
	}
	lcd1602_show_string(0,0,"Temp:   C");
	lcd1602_show_string(0,1,"Humi:   %RH");
	while(1)
	{
		i++;
		if(i%200==0)
		{
			DHT11_Read_Data(&temp,&humi);
			temp_buf[0]=temp/10+0x30;	
			temp_buf[1]=temp%10+0x30;
			temp_buf[2]='\0';
			lcd1602_show_string(6,0,temp_buf);

			humi_buf[0]=humi/10+0x30;	
			humi_buf[1]=humi%10+0x30;
			humi_buf[2]='\0';
			lcd1602_show_string(6,1,humi_buf);	
		}
			
		delay_ms(1);		
	}
}
