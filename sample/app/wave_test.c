#include "wave_test.h"
/*****************************************************************************
 Function    : WAVE
 Description : Configuration USART1
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
UINT32 UltrasonicWave()
{
     USART2_Config();
     UltrasonicWave_Configuration();
	while(1)
	{
                       printf("microwave start!\r\n");
                       UltrasonicWave_StartMeasure();		  
                       LOS_TaskDelay(50);
         } 
}










