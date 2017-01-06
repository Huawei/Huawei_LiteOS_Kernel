#include "mpu6050_driver.h"



	
void MPU6050_Config()
{
        uint8_t mpu6050_buffer[14];
	mpu6050_devAddr = (uint8_t)MPU6050_DEFAULT_ADDRESS;
	strcpy(mpu6050_name,MPU6050_NAME);
}



bool MPU6050_Initialize(void)
{
	if(!I2C_WriteByte(mpu6050_devAddr, MPU6050_RA_PWR_MGMT_1, 0x80)) return false;             //PWR_MGMT_1    -- DEVICE_RESET 1
	rt_thread_delay(100);
	if(!I2C_WriteByte(mpu6050_devAddr, MPU6050_RA_PWR_MGMT_1, 0x01)) return false;
	if(!I2C_WriteByte(mpu6050_devAddr, MPU6050_RA_GYRO_CONFIG, 0x10)) return false;
        if(!I2C_WriteByte(mpu6050_devAddr, MPU6050_RA_ACCEL_CONFIG, 0x18)) return false;
        if(!I2C_WriteByte(mpu6050_devAddr, MPU6050_RA_CONFIG, 0x03)) return false; //低通滤波，g延时4.8ms
	if(!I2C_WriteByte(mpu6050_devAddr, MPU6050_RA_SMPLRT_DIV, 0x00)) return false;
	if(!I2C_WriteByte(mpu6050_devAddr, MPU6050_RA_USER_CTRL, 0x00)) return false;
	if(!I2C_WriteByte(mpu6050_devAddr, MPU6050_RA_INT_PIN_CFG, 0xB2)) return false;	
	
/*
	
	I2Cdev::writeByte(mpu6050_devAddr, MPU6050_RA_PWR_MGMT_1, 0x80);             //PWR_MGMT_1    -- DEVICE_RESET 1
	rt_thread_delay(1);
	I2Cdev::writeByte(mpu6050_devAddr, MPU6050_RA_PWR_MGMT_1, 0x03);             //PWR_MGMT_1    -- SLEEP 0; CYCLE 0; TEMP_DIS 0; CLKSEL 3 (PLL with Z Gyro reference)
	I2Cdev::writeByte(mpu6050_devAddr, MPU6050_RA_CONFIG, 0x00); 				//CONFIG        -- EXT_SYNC_SET 0 (disable input pin for data sync) ; default DLPF_CFG = 0 => ACC bandwidth = 260Hz  GYRO bandwidth = 256Hz)
	I2Cdev::writeByte(mpu6050_devAddr, MPU6050_RA_GYRO_CONFIG, 0x18);             //GYRO_CONFIG   -- FS_SEL = 3: Full scale set to 2000 deg/sec  0x18
	
	// enable I2C bypass for AUX I2C
	I2Cdev::writeByte(mpu6050_devAddr, MPU6050_RA_INT_PIN_CFG, 0x02);           	//INT_PIN_CFG   -- INT_LEVEL=0 ; INT_OPEN=0 ; LATCH_INT_EN=0 ; INT_RD_CLEAR=0 ; FSYNC_INT_LEVEL=0 ; FSYNC_INT_EN=0 ; I2C_BYPASS_EN=1 ; CLKOUT_EN=0
	
	I2Cdev::writeByte(mpu6050_devAddr, MPU6050_RA_ACCEL_CONFIG, 0x10);             //ACCEL_CONFIG  -- AFS_SEL=2 (Full Scale = +/-8G)  ; ACCELL_HPF=0   //note something is wrong in the spec. 0x10
	//note: something seems to be wrong in the spec here. With AFS=2 1G = 4096 but according to my measurement: 1G=2048 (and 2048/8 = 256)
	//confirmed here: http://www.multiwii.com/forum/viewtopic.php?f=8&t=1080&start=10#p7480

*/
/*-------------------------------------------------
	mag uses bypass
--------------------------------------------------
	//at this stage, the MAG is configured via the original MAG init function in I2C bypass mode
	//now we configure MPU as a I2C Master device to handle the MAG via the I2C AUX port (done here for HMC5883)
//	I2Cdev::writeByte(MPU6050_ADDRESS, MPU6050_RA_USER_CTRL, 0b00100000);       //USER_CTRL     -- DMP_EN=0 ; FIFO_EN=0 ; I2C_MST_EN=1 (I2C master mode) ; I2C_IF_DIS=0 ; FIFO_RESET=0 ; I2C_MST_RESET=0 ; SIG_COND_RESET=0
//	I2Cdev::writeByte(MPU6050_ADDRESS, MPU6050_RA_INT_PIN_CFG, 0x00);             //INT_PIN_CFG   -- INT_LEVEL=0 ; INT_OPEN=0 ; LATCH_INT_EN=0 ; INT_RD_CLEAR=0 ; FSYNC_INT_LEVEL=0 ; FSYNC_INT_EN=0 ; I2C_BYPASS_EN=0 ; CLKOUT_EN=0
//	I2Cdev::writeByte(MPU6050_ADDRESS, MPU6050_RA_I2C_MST_CTRL, 0x0D);             //I2C_MST_CTRL  -- MULT_MST_EN=0 ; WAIT_FOR_ES=0 ; SLV_3_FIFO_EN=0 ; I2C_MST_P_NSR=0 ; I2C_MST_CLK=13 (I2C slave speed bus = 400kHz)
//	I2Cdev::writeByte(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV0_ADDR, 0x80|MAG_ADDRESS);//I2C_SLV0_ADDR -- I2C_SLV4_RW=1 (read operation) ; I2C_SLV4_ADDR=MAG_ADDRESS
//	I2Cdev::writeByte(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV0_REG, MAG_DATA_REGISTER);//I2C_SLV0_REG  -- 6 data bytes of MAG are stored in 6 registers. First register address is MAG_DATA_REGISTER
//	I2Cdev::writeByte(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV0_CTRL, 0x86);             //I2C_SLV0_CTRL -- I2C_SLV0_EN=1 ; I2C_SLV0_BYTE_SW=0 ; I2C_SLV0_REG_DIS=0 ; I2C_SLV0_GRP=0 ; I2C_SLV0_LEN=3 (3x2 bytes)
*/
	int16_t ax, ay, az;
	int16_t gx, gy, gz;
	for(uint8_t i=0;i<20;i++)
	{
		MPU6050_GetAccelerationRaw(&ax,&ay,&az);
		MPU6050_GetRotationRaw(&gx,&gy,&gz);
		rt_thread_delay(1);
	}
	return true;
}

