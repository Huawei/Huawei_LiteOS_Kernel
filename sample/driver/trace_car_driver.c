#include "trace_car_driver.h"

s16 TIM_T=144;
u16 Drv_PWM_L=100;
u16 Drv_PWM_R=100;

void delay1ms(int n)
{
    u32 i;
    while(n--)
    {
        i=1000;
        while(i--);
    }
} 

/*****************************************************************************
 Function    : EXTI9_5_IRQHandler
 Description : External interrupt
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line5) != RESET)
    {		
        Follow();
        EXTI_ClearITPendingBit(EXTI_Line5);	
	
    }
    else if(EXTI_GetITStatus(EXTI_Line7) != RESET)
    {
        Follow();
        EXTI_ClearITPendingBit(EXTI_Line7);	
	
    }
    else if(EXTI_GetITStatus(EXTI_Line9) != RESET)
    {
        Follow();
        EXTI_ClearITPendingBit(EXTI_Line9);	
    }		
}



/*****************************************************************************
 Function    : TIM4_Configuration
 Description : The general-purpose timer 4 is set so that its channels 1 and 2 output PWM
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void TIM4_Configuration(void)
{	
    TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;
    TIM_OCInitTypeDef	TIM_OCInitStructure; 
    /* TIM4 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = TIM_T;          
    TIM_TimeBaseStructure.TIM_Prescaler = 4000;       
    TIM_TimeBaseStructure.TIM_ClockDivision =  0x0;    
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);		 

    /* Channel 1 Configuration in PWM mode */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 				
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	
    TIM_OCInitStructure.TIM_Pulse = Drv_PWM_L; 							
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 		
    TIM_OC1Init(TIM4,&TIM_OCInitStructure); 					

    /* Channel 2 Configuration in PWM mode */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 				
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	
    TIM_OCInitStructure.TIM_Pulse = Drv_PWM_R; 							
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 		
    TIM_OC2Init(TIM4,&TIM_OCInitStructure); 					

    /* TIM4 counter enable */
    TIM_Cmd(TIM4,ENABLE);
    /* TIM4 Main Output Enable */
    TIM_CtrlPWMOutputs(TIM4,ENABLE);
}

/*****************************************************************************
 Function    : GPIO_PWM
 Description : Initialize the PWM IO port
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void GPIO_PWM(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    //-------------- Two-way motor drive ------ PB8, 9 ----- PB14, 15-----------
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;       
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 	//
    GPIO_Init(GPIOB, &GPIO_InitStructure);
        
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 	//
    GPIO_Init(GPIOB, &GPIO_InitStructure);
        
}

/*****************************************************************************
 Function    : GPIO_INT
 Description : Initialize the photoelectric on the tube IO port
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void GPIO_INT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
                              
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
                
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7|GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
                
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN ; 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource5|EXTI_PinSource7|EXTI_PinSource9);

    EXTI_InitStructure.EXTI_Line = EXTI_Line5|EXTI_Line7|EXTI_Line9;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger =  EXTI_Trigger_Rising;	  
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	
                
}

/*****************************************************************************
 Function    : Motor_Stop
 Description : Initialize the PWM IO port
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void Motor_Stop(void)
{
    Motor_L_IN1_Reset;
    Motor_L_IN2_Reset;
    Motor_R_IN1_Reset;
    Motor_R_IN2_Reset;  
}

/*****************************************************************************
 Function    : Motor_L_PWM
 Description : Left motor PWM
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void Motor_L_PWM(s16 speed_L)
{
    if(speed_L>0)
    {
        if(speed_L>TIM_T)
        {
            speed_L=TIM_T;
        } 	
        Motor_L_IN1_Set;
        Motor_L_IN2_Reset;
        Drv_PWM_L=(u16)speed_L;	
    }
    else
    {	
        Motor_L_IN1_Reset;	
        Motor_L_IN2_Reset;
        Drv_PWM_L=72;	
    }	
    TIM_SetCompare1(TIM4,Drv_PWM_L);
}


/*****************************************************************************
 Function    : Motor_R_PWM
 Description : Right motor PWM
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void Motor_R_PWM(s16 speed_R)
{
    if(speed_R>0)
    {
        if(speed_R>TIM_T)
        {
            speed_R=TIM_T;
        }
        Motor_R_IN1_Set;
        Motor_R_IN2_Reset;
        Drv_PWM_R=(u16)speed_R;	
    }
    else
    {
		
        Motor_R_IN1_Reset;	
        Motor_R_IN2_Reset;
        Drv_PWM_R=72;	
    }	
	
    TIM_SetCompare2(TIM4,Drv_PWM_R);

}

/*****************************************************************************
 Function    : Motor_Set
 Description : Enable motor PWM
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void Motor_Set(s16 LS,s16 RS)
{
    Motor_L_PWM(LS);
    Motor_R_PWM(RS);			
}

/*****************************************************************************
 Function    : Follow
 Description : Initialize the PWM IO port
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void  Follow(void)
{
	
    s8 temp=0x00,State=0x00;
    temp = Follow_L();
    temp = (temp<<1)|Follow_M();
    temp = (temp<<1)|Follow_R();
    State= temp&0x07;

    switch(State)
    {
        case 0x00:  
        {
            Motor_Set(58,58);
            break;
        }
				
        case 0x01:  
        {
            Motor_Set(58,0);
            break;
        }
				
        case 0x04:  
        {
            Motor_Set(0,58);
            break;
        }
    }
}

