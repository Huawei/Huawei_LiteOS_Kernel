#include "led_flash.h"
  
/*****************************************************************************
 Function    : LED_toggle
 Description : LD2 on the stm32f411 Nucleo ON/OFF,and serial port printf LED ON!/LED OFF!
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
UINT32 LED_toggle(void)
{
    UINT32 uwRet;
    USART2_Config();
    printf("USART Config\r\n");
    LED_Config();
    printf("LED Config\r\n");
    while(1)
    {
        LED_ON();
        printf("LED ON!\r\n");
        uwRet = LOS_TaskDelay(500); //delay 500ms
        if (uwRet != LOS_OK)
        {
            printf("Delay Task Failed.\r\n");
            return LOS_NOK;
        }
        LED_OFF();
        printf("LED OFF!\r\n");
        uwRet = LOS_TaskDelay(500);
        if (uwRet != LOS_OK)
        {
            printf("Delay Task Failed.\r\n");
            return LOS_NOK;
        }

    }    
}
