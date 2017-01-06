#include "i2c_config.h"



void I2C_Delay(void)
{
    volatile uint16_t i = SCL_TIME_OUT;
    while (i)
        i--;
}

bool I2C_Start(void)
{
    SDA_H;
    SCL_H;
    I2C_Delay();
    if (!SDA_read)
        return false;
    SDA_L;
    I2C_Delay();
    if (SDA_read)
        return false;
    SDA_L;
    I2C_Delay();
    return true;
}

void I2C_Stop(void)
{
    SCL_L;
    I2C_Delay();
    SDA_L;
    I2C_Delay();
    SCL_H;
    I2C_Delay();
    SDA_H;
    I2C_Delay();
}

void I2C_Ack(void)
{
    SCL_L;
    I2C_Delay();
    SDA_L;
    I2C_Delay();
    SCL_H;
    I2C_Delay();
    SCL_L;
    I2C_Delay();
}

void I2C_NoAck(void)
{
    SCL_L;
    I2C_Delay();
    SDA_H;
    I2C_Delay();
    SCL_H;
    I2C_Delay();
    SCL_L;
    I2C_Delay();
}

bool I2C_WaitAck(void)
{
//	u8 n = 0;
//    SCL_L;
    I2C_Delay();
    SDA_H;
    I2C_Delay();
    SCL_H;
    I2C_Delay();
    if(!SDA_read) {
        SCL_L;
        return true;
    }
    SCL_L;
    return false;
}
//===================================================================================
//===================================================================================
void I2C_SendByte(uint8_t byte)
{
    uint8_t i = 8;
    while (i--) {
        SCL_L;
        I2C_Delay();
        if (byte & 0x80)
            SDA_H;
        else
            SDA_L;
        byte <<= 1;
        I2C_Delay();
        SCL_H;
        I2C_Delay();
    }
    SCL_L;
}

uint8_t I2C_ReceiveByte(void)
{
    uint8_t i = 8;
    uint8_t byte = 0;

    SDA_H;
    while (i--) {
        byte <<= 1;
        SCL_L;
        I2C_Delay();
        SCL_H;
        I2C_Delay();
        if (SDA_read) {
            byte |= 0x01;
        }
    }
    SCL_L;
    return byte;
}
//===================================================================================
//===================================================================================
void I2C_Initialize(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 

	GPIO_Init(GPIOB, &GPIO_InitStructure);
	printf("I2Cinit over\r\n");
}

bool I2C_WriteBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data)
{
	rt_enter_critical();
    int i;
    if (!I2C_Start())
	{
		rt_exit_critical();
        return false;
	}
    I2C_SendByte(devAddr << 1 | I2C_Direction_Transmitter);
    if (!I2C_WaitAck()) {
        I2C_Stop();
		rt_exit_critical();
        return false;
    }
    I2C_SendByte(regAddr);
    if (!I2C_WaitAck()) {
        I2C_Stop();
		rt_exit_critical();
        return false;
    }
    for (i = 0; i < length; i++) {
        I2C_SendByte(data[i]);
        if (!I2C_WaitAck()) {
            I2C_Stop();
			rt_exit_critical();
            return false;
        }
    }
    I2C_Stop();
	rt_exit_critical();
    return true;
}

bool I2C_WriteByte(uint8_t devAddr,uint8_t regAddr,uint8_t data)
{
	return I2C_WriteBytes(devAddr,regAddr,1,&data);
}

bool I2C_ReadBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data)
{
	rt_enter_critical();
    if (!I2C_Start())
	{
		rt_exit_critical();
        return false;
	}
    I2C_SendByte(devAddr << 1 | I2C_Direction_Transmitter);
    if (!I2C_WaitAck()) {
        I2C_Stop();
		rt_exit_critical();
        return false;
    }
    I2C_SendByte(regAddr);
    if (!I2C_WaitAck()) {
        I2C_Stop();
		rt_exit_critical();
        return false;
    }
	I2C_Stop();//增加一个stop
    I2C_Start();
    I2C_SendByte(devAddr << 1 | I2C_Direction_Receiver);
    if (!I2C_WaitAck()) {
        I2C_Stop();
		rt_exit_critical();
        return false;
    }
    while (length) {
        *data = I2C_ReceiveByte();
        if (length == 1)
            I2C_NoAck();
        else
            I2C_Ack();
        data++;
        length--;
    }
    I2C_Stop();
	rt_exit_critical();
    return true;
}

bool I2C_ReadByte(uint8_t devAddr,uint8_t regAddr,uint8_t *data)
{
	return I2C_ReadBytes(devAddr,regAddr,1,data);
}

bool I2C_WriteBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data) {
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 00011100 mask byte
    // 10101111 original value (sample)
    // 10100011 original & ~mask
    // 10101011 masked | value
    uint8_t b;
    if(!I2C_ReadByte(devAddr, regAddr, &b)) return false;
	uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
	data <<= (bitStart - length + 1); // shift data into correct position
	data &= mask; // zero all non-important bits in data
	b &= ~(mask); // zero all important bits in existing byte
	b |= data; // combine data with existing byte
	return I2C_WriteByte(devAddr, regAddr, b);
}

