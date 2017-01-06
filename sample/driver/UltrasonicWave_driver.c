#include "UltrasonicWave_driver.h"




float UltrasonicWave_Distance;      //Calculated distance    

/*****************************************************************************
Function :delay_us
Description :Make a probable delay
Input : None
Output : None
Return : None 
******************************************************************************/
void delay_us()
{
    int i,j;
    for(i=0;i<30;i++)
      for(j=0;j<100;j++)
      {
         ;
      }
}

/*****************************************************************************
Function :Timerx_Init
Description :Timer  Initialization
Input : None
Output : None
Return : None 
******************************************************************************/
void Timerx_Init(UINT16 arr,UINT16 psc)
{

    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);          

    TIM_TimeBaseStructure.TIM_Period = 5000-1;                    
    TIM_TimeBaseStructure.TIM_Prescaler =(800-1);             
        
        //TIM_TimeBaseStructure.TIM_Prescaler =(9000-1); 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;                   
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;    
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);              
							 
}
/*****************************************************************************
Function :UltrasonicWave_Configuration
Description : UltrasonicWave Initialization
Input : None
Output : None
Return : None 
******************************************************************************/
void UltrasonicWave_Configuration(void)
{
  	
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
              
    GPIO_InitStructure.GPIO_Pin = TRIG_PIN;	
  	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
    
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		
    GPIO_Init(TRIG_PORT, &GPIO_InitStructure);	
    
    GPIO_InitStructure.GPIO_Pin = ECHO_PIN;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN ; 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(ECHO_PORT, &GPIO_InitStructure);
      

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC,EXTI_PinSource12);//EXTI_PortSourceGPIOC, EXTI_PinSource12       
    EXTI_InitStructure.EXTI_Line=EXTI_Line12;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);		
    Timerx_Init(0,0); 
}

/*****************************************************************************
Function :UltrasonicWave_StartMeasure
Description : first to make  a pulse >=10us,and then measure the time of the high pluse returnned .
Input : None
Output : None
Return : None 
******************************************************************************/
void UltrasonicWave_StartMeasure(void)
{
  	
    GPIO_SetBits(TRIG_PORT,TRIG_PIN); 		  

    delay_4us(20); 
    GPIO_ResetBits(TRIG_PORT,TRIG_PIN);
}

void EXTI15_10_IRQHandler(void)
{    
    UINT32 time1=0;
    if(EXTI_GetITStatus(EXTI_Line12) != RESET)
    {				        
       TIM_Cmd(TIM2, ENABLE);	                         //start the  timer 
       while(GPIO_ReadInputDataBit(ECHO_PORT,ECHO_PIN)==1); //wait until  the high level  returned
       TIM_Cmd(TIM2, DISABLE);	                         //close  the 	timer	 
       time1=TIM_GetCounter(TIM2);	                         //count the time of high level 
       TIM_SetCounter(TIM2,0x00);		 
		                                          
       UltrasonicWave_Distance=time1*1.0625;                       //timer's clock is 16khz   then  distance=time1/16000*340*100/2                                        
       printf("distance:%f\r cm \r\n ",UltrasonicWave_Distance);			 
       EXTI_ClearITPendingBit(EXTI_Line12);  
                   

     }
}





