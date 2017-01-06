#ifndef __UltrasonicWave_H
#define	__UltrasonicWave_H
#include "los_typedef.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_gpio.h"
#include "los_sys.h"
#include "los_typedef.h"
#include "gpio_config.h"
#include "stm32f4xx_exti.h"
#include "misc.h"
#include "delay_driver.h"
#include "stm32f4xx_tim.h"

   
#define	TRIG_PORT      GPIOC		//TRIG       
#define	ECHO_PORT      GPIOC		//ECHO 
#define	TRIG_PIN       GPIO_Pin_10       //TRIG       
#define	ECHO_PIN       GPIO_Pin_12	//ECHO   

void UltrasonicWave_Configuration(void);               //对超声波模块初始化
void UltrasonicWave_StartMeasure(void);                //开始测距，发送一个>10us的脉冲，然后测量返回的高电平时间
extern void EXTI15_10_IRQHandler(void);

extern float UltrasonicWave_Distance;      //计算出的距离


#endif /* __UltrasonicWave_H */

