#include "los_config.h"
#include "los_task.h"
#include "esp8266_test.h"
#include "los_hwi.h"


/*****************************************************************************
 Function    : LOS_AppInit
 Description : Task function entry
 Input       : None
 Output      : None
 Return      : None 
******************************************************************************/
UINT32 LOS_AppInit()
{
    UINT32 uwRet;
    UINT32 LED_FlickerTaskID;
    TSK_INIT_PARAM_S stTaskInitParam;
  
    /*******LED*******************************************************************/
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)ESP8266_Test;
    stTaskInitParam.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    stTaskInitParam.pcName = "ESP8266_Test";
    stTaskInitParam.usTaskPrio = 4;
    uwRet = LOS_TaskCreate(&LED_FlickerTaskID, &stTaskInitParam);
    LOS_HwiCreate(37,0x2,0,USART1_IRQHandler,0);
    if (uwRet != LOS_OK)
    {
        return uwRet;
    }
   
  
   return LOS_OK;
    
}

/*****************************************************************************
 Function    : main
 Description : Main function entry
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
LITE_OS_SEC_TEXT_INIT
int main(void)
{
    UINT32 uwRet;
    uwRet = osMain();
    if (uwRet != LOS_OK) 
    {
        return LOS_NOK;
    }
 /******Application task init*************************************************/
    uwRet = LOS_AppInit();
    if (uwRet != LOS_OK) 
    {
        PRINT_ERR("LOS_AppInit error\n");
        return LOS_NOK;
    }
    
    LOS_Start();

    for (;;);
    /* Replace the dots (...) with your own code.  */
}