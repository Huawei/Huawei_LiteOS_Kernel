#include "los_config.h"
#include "los_task.h"
#include "los_task.ph"
#include "wave_test.h"

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
    UINT32 g_uwwaveTaskID;//--------temp var ----------
    TSK_INIT_PARAM_S stTaskInitParam;


/*************microwave**************************************************/
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)UltrasonicWave;
    stTaskInitParam.uwStackSize = LOSCFG_BASE_CORE_TSK_IDLE_STACK_SIZE;
    stTaskInitParam.pcName = "MICROWAVE";
    stTaskInitParam.usTaskPrio = OS_TASK_PRIORITY_LOWEST-3;//--------------------------
    uwRet = LOS_TaskCreate(&g_uwwaveTaskID, &stTaskInitParam);
   
    if (uwRet != LOS_OK)
    {
        return uwRet;
    }   
    uwRet=LOS_HwiCreate(EXTI15_10_IRQn, 0x2, 0,EXTI15_10_IRQHandler, 0);//Create interrupt 
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
    {;
        PRINT_ERR("LOS_AppInit error\n");
        return LOS_NOK;
    }
    
    LOS_Start();

    for (;;);
    /* Replace the dots (...) with your own code.  */
}