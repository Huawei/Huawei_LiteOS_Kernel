#include "trace_car.h"

/*****************************************************************************
 Function    : trace_car
 Description : Traction trolley initialization
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
UINT32 trace_car()
{	
    GPIO_INT();
    GPIO_PWM();
    TIM4_Configuration();
    while(1)
    {
        Motor_Set(58,58);
        Follow();
        LOS_TaskDelay(50);             
    }
                 
}




