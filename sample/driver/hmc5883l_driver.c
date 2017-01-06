#include "hmc5883l_driver.h"
    uint8_t *hmc5883l_buffer;
void HMC5883L_Config(void)
{
    
	hmc5883l_devAddr = HMC5883L_ADDRESS;
	
	strcpy(hmc5883l_name,HMC5883L_NAME);
}

bool HMC5883L_Initialize(void)
{
    // write CONFIG_A register
   if(!I2C_WriteByte(hmc5883l_devAddr, HMC5883L_RA_CONFIG_A,
        (HMC5883L_AVERAGING_8 << (HMC5883L_CRA_AVERAGE_BIT - HMC5883L_CRA_AVERAGE_LENGTH + 1)) |
        (HMC5883L_RATE_30     << (HMC5883L_CRA_RATE_BIT - HMC5883L_CRA_RATE_LENGTH + 1)) |
        (HMC5883L_BIAS_NORMAL << (HMC5883L_CRA_BIAS_BIT - HMC5883L_CRA_BIAS_LENGTH + 1)))) return false;

    // write CONFIG_B register
	if(!I2C_WriteByte(hmc5883l_devAddr, HMC5883L_RA_CONFIG_B, HMC5883L_GAIN_440 << (HMC5883L_CRB_GAIN_BIT - HMC5883L_CRB_GAIN_LENGTH + 1))) return false;
    
    // write MODE register
    return I2C_WriteByte(hmc5883l_devAddr, HMC5883L_RA_MODE, HMC5883L_MODE_CONTINUOUS << (HMC5883L_MODEREG_BIT - HMC5883L_MODEREG_LENGTH + 1));
}

bool HMC5883L_TestConnection(void) 
{
	uint8_t hmc5883l_buffer[3];
	I2C_ReadBytes(hmc5883l_devAddr, HMC5883L_RA_ID_A, 3, hmc5883l_buffer);
	return (hmc5883l_buffer[0] == 'H' && hmc5883l_buffer[1] == '4' && hmc5883l_buffer[2] == '3');
}

uint8_t HMC5883L_GetData(void* data1,void* data2,void* data3,void* data4,void* data5,void* data6)
{
//	getHeadingCal((int16_t*)data1,(int16_t*)data2,(int16_t*)data3);
	if(data4 != NULL) HMC5883L_GetHeading((float*)data4);
	return 4;
}

// DATA* registers

/** Get 3-axis heading measurements.
 * In the event the ADC reading overflows or underflows for the given channel,
 * or if there is a math overflow during the bias measurement, this data
 * register will contain the value -4096. This register value will clear when
 * after the next valid measurement is made. Note that this method automatically
 * clears the appropriate bit in the MODE register if Single mode is active.
 * @param x 16-bit signed integer container for X-axis heading
 * @param y 16-bit signed integer container for Y-axis heading
 * @param z 16-bit signed integer container for Z-axis heading
 * @see HMC5883L_RA_DATAX_H
 */
void HMC5883L_GetHeadingRaw(int16_t *x, int16_t *y, int16_t *z)         //地磁计数据  未处理零漂    
{
	I2C_RreadBytes(hmc5883l_devAddr, HMC5883L_RA_DATAX_H, 6, hmc5883l_buffer);
//I2Cdev::writeByte(hmc5883l_devAddr, HMC5883L_RA_MODE, HMC5883L_MODE_SINGLE << (HMC5883L_MODEREG_BIT - HMC5883L_MODEREG_LENGTH + 1));
	*x = (((int16_t)hmc5883l_buffer[0]) << 8) | hmc5883l_buffer[1];
	*y = (((int16_t)hmc5883l_buffer[4]) << 8) | hmc5883l_buffer[5];
	*z = (((int16_t)hmc5883l_buffer[2]) << 8) | hmc5883l_buffer[3];
}

