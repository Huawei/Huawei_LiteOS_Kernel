#include "los_config.h"
#include "los_task.h"
#include "web_server.h"

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
    UINT32 Web_ServerTaskID;
    TSK_INIT_PARAM_S stTaskInitParam;

    /*******LED*******************************************************************/
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)Web_Server;
    stTaskInitParam.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    stTaskInitParam.pcName = "Web_Server";
    stTaskInitParam.usTaskPrio = 4;
    uwRet = LOS_TaskCreate(&Web_ServerTaskID, &stTaskInitParam);

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