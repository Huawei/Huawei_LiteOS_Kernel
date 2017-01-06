#ifndef MS5611_DRIVER_H
#define MS5611_DRIVER_H

#include "i2c_Config.h"
#include "arm_math.h"
#include "string.h"

#define SEA_PRESS 1013.25f
#define C1 51270
#define C2 51755
#define C3 31295
#define C4 28176
#define C5 32431
#define C6 27811


#define MS5611_NAME "MS5611"

#define  MS561101BA_SlaveAddress 0x77  //定义器件在IIC总线中的从地址
#define  MS561101BA_D1 0x40 
#define  MS561101BA_D2 0x50 
#define  MS561101BA_RST 0x1E 

#define  MS561101BA_D1_OSR_256 0x40 
#define  MS561101BA_D1_OSR_512 0x42 
#define  MS561101BA_D1_OSR_1024 0x44 
#define  MS561101BA_D1_OSR_2048 0x46 
#define  MS561101BA_D1_OSR_4096 0x48 

#define  MS561101BA_D2_OSR_256 0x50 
#define  MS561101BA_D2_OSR_512 0x52 
#define  MS561101BA_D2_OSR_1024 0x54 
#define  MS561101BA_D2_OSR_2048 0x56 
#define  MS561101BA_D2_OSR_4096 0x58 

#define  MS561101BA_PROM_RD 0xA2
uint8_t ms5611_devAddr;
char ms5611_name[10];
uint8_t *ms5611_buffer;
uint16_t C[6];
int32_t dT;
int32_t temperature,pressure,groundPress;


void    MS5611_Config();
	

 bool MS5611_Initialize(void);
	 bool MS5611_TestConnection(void);
	 uint8_t MS5611_GetData(void* data1,void* data2 ,void* data3,void* data4,void* data5 ,void* data6 );

	bool MS5611_Pressure2Float(float *press);
	bool MS5611_Temperature2Float(float *temp);
	bool MS5611_GetPressure();
	bool MS5611_GetTemperature();
	bool MS5611_GetAltitude(float *altitude);

	void MS5611_ConvertPressure();
	void MS5611_ConvertTemperature();
	void MS5611_SetGround(void);
	bool MS5611_ReadPROM(void);
	bool MS5611_Reset();






#endif