//void HMC5883L::getHeadingCal(int16_t *x, int16_t *y, int16_t *z)
//{
//	static int16_t avgX = 0,avgY = 0,avgZ = 0;
//	
//	I2Cdev::readBytes(hmc5883l_devAddr, HMC5883L_RA_DATAX_H, 6, hmc5883l_buffer);
////	I2Cdev::writeByte(hmc5883l_devAddr, HMC5883L_RA_MODE, HMC5883L_MODE_SINGLE << (HMC5883L_MODEREG_BIT - HMC5883L_MODEREG_LENGTH + 1));
//	*x = ((((int16_t)hmc5883l_buffer[0]) << 8) | hmc5883l_buffer[1]) - param.magXOffset;
//	*y = ((((int16_t)hmc5883l_buffer[4]) << 8) | hmc5883l_buffer[5]) - param.magYOffset;
//	*z = ((((int16_t)hmc5883l_buffer[2]) << 8) | hmc5883l_buffer[3]) - param.magZOffset;
//	
//	if(avgX == 0 && avgY == 0) 
//	{
//		avgX = *x;
//		avgY = *y;
//		avgZ = *z;
//	}
//	avgX = (((int32_t)*x)*3 + (int32_t)avgX*5) >> 3;
//	avgY = (((int32_t)*y)*3 + (int32_t)avgY*5) >> 3;
//	avgZ = (((int32_t)*z)*3 + (int32_t)avgZ*5) >> 3;
//		
//	*x = avgX;
//	*y = avgY;
//	*z = avgZ;
//	
////	#define MAG_TAB_SIZE 5
////	static int16_t magXHistTab[MAG_TAB_SIZE] = {0},magYHistTab[MAG_TAB_SIZE] = {0},magZHistTab[MAG_TAB_SIZE] = {0};
////	static int32_t magXSum = 0,magYSum = 0,magZSum = 0;
////	static uint8_t magHistIdx = 0;
////	uint8_t indexplus1 = (magHistIdx + 1);
////	if (indexplus1 == MAG_TAB_SIZE) indexplus1 = 0;

////	magXHistTab[magHistIdx] = *x;
////	magXSum += magXHistTab[magHistIdx];
////	magXSum -= magXHistTab[indexplus1];

////	magYHistTab[magHistIdx] = *y;
////	magYSum += magYHistTab[magHistIdx];
////	magYSum -= magYHistTab[indexplus1];

////	magZHistTab[magHistIdx] = *z;
////	magZSum += magZHistTab[magHistIdx];
////	magZSum -= magZHistTab[indexplus1];

////	magHistIdx = indexplus1;

////	*x = magXSum >> 2;
////	*y = magYSum >> 2;
////	*z = magZSum >> 2;
//}

void HMC5883L_GetHeadingCal(int16_t *x, int16_t *y, int16_t *z)//地磁计数据   处理零漂+滤波
{
	if(I2C_ReadBytes(hmc5883l_devAddr, HMC5883L_RA_DATAX_H, 6, hmc5883l_buffer))
	{
		int16_t xt = ((((int16_t)hmc5883l_buffer[0]) << 8) | hmc5883l_buffer[1]) - magXOffset;
		int16_t yt = ((((int16_t)hmc5883l_buffer[4]) << 8) | hmc5883l_buffer[5]) - magYOffset;
		int16_t zt = ((((int16_t)hmc5883l_buffer[2]) << 8) | hmc5883l_buffer[3]) - magZOffset;

		*x = (((int32_t)*x)*3 + (int32_t)xt*5) >> 3;
		*y = (((int32_t)*y)*3 + (int32_t)yt*5) >> 3;
		*z = (((int32_t)*z)*3 + (int32_t)zt*5) >> 3;
	}
	else
	{
		*x = *y = *z = 0;
	}
}

 void HMC5883L_GetHeading(float *heading)                   //地磁计数据   计算偏角
{
	int16_t x,y,z;
	HMC5883L_GetHeadingCal(&x,&y,&z);
	*heading = atan2((float)y, (float)x);
        if(*heading < 0)
        *heading += 2 * PI;
	*heading = *heading * M_57_3;
}

void HMC5883L_SetOffset(void)
{
#define CALI_TIME_S 20
	uint32_t tick = rt_tick_get() + CALI_TIME_S * 500;
	int16_t data[3],min[3],max[3];
	for(uint8_t i=0;i<3;i++)
	{
		min[i] = 30000;
		max[i] = -30000;
	}
	while(tick>rt_tick_get())
	{
		HMC5883L_GetHeadingRaw(&data[0],&data[1],&data[2]);
		for(uint8_t i=0;i<3;i++)
		{
			if(data[i]<min[i]) min[i] = data[i];
			if(data[i]>max[i]) max[i] = data[i];
		}
		//TODO
		rt_thread_delay(10);
	}
	magXOffset = (min[0] + max[0]) / 2;
	magYOffset = (min[1] + max[1]) / 2;
	magZOffset = (min[2] + max[2]) / 2;
}
