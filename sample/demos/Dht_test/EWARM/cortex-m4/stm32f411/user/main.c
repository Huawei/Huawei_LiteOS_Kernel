#include "los_config.h"
#include "los_task.h"
#include "los_task.ph"
#include "dht_test.h"

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
    UINT32 g_uwDHTTaskID;//--------temp var ?----------
    TSK_INIT_PARAM_S stTaskInitParam;

 /*************DHT11*************************************************/
   
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)DHT11_TEST;
    stTaskInitParam.uwStackSize = LOSCFG_BASE_CORE_TSK_IDLE_STACK_SIZE;
    stTaskInitParam.pcName = "DHT11_TEST";
    stTaskInitParam.usTaskPrio = OS_TASK_PRIORITY_LOWEST-1;//------modified !!!---------
    uwRet = LOS_TaskCreate(&g_uwDHTTaskID, &stTaskInitParam);
    
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
    {;
        PRINT_ERR("LOS_AppInit error\n");
        return LOS_NOK;
    }
    
    LOS_Start();

    for (;;);
    /* Replace the dots (...) with your own code.  */
}