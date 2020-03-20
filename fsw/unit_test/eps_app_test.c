/*
  Copyright (C) 2009 - 2016 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

  This software is provided "as is" without any warranty of any, kind either express, implied, or statutory, including, but not
  limited to, any warranty that the software will conform to, specifications any implied warranties of merchantability, fitness
  for a particular purpose, and freedom from infringement, and any warranty that the documentation will conform to the program, or
  any warranty that the software will be error free.

  In no event shall NASA be liable for any damages, including, but not limited to direct, indirect, special or consequential damages,
  arising out of, resulting from, or in any way connected with the software or its documentation.  Whether or not based upon warranty,
  contract, tort or otherwise, and whether or not loss was sustained from, or arose out of the results of, or use of, the software,
  documentation or services provided hereunder

  ITC Team
  NASA IV&V
  ivv-itc@lists.nasa.gov
*/

#include "eps_app_test.h"
#include "eps_test_utils.h"

#include <eps_events.h>
#include <eps_app.h>
#include <cfe_evs.h>

#include <eps_app.h>
#include <eps_msgids.h>
#include <eps_msgdefs.h>

#include <uttest.h>
#include <utassert.h>
#include <ut_cfe_sb_hooks.h>
#include <ut_cfe_evs_hooks.h>
#include <ut_cfe_es_stubs.h>
#include <ut_cfe_sb_stubs.h>

#include <stdio.h>

#define REGISTER_APP_FAIL       9999
#define CREATE_PIPE_FAIL        9998
#define SUBSCRIBE_FAIL_CMD      9997
#define SUBSCRIBE_FAIL_SEND     9996

extern i2c_data_t eps_i2c_data;

int eps_cmd_mid_fail;
int eps_send_hk_fail;

int32 Ut_CFE_ES_RegisterApp_Hook(void)
{
    return REGISTER_APP_FAIL;
}

int32 Ut_CFE_SB_CreatePipe_Hook(CFE_SB_PipeId_t *PipeIdPtr, uint16 Depth, char *PipeName)
{
    return CREATE_PIPE_FAIL;
}

int32 Ut_CFE_SB_Subscribe_Hook(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
{
    int32 retval = 0;
    if(MsgId == EPS_CMD_MID && eps_cmd_mid_fail == 1)
        retval = SUBSCRIBE_FAIL_CMD;
    if(MsgId == EPS_SEND_HK_MID && eps_send_hk_fail == 1)
        retval = SUBSCRIBE_FAIL_SEND;

    return retval;
}

static void EPS_App_Test_APPINIT_NOMINAL(void)
{
    int32 status = EPS_app_init();
    UtAssert_True(status == CFE_SUCCESS, "EPS App:  AppInit Nominal successful");
    UtAssert_EventIDSent(EPS_INIT_EID, CFE_EVS_INFORMATION,"EPS Startup Event Sent, Task Initialized");
}

static void EPS_App_Test_APPINIT_REGISTERAPP_FAIL(void)
{
    int32 status = CFE_SUCCESS;
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_REGISTERAPP_INDEX, &Ut_CFE_ES_RegisterApp_Hook);
    UtAssert_True(status == CFE_SUCCESS, "Status == CFE_SUCCESS");

    status = EPS_app_init();
    UtAssert_True(status == REGISTER_APP_FAIL, "Status == 9999");
}

static void EPS_App_Test_APPINIT_CREATEPIPE_FAIL(void)
{
    int32 status = CFE_SUCCESS;
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_CREATEPIPE_INDEX, &Ut_CFE_SB_CreatePipe_Hook);

    UtAssert_True(status == CFE_SUCCESS, "Status == CFE_SUCCES");

    status = EPS_app_init();
    UtAssert_EventIDSent(EPS_SB_ERR_EID, CFE_EVS_ERROR, "Create Pipe Failure Event Sent");
    UtAssert_True(status == CREATE_PIPE_FAIL, "Status == 9998");
}

static void EPS_App_Test_APPINIT_SUBSCRIBE_CMD_FAIL(void)
{
    eps_cmd_mid_fail = 1;
    int32 status = CFE_SUCCESS;

    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_SUBSCRIBE_INDEX, &Ut_CFE_SB_Subscribe_Hook);
    status = EPS_app_init();

    UtAssert_True(status == SUBSCRIBE_FAIL_CMD, "Status == 9997");
    UtAssert_EventIDSent(EPS_SB_ERR_EID, CFE_EVS_ERROR, "Subscribe CMD Failure Event Sent");
}

static void EPS_App_Test_APPINIT_SUBSCRIBE_SEND_FAIL(void)
{
    eps_send_hk_fail = 1;
    int32 status = CFE_SUCCESS;

    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_SUBSCRIBE_INDEX, &Ut_CFE_SB_Subscribe_Hook);
    status = EPS_app_init();

    UtAssert_True(status == SUBSCRIBE_FAIL_SEND, "Status == 9997");
    UtAssert_EventIDSent(EPS_SB_ERR_EID, CFE_EVS_ERROR, "Subscribe SEND Failure Event Sent");
}

void EPS_App_Test_AddTestCases(void)
{
    UtTest_Add(EPS_App_Test_APPINIT_NOMINAL, EPS_Test_Setup, EPS_Test_TearDown,
        "EPS App Test: Nominal App Init");
    UtTest_Add(EPS_App_Test_APPINIT_REGISTERAPP_FAIL, EPS_Test_Setup, EPS_Test_TearDown,
        "EPS App Test: Register App Fail");
    UtTest_Add(EPS_App_Test_APPINIT_CREATEPIPE_FAIL, EPS_Test_Setup, EPS_Test_TearDown,
        "EPS App Test: Create Pipe Fail");
    UtTest_Add(EPS_App_Test_APPINIT_SUBSCRIBE_CMD_FAIL, EPS_Test_Setup, EPS_Test_TearDown,
        "EPS App Test: Subscribe CMD Fail");
    UtTest_Add(EPS_App_Test_APPINIT_SUBSCRIBE_SEND_FAIL, EPS_Test_Setup, EPS_Test_TearDown,
        "EPS App Test: Subscribe SEND Fail");
}