bool MPU6050_TestConnection(void)
{
	uint8_t who_am_i;
	if(!I2C_ReadByte(mpu6050_devAddr,MPU6050_RA_WHO_AM_I,&who_am_i)) return false;
	return who_am_i == MPU6050_DEFAULT_ADDRESS;
}

uint8_t MPU6050_GetData(void* data1,void* data2,void* data3,void* data4,void* data5,void* data6)
{
  //	getMotion6Cal((int16_t*)data1,(int16_t*)data2,(int16_t*)data3,(int16_t*)data4,(int16_t*)data5,(int16_t*)data6);
	return 6;
}


/*-------------------------------------------------
	ACCEL_*OUT_* registers
--------------------------------------------------*/
//void MPU6050::getMotion9(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz, int16_t* mx, int16_t* my, int16_t* mz)
//{
//    getMotion6(ax, ay, az, gx, gy, gz);
//    // TODO: magnetometer integration
//}

//void MPU6050::getMotion6Cal(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz)
//{
//	static int16_t accXAve = 0, accYAve = 0, accZAve = 0;
//	static int16_t gyroXAve = 0, gyroYAve = 0, gyroZAve = 0;
//	
//    if(I2Cdev::readBytes(mpu6050_devAddr, MPU6050_RA_ACCEL_XOUT_H, 14, mpu6050_buffer))
//	{
//		*ax = ((((int16_t)mpu6050_buffer[0]) << 8) | mpu6050_buffer[1]) - accXOffset;
//		*ay = ((((int16_t)mpu6050_buffer[2]) << 8) | mpu6050_buffer[3]) - accYOffset;
//		*az = ((((int16_t)mpu6050_buffer[4]) << 8) | mpu6050_buffer[5]) - accZOffset;
//		*gx = ((((int16_t)mpu6050_buffer[8]) << 8) | mpu6050_buffer[9]) - gyroXOffset;
//		*gy = ((((int16_t)mpu6050_buffer[10]) << 8) | mpu6050_buffer[11]) - gyroYOffset;
//		*gz = ((((int16_t)mpu6050_buffer[12]) << 8) | mpu6050_buffer[13]) - gyroZOffset;
//		
//		
//		if(accXAve == 0 && accZAve == 0)
//		{
//			accXAve = *ax;
//			accYAve = *ay;
//			accZAve = *az;
//			
//			gyroXAve = *gx;
//			gyroYAve = *gy;
//			gyroZAve = *gz;
//		}
//		accXAve = (((int32_t)*ax)*3 + (int32_t)accXAve*5) >> 3;
//		accYAve = (((int32_t)*ay)*3 + (int32_t)accYAve*5) >> 3;
//		accZAve = (((int32_t)*az)*3 + (int32_t)accZAve*5) >> 3;
//		*ax = accXAve;
//		*ay = accYAve;
//		*az = accZAve;
//		
//		gyroXAve = (((int32_t)*gx)*15 + (int32_t)gyroXAve) >> 4;
//		gyroYAve = (((int32_t)*gy)*15 + (int32_t)gyroYAve) >> 4;
//		gyroZAve = (((int32_t)*gz)*15 + (int32_t)gyroZAve) >> 4;
//		*gx = gyroXAve;
//		*gy = gyroYAve;
//		*gz = gyroZAve;		
//	}
//	else
//	{
//		*ax = 0;
//		*ay = 0;
//		*az = 2048;
//		*gx = 0;
//		*gy = 0;
//		*gz = 0;
//	}
//}

