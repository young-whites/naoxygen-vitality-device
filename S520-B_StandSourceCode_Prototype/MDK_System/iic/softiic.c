#include "softiic.h"



#if TEST_SOFT_IIC

/********************************************************************
* Function Name  : Soft_IIC_Start.
* Description    : iic启动:当SCL处于高电平状态时，SDA出现一个下降沿,即产生IIC启动信号
* Input          : None.
* Output         : None.
* Return         : None.
********************************************************************/
void _IIC_Start(void)
{
	_SDA_OUT();     //sda线输出
	_IIC_SDA(1);	  	  
	_IIC_SCL(1);
	delay_us(4);
 	_IIC_SDA(0);//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	_IIC_SCL(0);//钳住I2C总线，准备发送或接收数据 
}	  





/********************************************************************
* Function Name  : Soft_IIC_Stop.
* Description    : iic停止:当SCL处于高电平状态时，SDA出现一个上升沿,即产生IIC停止信号
* Input          : None.
* Output         : None.
* Return         : None.
********************************************************************/
void _IIC_Stop(void)
{
	_SDA_OUT();//sda线输出
	_IIC_SCL(0);
	_SDA_OUT();//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	_IIC_SCL(1); 
	_SDA_OUT();//发送I2C总线结束信号
	delay_us(4);							   	
}



/********************************************************************
* Function Name  : _IIC_Wait_Ack.
* Description    : 应答状态，0表示应答，1表示设备应答失败
* Input          : None.
* Output         : None.
* Return         : None.
********************************************************************/
uint8_t _IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	_SDA_IN();      //SDA设置为输入  
	_IIC_SDA(1);delay_us(1);	   
	_IIC_SCL(1);delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			_IIC_Stop();
			return 1;
		}
	}
	_IIC_SCL(0);//时钟输出0 	   
	return 0;  
} 




/********************************************************************
* Function Name  : _IIC_Ack.
* Description    : 主机（主控制器）产生应答信号
* Input          : None.
* Output         : None.
* Return         : None.
********************************************************************/
void _IIC_Ack(void)
{
	_IIC_SCL(0);
	_SDA_OUT();
	_IIC_SDA(0);
	delay_us(1);
	_IIC_SCL(1);
	delay_us(1);
	_IIC_SCL(0);
}






/********************************************************************
* Function Name  : _IIC_NAck.
* Description    : 主机（主控制器）不产生应答信号
* Input          : None.
* Output         : None.
* Return         : None.
********************************************************************/	    
void _IIC_NAck(void)
{
	_IIC_SCL(0);
	_SDA_OUT();
	_IIC_SCL(1);
	delay_us(1);
	_IIC_SDA(1);
	delay_us(1);
	_IIC_SCL(0);
}	




/********************************************************************
* Function Name  : _IIC_Send_Byte.
* Description    : 主机（主控制器）发送一个字节数据到外设
* Input          : None.
* Output         : None.
* Return         : None.
********************************************************************/	   	  
void _IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	_SDA_OUT(); 	    
    _IIC_SCL(0);//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        _IIC_SDA((txd&0x80)>>7);
        txd<<=1; 	  
		delay_us(2);  
		_IIC_SCL(1);
		delay_us(2); 
		_IIC_SCL(0);	
		delay_us(2);
    }	 
} 	






/********************************************************************
* Function Name  : _IIC_Read_Byte.
* Description    : 主机（主控制器）从外设读取一个字节数据
* Input          : None.
* Output         : None.
* Return         : None.
********************************************************************/	
uint8_t _IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	_SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        _IIC_SCL(0); 
        delay_us(2);
		_IIC_SCL(1);
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        _IIC_NAck();//发送nACK
    else
        _IIC_Ack(); //发送ACK   
    return receive;
}







#endif /*TEST_SOFT_IIC*/