/*

void I2C_writeByte(uint8_t SlaveAddress,uint8_t REG_Address,uint8_t data)
{
//	rt_enter_critical();

//	SlaveAddress <<= 1;

//	I2C_GenerateSTART(I2C1,ENABLE);

//	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));

//	I2C_Send7bitAddress(I2C1,SlaveAddress,I2C_Direction_Transmitter);

//	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

//	I2C_SendData(I2C1,REG_Address);

//	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));

//	I2C_SendData(I2C1,data);

//	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));

//	I2C_GenerateSTOP(I2C1,ENABLE);
//	
//	rt_exit_critical();
	

	writeBytes(SlaveAddress,REG_Address,1,&data);
}

void I2C_readByte(uint8_t SlaveAddress,uint8_t REG_Address,uint8_t *data)
{
	rt_enter_critical();

	SlaveAddress <<= 1;

	uint8_t REG_data;

	if(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY))
	{
		I2C1->CR1|=0x8000;             // reset i2c lines,disable i2c
		I2C1->CR1&=~0x8000;             // reset i2c lines,enable i2c
	}
	
	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));

	I2C_GenerateSTART(I2C1,ENABLE);//起始信号

	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C1,SlaveAddress,I2C_Direction_Transmitter);//发送设备地址+写信号

	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));//

	//I2C_Cmd(I2C1,ENABLE);

	I2C_SendData(I2C1,REG_Address);//发送存储单元地址，从0开始

	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_GenerateSTART(I2C1,ENABLE);//起始信号

	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C1,SlaveAddress,I2C_Direction_Receiver);//发送设备地址+读信号

	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	I2C_AcknowledgeConfig(I2C1,DISABLE);

	I2C_GenerateSTOP(I2C1,ENABLE);

	REG_data=I2C_ReceiveData(I2C1);//读出寄存器数据
	
	while(!(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED)));
	
	*data = REG_data;
	
	GenerateSTOP();
	
	if(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY))
	{
		I2C1->CR1|=0x8000;             // reset i2c lines,disable i2c
		I2C_GenerateSTOP(I2C1,ENABLE);
		I2C1->CR1&=~0x8000;             // reset i2c lines,enable i2c
		I2C_GenerateSTOP(I2C1,ENABLE);
	}

	rt_exit_critical();
	
//	readBytes(SlaveAddress,REG_Address,1,data);
}

void I2C_writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data)
{	
	rt_enter_critical();
	
	devAddr = devAddr << 1;
	
	if(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY))
	{
		I2C1->CR1|=0x8000;             // reset i2c lines,disable i2c
		I2C1->CR1&=~0x8000;             // reset i2c lines,enable i2c
	}
	
	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
	I2C_GenerateSTART(I2C1,ENABLE);//起始信号
	
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1,devAddr,I2C_Direction_Transmitter);//发送设备地址+写信号
	
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));//
	I2C_SendData(I2C1,regAddr);//发送存储单元地址，从0开始
	
    for (uint8_t i = 0; i < length; i++)
	{
		while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		I2C_SendData(I2C1,data[i]);
	}
    	
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTOP(I2C1,ENABLE);
	
	rt_exit_critical();
}

void I2C_readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data)
{
	rt_enter_critical();
	
	devAddr = devAddr << 1;
	
	if(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY))
	{
		I2C1->CR1|=0x8000;             // reset i2c lines,disable i2c
		I2C1->CR1&=~0x8000;             // reset i2c lines,enable i2c
	}
	
	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
	I2C_GenerateSTART(I2C1,ENABLE);//起始信号
	
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1,devAddr,I2C_Direction_Transmitter);//发送设备地址+写信号
	
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));//
	I2C_Cmd(I2C1,ENABLE);
	I2C_SendData(I2C1,regAddr);//发送存储单元地址，从0开始
	
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTART(I2C1,ENABLE);//起始信号

	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1,devAddr,I2C_Direction_Receiver);//发送设备地址+读信号
	
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	for (uint8_t k = 0; k < length; k++) 
	{
		if(k == length -1)
		{
			I2C_AcknowledgeConfig(I2C1,DISABLE);
			I2C_GenerateSTOP(I2C1,ENABLE);
		}
		data[k]=I2C_ReceiveData(I2C1);//读出寄存器数据
		while(!(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED)));
	}
	I2C_GenerateSTOP(I2C1,ENABLE);
	I2C_GenerateSTOP(I2C1,ENABLE);
	I2C_GenerateSTOP(I2C1,ENABLE);
	if(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY))
	{
		int a = 0;
	}
	else
	{
		int b = 0;
	}
	I2C_AcknowledgeConfig(I2C1,ENABLE);
	
	rt_exit_critical();
}



*/
