#ifndef _TRACE_CAR_DRIVER_H_
#define _TRACE_CAR_DRIVER_H_
#include "stm32f4xx.h"

#define Motor_R_IN1_Set GPIO_SetBits(GPIOB,GPIO_Pin_14);	//Input 1 is high
#define Motor_R_IN1_Reset GPIO_ResetBits(GPIOB,GPIO_Pin_14);	//Input 1 is low
#define Motor_R_IN2_Set GPIO_SetBits(GPIOB,GPIO_Pin_15);	//Input 2 is high
#define Motor_R_IN2_Reset GPIO_ResetBits(GPIOB,GPIO_Pin_15);	//Input 2 is low-----------This is the right motor-


#define Motor_L_IN1_Set GPIO_SetBits(GPIOB,GPIO_Pin_8);		//Input 1 is high
#define Motor_L_IN1_Reset GPIO_ResetBits(GPIOB,GPIO_Pin_8);	//Input 1 is low
#define Motor_L_IN2_Set GPIO_SetBits(GPIOB,GPIO_Pin_9);		//Input 2 is high
#define Motor_L_IN2_Reset GPIO_ResetBits(GPIOB,GPIO_Pin_9);	//Input 2 is low---------This is the left motor------


#define Follow_L() GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)	//Reads the left tracking sensor status
#define Follow_M() GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)	//The status of the intermediate tracking sensor is read
#define Follow_R() GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_9)	//Reads the right tracking sensor status
extern s16 TIM_T;
extern u16 Drv_PWM_L;
extern u16 Drv_PWM_R;

extern void  Follow(void);
extern void  GPIO_INT(void);
extern void  delay1ms(int n);
extern void  Motor_PWM_EN(void);
extern void  Motor_Set(s16 LS,s16 RS);
extern void  EXTI9_5_IRQHandler(void);
extern void TIM4_Configuration(void);
extern void GPIO_PWM(void);

#endif