void MPU6050_GetMotion6Cal(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz)        //陀螺仪加速度计零漂处理过的   
{
    if(I2C_ReadBytes(mpu6050_devAddr, MPU6050_RA_ACCEL_XOUT_H, 14, mpu6050_buffer))
	{
		int16_t axt = ((((int16_t)mpu6050_buffer[0]) << 8) | mpu6050_buffer[1]) - accXOffset;
		int16_t ayt = ((((int16_t)mpu6050_buffer[2]) << 8) | mpu6050_buffer[3]) - accYOffset;
		int16_t azt = ((((int16_t)mpu6050_buffer[4]) << 8) | mpu6050_buffer[5]) - accZOffset;
		int16_t gxt = ((((int16_t)mpu6050_buffer[8]) << 8) | mpu6050_buffer[9]) - gyroXOffset;
		int16_t gyt = ((((int16_t)mpu6050_buffer[10]) << 8) | mpu6050_buffer[11]) - gyroYOffset;
		int16_t gzt = ((((int16_t)mpu6050_buffer[12]) << 8) | mpu6050_buffer[13]) - gyroZOffset;
		
		ax = (((int32_t)ax)*3 + (int32_t)axt*5) >> 3;
		ay = (((int32_t)ay)*3 + (int32_t)ayt*5) >> 3;
		az = (((int32_t)az)*3 + (int32_t)azt*5) >> 3;
		gx = (((int32_t)gx)*15 + (int32_t)gxt) >> 4;
		gy = (((int32_t)gy)*15 + (int32_t)gyt) >> 4;
		gz = (((int32_t)gz)*15 + (int32_t)gzt) >> 4;
	}
	else
	{
		ax = 0;
		ay = 0;
		az = 2048;
		gx = 0;
		gy = 0;
		gz = 0;
	}
}

void MPU6050_GetAccelerationRaw(int16_t* x, int16_t* y, int16_t* z)         //加速度计数据  未处理的
{
    I2C_ReadBytes(mpu6050_devAddr, MPU6050_RA_ACCEL_XOUT_H, 6, mpu6050_buffer);
    *x = (((int16_t)mpu6050_buffer[0]) << 8) | mpu6050_buffer[1];
    *y = (((int16_t)mpu6050_buffer[2]) << 8) | mpu6050_buffer[3];
    *z = (((int16_t)mpu6050_buffer[4]) << 8) | mpu6050_buffer[5];
}

void MPU6050_GetAccelerationCal(int16_t* ax, int16_t* ay, int16_t* az)       //加速度计数据  零漂处理
{
    I2C_ReadBytes(mpu6050_devAddr, MPU6050_RA_ACCEL_XOUT_H, 6, mpu6050_buffer);
    *ax = ((((int16_t)mpu6050_buffer[0]) << 8) | mpu6050_buffer[1]) - accXOffset;
    *ay = ((((int16_t)mpu6050_buffer[2]) << 8) | mpu6050_buffer[3]) - accYOffset;
    *az = ((((int16_t)mpu6050_buffer[4]) << 8) | mpu6050_buffer[5]) - accZOffset;
	
//	accXAve = (*ax + accXAve) >> 1;
//	accYAve = (*ay + accYAve) >> 1;
//	accZAve = (*az + accZAve) >> 1;
//	
//	*ax = accXAve;
//	*ay = accYAve;
//	*az = accZAve;
}

//int16_t MPU6050::getAccelerationX()
//{

//}
//int16_t MPU6050::getAccelerationY()
//{

//}
//int16_t MPU6050::getAccelerationZ()
//{

//}

/*-------------------------------------------------
	TEMPERATURE_*OUT_* registers
--------------------------------------------------*/
int16_t MPU6050_GetTemperature()
{
    I2C_ReadBytes(mpu6050_devAddr, MPU6050_RA_TEMP_OUT_H, 2, mpu6050_buffer);
    return (((int16_t)mpu6050_buffer[0]) << 8) | mpu6050_buffer[1];
}
/*-------------------------------------------------
	GYRO_*OUT_* registers
--------------------------------------------------*/
void MPU6050_GetRotationRaw(int16_t* x, int16_t* y, int16_t* z)							//陀螺仪数据   未处理的      
{
    I2C_ReadBytes(mpu6050_devAddr, MPU6050_RA_GYRO_XOUT_H, 6, mpu6050_buffer);
    *x = (((int16_t)mpu6050_buffer[0]) << 8) | mpu6050_buffer[1];
    *y = (((int16_t)mpu6050_buffer[2]) << 8) | mpu6050_buffer[3];
    *z = (((int16_t)mpu6050_buffer[4]) << 8) | mpu6050_buffer[5];
}

void MPU6050_GetRotationCal(int16_t* gx, int16_t* gy, int16_t* gz)           //陀螺仪数据   零漂处理的
{
    I2Cdev_RreadBytes(mpu6050_devAddr, MPU6050_RA_GYRO_XOUT_H, 6, mpu6050_buffer);
    *gx = ((((int16_t)mpu6050_buffer[0]) << 8) | mpu6050_buffer[1]) - gyroXOffset;
    *gy = ((((int16_t)mpu6050_buffer[2]) << 8) | mpu6050_buffer[3]) - gyroYOffset;
    *gz = ((((int16_t)mpu6050_buffer[4]) << 8) | mpu6050_buffer[5]) - gyroZOffset;
	
//	gyroXAve = (*gx + gyroXAve) >> 1;
//	gyroYAve = (*gy + gyroYAve) >> 1;
//	gyroZAve = (*gz + gyroZAve) >> 1;
//	
//	*gx = gyroXAve;
//	*gy = gyroYAve;
//	*gz = gyroZAve;
}

//int16_t MPU6050::getRotationX();
//int16_t MPU6050::getRotationY();
//int16_t MPU6050::getRotationZ();

void MPU6050_SetOffset(void)                                  //计算零漂
{
	int16_t ax, ay, az;
	int16_t gx, gy, gz;
	for(uint8_t i=0;i<200;i++)
	{	
		MPU6050_GetAccelerationRaw(&ax,&ay,&az);
		MPU6050_GetRotationRaw(&gx,&gy,&gz);
		
		accXOffset = (accXOffset + ax) >> 1;
		accYOffset = (accYOffset + ay) >> 1;
		accZOffset = (accZOffset + az) >> 1;
		
		gyroXOffset = (gyroXOffset + gx) >> 1;
		gyroYOffset = (gyroYOffset + gy) >> 1;
		gyroZOffset = (gyroZOffset + gz) >> 1;
		
		rt_thread_delay(1);
	}
	accZOffset -= 2048;
}
