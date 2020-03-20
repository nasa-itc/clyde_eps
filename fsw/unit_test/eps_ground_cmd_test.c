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

#include "eps_ground_cmd_test.h"
#include "eps_test_utils.h"

#include <eps_app.h>
#include <eps_msgids.h>
#include <eps_msgdefs.h>

#include <eps_events.h>
#include <eps_app.h>
#include <cfe_evs.h>

#include <uttest.h>
#include <utassert.h>
#include <ut_cfe_sb_hooks.h>
#include <ut_cfe_evs_hooks.h>
#include <ut_cfe_es_stubs.h>
#include <ut_cfe_sb_stubs.h>

#include <stdio.h>

int SEND_message_sent;

/* test noop cmd */
static void EPS_Ground_Cmd_Test_NOOP(void)
{
    /* init cmd counters */
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    /* init noop cmd */
    EPS_no_args_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_no_args_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_NOOP_CC);

    /* process cmd */
    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();
    
    /* test cmd counters */
    UtAssert_True(EPS_AppData.hk.cmd_counters.success == 21,
                  "eps cmd count success");
    UtAssert_True(EPS_AppData.hk.cmd_counters.error == 30,
                  "eps cmd count error");
}

/* test reset counters cmd */
static void EPS_Ground_Cmd_Test_RESET_COUNTERS(void)
{
    /* init cmd counters */
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    /* init reset counters cmd */
    EPS_no_args_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_no_args_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_RESET_COUNTERS_CC);

    /* process cmd */
    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();
    
    /* test cmd counters */
    UtAssert_True(EPS_AppData.hk.cmd_counters.success == 0,
                  "eps cmd count success");
    UtAssert_True(EPS_AppData.hk.cmd_counters.error == 0,
                  "eps cmd count error");
}

/* test set watchdog timeout period cmd */
static void EPS_Ground_Cmd_Test_SET_COM_WATCHDOG_PERIOD(void)
{
    /* init cmd counters */
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    /* expected counter values */
    unsigned int exp_cmd_count_success = 20;
    unsigned int exp_cmd_count_error = 30;

    /* valid timeouts */
    uint16_t timeouts[] = {1, 10, 90};
    int i;
    for(i = 0; i < 3; i++)
    {
        /* init watchdog timeout period cmd */
        EPS_cmd_t cmd;
        Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_cmd_t), TRUE);
        Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_SET_COM_WATCHDOG_PERIOD_CC);
        cmd.data = timeouts[i];

        /* process cmd */
        EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
        EPS_process_command_packet();

        /* update expected counter values */
        exp_cmd_count_success += 1;

        /* test i2c cmd */
        UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                      "eps set com watchdog period cmd sent");
        UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_SET_COM_WATCHDOG_PERIOD,
                      "eps set com watchdog period cmd");
        UtAssert_True(eps_i2c_data.txbuf[1] == timeouts[i],
                      "eps set com watchdog period timeout");

        /* test cmd counters */
        UtAssert_True(EPS_AppData.hk.cmd_counters.success == exp_cmd_count_success,
                      "eps cmd count success");
        UtAssert_True(EPS_AppData.hk.cmd_counters.error == exp_cmd_count_error,
                      "eps cmd count error");

        /* reset i2c data */
        memset(&eps_i2c_data, 0, sizeof(i2c_data_t));
        eps_i2c_data.retcode = E_NO_ERR;
    }

    /* invalid timeouts */
    uint16_t invalid_timeouts[] = {0, 91};
    for(i = 0; i < 2; i++)
    {
        /* init watchdog timeout period cmd */
        EPS_cmd_t cmd;
        Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_cmd_t), TRUE);
        Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_SET_COM_WATCHDOG_PERIOD_CC);
        cmd.data = invalid_timeouts[i];

        /* process cmd */
        EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
        EPS_process_command_packet();

        /* update expected counter values */
        exp_cmd_count_error += 1;

        /* test i2c cmd */
        UtAssert_True(eps_i2c_data.cmd_cnt == 0,
                      "eps set com watchdog period cmd not sent");

        /* test cmd counters */
        UtAssert_True(EPS_AppData.hk.cmd_counters.success == exp_cmd_count_success,
                      "eps cmd count success");
        UtAssert_True(EPS_AppData.hk.cmd_counters.error == exp_cmd_count_error,
                      "eps cmd count error");

        /* reset i2c data */
        memset(&eps_i2c_data, 0, sizeof(i2c_data_t));
        eps_i2c_data.retcode = E_NO_ERR;
    }
}

/* test reset watchdog cmd */
static void EPS_Ground_Cmd_Test_RESET_COM_WATCHDOG(void)
{
    /* init cmd counters */
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    /* init reset watchdog timeout cmd */
    EPS_no_args_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_no_args_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_RESET_COM_WATCHDOG_CC);

    /* process cmd */
    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    /* test i2c cmd */
    UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                  "eps reset com watchdog cmd sent");
    UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_RESET_COM_WATCHDOG,
                  "eps reset com watchdog cmd");
    UtAssert_True(eps_i2c_data.txbuf[1] == 0,
                  "eps reset com watchdog data");

    /* test cmd counters */
    UtAssert_True(EPS_AppData.hk.cmd_counters.success == 21,
                  "eps cmd count success");
    UtAssert_True(EPS_AppData.hk.cmd_counters.error == 30,
                  "eps cmd count error");
}

/* test switch on all pdm cmd */
static void EPS_Ground_Cmd_Test_SWITCH_ON_ALL_PDM(void)
{
    /* init cmd counters */
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    /* init switch on all pdm cmd */
    EPS_no_args_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_no_args_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_SWITCH_ON_ALL_PDM_CC);

    /* process cmd */
    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    /* test i2c cmd */
    UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                  "eps switch all pdm on cmd sent");
    UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_SWITCH_ON_ALL_PDM,
                  "eps switch all pdm on cmd");
    UtAssert_True(eps_i2c_data.txbuf[1] == 0,
                  "eps switch all pdm on data");

    /* test cmd counters */
    UtAssert_True(EPS_AppData.hk.cmd_counters.success == 21,
                  "eps cmd count success");
    UtAssert_True(EPS_AppData.hk.cmd_counters.error == 30,
                  "eps cmd count error");
}

/* test switch off all pdm cmd */
static void EPS_Ground_Cmd_Test_SWITCH_OFF_ALL_PDM(void)
{
    /* init cmd counters */
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    /* init switch off all pdm cmd */
    EPS_no_args_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_no_args_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_SWITCH_OFF_ALL_PDM_CC);

    /* process cmd */
    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    /* test i2c cmd */
    UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                  "eps switch all pdm off cmd sent");
    UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_SWITCH_OFF_ALL_PDM,
                  "eps switch all pdm off cmd");
    UtAssert_True(eps_i2c_data.txbuf[1] == 0,
                  "eps switch all pdm off data");

    /* test cmd counters */
    UtAssert_True(EPS_AppData.hk.cmd_counters.success == 21,
                  "eps cmd count success");
    UtAssert_True(EPS_AppData.hk.cmd_counters.error == 30,
                  "eps cmd count error");
}

/* test set all pdm to initial state cmd */
static void EPS_Ground_Cmd_Test_SET_ALL_PDM_INIT_STATE(void)
{
    /* init cmd counters */
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    /* init set all pdm to initial state cmd */
    EPS_no_args_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_no_args_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_SET_ALL_PDM_TO_INIT_STATE_CC);

    /* process cmd */
    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    /* test i2c cmd */
    UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                  "eps set all pdm to init state cmd sent");
    UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_SET_ALL_PDM_TO_INIT_STATE,
                  "eps set all pdm to init state cmd");
    UtAssert_True(eps_i2c_data.txbuf[1] == 0,
                  "eps set all pdm to init state data");

    /* test cmd counters */
    UtAssert_True(EPS_AppData.hk.cmd_counters.success == 21,
                  "eps cmd count success");
    UtAssert_True(EPS_AppData.hk.cmd_counters.error == 30,
                  "eps cmd count error");
}

/* test switch on pdm cmd */
static void EPS_Ground_Cmd_Test_SWITCH_PDM_N_ON(void)
{
    /* init cmd counters */
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    /* expected counter values */
    unsigned int exp_cmd_count_success = 20;
    unsigned int exp_cmd_count_error = 30;

    /* test valid/invalid pdm channels */
    int i;
    for(i = 0; i < 15; i++)
    {
        /* init switch on pdm cmd */
        EPS_cmd_t cmd;
        Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_cmd_t), TRUE);
        Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_SWITCH_PDM_N_ON_CC);
        cmd.data = i;

        /* process cmd */
        EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
        EPS_process_command_packet();

        /* valid channels */
        if((i >= 1) && (i <= 10))
        {
            exp_cmd_count_success += 1;

            /* test i2c cmd */
            UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                          "eps switch pdm on cmd sent");
            UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_SWITCH_PDM_N_ON,
                          "eps switch pdm on cmd");
            UtAssert_True(eps_i2c_data.txbuf[1] == i,
                          "eps switch pdm on data");
        }
        else
        {
            exp_cmd_count_error += 1;
        }


        /* test cmd counters */
        UtAssert_True(EPS_AppData.hk.cmd_counters.success == exp_cmd_count_success,
                      "eps cmd count success");
        UtAssert_True(EPS_AppData.hk.cmd_counters.error == exp_cmd_count_error,
                      "eps cmd count error");

        /* reset i2c data */
        memset(&eps_i2c_data, 0, sizeof(i2c_data_t));
        eps_i2c_data.retcode = E_NO_ERR;
    }
}

/* test switch off pdm cmd */
static void EPS_Ground_Cmd_Test_SWITCH_PDM_N_OFF(void)
{
    /* init cmd counters */
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    /* expected counter values */
    unsigned int exp_cmd_count_success = 20;
    unsigned int exp_cmd_count_error = 30;

    /* test valid/invalid pdm channels */
    int i;
    for(i = 0; i < 15; i++)
    {
        /* init switch off pdm cmd */
        EPS_cmd_t cmd;
        Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_cmd_t), TRUE);
        Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_SWITCH_PDM_N_OFF_CC);
        cmd.data = i;

        /* process cmd */
        EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
        EPS_process_command_packet();

        /* valid channels */
        if((i >= 1) && (i <= 10))
        {
            exp_cmd_count_success += 1;

            /* test i2c cmd */
            UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                          "eps switch pdm off cmd sent");
            UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_SWITCH_PDM_N_OFF,
                          "eps switch pdm off cmd");
            UtAssert_True(eps_i2c_data.txbuf[1] == i,
                          "eps switch pdm off data");
        }
        else
        {
            exp_cmd_count_error += 1;
        }


        /* test cmd counters */
        UtAssert_True(EPS_AppData.hk.cmd_counters.success == exp_cmd_count_success,
                      "eps cmd count success");
        UtAssert_True(EPS_AppData.hk.cmd_counters.error == exp_cmd_count_error,
                      "eps cmd count error");

        /* reset i2c data */
        memset(&eps_i2c_data, 0, sizeof(i2c_data_t));
        eps_i2c_data.retcode = E_NO_ERR;
    }
}

/* test set pdm initial state on cmd */
static void EPS_Ground_Cmd_Test_SET_PDM_N_INIT_STATE_ON(void)
{
    /* init cmd counters */
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    /* expected counter values */
    unsigned int exp_cmd_count_success = 20;
    unsigned int exp_cmd_count_error = 30;

    /* test valid/invalid pdm channels */
    int i;
    for(i = 0; i < 15; i++)
    {
        /* init set pdm n init state on cmd */
        EPS_cmd_t cmd;
        Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_cmd_t), TRUE);
        Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_SET_PDM_N_INIT_STATE_ON_CC);
        cmd.data = i;

        /* process cmd */
        EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
        EPS_process_command_packet();

        /* valid channels */
        if((i >= 1) && (i <= 10))
        {
            exp_cmd_count_success += 1;

            /* test i2c cmd */
            UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                          "eps set pdm init state on cmd sent");
            UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_SET_PDM_N_INIT_STATE_ON,
                          "eps set pdm init state on cmd");
            UtAssert_True(eps_i2c_data.txbuf[1] == i,
                          "eps set pdm init state on data");
        }
        else
        {
            exp_cmd_count_error += 1;
        }


        /* test cmd counters */
        UtAssert_True(EPS_AppData.hk.cmd_counters.success == exp_cmd_count_success,
                      "eps cmd count success");
        UtAssert_True(EPS_AppData.hk.cmd_counters.error == exp_cmd_count_error,
                      "eps cmd count error");

        /* reset i2c data */
        memset(&eps_i2c_data, 0, sizeof(i2c_data_t));
        eps_i2c_data.retcode = E_NO_ERR;
    }
}

/* test set pdm initial state off cmd */
static void EPS_Ground_Cmd_Test_SET_PDM_N_INIT_STATE_OFF(void)
{
    /* init cmd counters */
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    /* expected counter values */
    unsigned int exp_cmd_count_success = 20;
    unsigned int exp_cmd_count_error = 30;

    /* test valid/invalid pdm channels */
    int i;
    for(i = 0; i < 15; i++)
    {
        /* init set pdm n init state off cmd */
        EPS_cmd_t cmd;
        Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_cmd_t), TRUE);
        Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_SET_PDM_N_INIT_STATE_OFF_CC);
        cmd.data = i;

        /* process cmd */
        EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
        EPS_process_command_packet();


        /* valid channels */
        if((i >= 1) && (i <= 10))
        {
            exp_cmd_count_success += 1;

            /* test i2c cmd */
            UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                          "eps set pdm init state off cmd sent");
            UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_SET_PDM_N_INIT_STATE_OFF,
                          "eps set pdm init state off cmd");
            UtAssert_True(eps_i2c_data.txbuf[1] == i,
                          "eps set pdm init state off data");
        }
        else
        {
            exp_cmd_count_error += 1;
        }


        /* test cmd counters */
        UtAssert_True(EPS_AppData.hk.cmd_counters.success == exp_cmd_count_success,
                      "eps cmd count success");
        UtAssert_True(EPS_AppData.hk.cmd_counters.error == exp_cmd_count_error,
                      "eps cmd count error");

        /* reset i2c data */
        memset(&eps_i2c_data, 0, sizeof(i2c_data_t));
        eps_i2c_data.retcode = E_NO_ERR;
    }
}

/* test set pdm timer limit cmd */
static void EPS_Ground_Cmd_Test_SET_PDM_N_TIMER_LIMIT(void)
{
    /* init cmd counters */
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    /* expected counter values */
    unsigned int exp_cmd_count_success = 20;
    unsigned int exp_cmd_count_error = 30;

    /* test valid/invalid pdm channels */
    int i;
    for(i = 0; i < 15; i++)
    {
        /* test valid/invalid timer limits */
        int j;
        for(j = 0; j < 0xff; j++)
        {
            /* init set pdm n timer limit cmd */
            EPS_cmd_t cmd;
            Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_cmd_t), TRUE);
            Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_SET_PDM_N_TIMER_LIMIT_CC);
            cmd.data = (i << 8) | j;

            /* process cmd */
            EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
            EPS_process_command_packet();


            /* valid channels and limits */
            if((i < 1) || (i > 10))
            {
                exp_cmd_count_error += 1;
            }
            else if((j != 0) && (j < 0x0a))
            {
                exp_cmd_count_error += 1;
            }
            else
            {
                exp_cmd_count_success += 1;

                /* test i2c cmd */
                UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                              "eps set pdm timer limit cmd sent");
                UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_SET_PDM_N_TIMER_LIMIT,
                              "eps set pdm timer limit cmd");
                UtAssert_True(eps_i2c_data.txbuf[1] == i,
                              "eps set pdm timer limit data");
            }

            /* test cmd counters */
            UtAssert_True(EPS_AppData.hk.cmd_counters.success == exp_cmd_count_success,
                          "eps cmd count success");
            UtAssert_True(EPS_AppData.hk.cmd_counters.error == exp_cmd_count_error,
                          "eps cmd count error");

            /* reset i2c data */
            memset(&eps_i2c_data, 0, sizeof(i2c_data_t));
            eps_i2c_data.retcode = E_NO_ERR;
        }
    }
}

/* test reset pcm channel cmd */
static void EPS_Ground_Cmd_Test_PCM_RESET(void)
{
    /* init cmd counters */
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    /* expected counter values */
    unsigned int exp_cmd_count_success = 20;
    unsigned int exp_cmd_count_error = 30;

    /* test valid pcm channels */
    int i;
    for(i = 0; i < 16; i++)
    {
        /* init reset pcm cmd */
        EPS_cmd_t cmd;
        Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_cmd_t), TRUE);
        Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_PCM_RESET_CC);
        cmd.data = i;

        /* process cmd */
        EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
        EPS_process_command_packet();


        /* valid channels */
//        if((i >= 0x01) && (i <= 0x0f))
//        {
        exp_cmd_count_success += 1;

            /* test i2c cmd */
        UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                      "eps pcm reset cmd sent");
        UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_PCM_RESET,
                      "eps pcm reset cmd");
        UtAssert_True(eps_i2c_data.txbuf[1] == i,
                      "eps pcm reset data");
//        }
//        else
//        {
//            exp_cmd_count_error += 1;
            //       }


        /* test cmd counters */
        UtAssert_True(EPS_AppData.hk.cmd_counters.success == exp_cmd_count_success,
                      "eps cmd count success");
        UtAssert_True(EPS_AppData.hk.cmd_counters.error == exp_cmd_count_error,
                      "eps cmd count error");

        /* reset i2c data */
        memset(&eps_i2c_data, 0, sizeof(i2c_data_t));
        eps_i2c_data.retcode = E_NO_ERR;
    }
}

/* test manual reset cmd */
static void EPS_Ground_Cmd_Test_MANUAL_RESET(void)
{
    /* init cmd counters */
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    /* init manual reset cmd */
    EPS_no_args_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_no_args_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_MANUAL_RESET_CC);

    /* process cmd */
    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    /* test i2c cmd */
    UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                  "eps manual reset cmd sent");
    UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_MANUAL_RESET,
                  "eps manual reset cmd");
    UtAssert_True(eps_i2c_data.txbuf[1] == 0,
                  "eps manual reset data");

    /* test cmd counters */
    UtAssert_True(EPS_AppData.hk.cmd_counters.success == 21,
                  "eps cmd count success");
    UtAssert_True(EPS_AppData.hk.cmd_counters.error == 30,
                  "eps cmd count error");
}

static void EPS_Ground_Cmd_Test_GET_BOARD_STATUS(void)
{
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    EPS_no_args_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_no_args_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_GET_BOARD_STATUS_CC);

    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                  "eps manual reset cmd sent");
    UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_GET_BOARD_STATUS,
                  "eps manual reset cmd");
    UtAssert_True(eps_i2c_data.txbuf[1] == 0,
                  "eps manual reset data");

    /* test cmd counters */
    //TODO:  Verify that this should have an incrementing counter?
    UtAssert_True(EPS_AppData.hk.cmd_counters.success == 21,
                  "eps cmd count success");
    UtAssert_True(EPS_AppData.hk.cmd_counters.error == 30,
                  "eps cmd count error");
    UtAssert_EventIDSent(EPS_CMD_GET_BOARD_STATUS_EID, CFE_EVS_ERROR, "GET BOARD STATUS Event Sent");
}

static void EPS_Ground_Cmd_Test_GET_LAST_ERROR(void)
{
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    EPS_no_args_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_no_args_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_GET_LAST_ERROR_CC);

    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                  "eps manual reset cmd sent");
    UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_GET_LAST_ERROR,
                  "eps manual reset cmd");
    UtAssert_True(eps_i2c_data.txbuf[1] == 0,
                  "eps manual reset data");

    /* test cmd counters */
    //TODO:  Verify that this should have an incrementing counter?
    UtAssert_True(EPS_AppData.hk.cmd_counters.success == 21,
                  "eps cmd count success");
    UtAssert_True(EPS_AppData.hk.cmd_counters.error == 30,
                  "eps cmd count error");
    UtAssert_EventIDSent(EPS_CMD_GET_LAST_ERROR_EID, CFE_EVS_ERROR, "GET LAST ERROR Event Sent");
}

static void EPS_Ground_Cmd_Test_GET_VERSION(void)
{
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    EPS_no_args_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_no_args_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_GET_VERSION_CC);

    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                  "eps manual reset cmd sent");
    UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_GET_VERSION,
                  "eps manual reset cmd");
    UtAssert_True(eps_i2c_data.txbuf[1] == 0,
                  "eps manual reset data");

    /* test cmd counters */
    //TODO:  Verify that this should have an incrementing counter?
    UtAssert_True(EPS_AppData.hk.cmd_counters.success == 21,
                  "eps cmd count success");
    UtAssert_True(EPS_AppData.hk.cmd_counters.error == 30,
                  "eps cmd count error");
    UtAssert_EventIDSent(EPS_CMD_GET_VERSION_EID, CFE_EVS_ERROR, "GET VERSION Event Sent");
}

static void EPS_Ground_Cmd_Test_GET_CHECKSUM(void)
{
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    EPS_no_args_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_no_args_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_GET_CHECKSUM_CC);

    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                  "eps manual reset cmd sent");
    UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_GET_CHECKSUM,
                  "eps manual reset cmd");
    UtAssert_True(eps_i2c_data.txbuf[1] == 0,
                  "eps manual reset data");

    /* test cmd counters */
    //TODO:  Verify that this should have an incrementing counter?
    UtAssert_True(EPS_AppData.hk.cmd_counters.success == 21,
                  "eps cmd count success");
    UtAssert_True(EPS_AppData.hk.cmd_counters.error == 30,
                  "eps cmd count error");
    UtAssert_EventIDSent(EPS_CMD_GET_CHECKSUM_EID, CFE_EVS_ERROR,  "GET CHECKSUM Event Sent");
}

static void EPS_Ground_Cmd_Test_GET_TELEMETRY(void)
{
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    EPS_no_args_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_no_args_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_GET_TELEMETRY_CC);

    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                  "eps manual reset cmd sent");
    UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_GET_TELEMETRY,
                  "eps manual reset cmd");
    UtAssert_True(eps_i2c_data.txbuf[1] == 0,
                  "eps manual reset data");

    /* test cmd counters */
    //TODO:  Verify that this should have an incrementing counter?
    UtAssert_True(EPS_AppData.hk.cmd_counters.success == 21,
                  "eps cmd count success");
    UtAssert_True(EPS_AppData.hk.cmd_counters.error == 30,
                  "eps cmd count error");
    UtAssert_EventIDSent(EPS_CMD_GET_TELEMETRY_EID, CFE_EVS_ERROR,  "GET TELEMETRY Event Sent");
}

static void EPS_Ground_Cmd_Test_GET_WATCHDOG(void)
{
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    EPS_no_args_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_no_args_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_GET_COM_WATCHDOG_PERIOD_CC);

    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                  "eps manual reset cmd sent");
    UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_GET_COM_WATCHDOG_PERIOD,
                  "eps manual reset cmd");
    UtAssert_True(eps_i2c_data.txbuf[1] == 0,
                  "eps manual reset data");

    /* test cmd counters */
    UtAssert_True(EPS_AppData.hk.cmd_counters.success == 21, "eps cmd count success"); //TODO:  Timer didnt' increment?
    UtAssert_True(EPS_AppData.hk.cmd_counters.error == 30, "eps cmd count error");
    UtAssert_EventIDSent(EPS_CMD_GET_COM_WATCHDOG_PERIOD_EID, CFE_EVS_ERROR, "GET WATCHDOG Event Sent");
}

static void EPS_Ground_Cmd_Test_GET_NUMBER_OF_BOR(void)
{
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    EPS_no_args_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_no_args_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_GET_NUMBER_OF_BOR_CC);

    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                  "eps manual reset cmd sent");
    UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_GET_NUMBER_OF_BOR,
                  "eps manual reset cmd");
    UtAssert_True(eps_i2c_data.txbuf[1] == 0,
                  "eps manual reset data");

    /* test cmd counters */
    //TODO:  Verify that this should have an incrementing counter?
    UtAssert_True(EPS_AppData.hk.cmd_counters.success == 21,
                  "eps cmd count success");
    UtAssert_True(EPS_AppData.hk.cmd_counters.error == 30,
                  "eps cmd count error");
    UtAssert_EventIDSent(EPS_CMD_GET_NUMBER_OF_BOR_EID, CFE_EVS_ERROR, "GET NUMBER OF BOR Event Sent");
}

static void EPS_Ground_Cmd_Test_GET_NUMBER_OF_ASR(void)
{
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    EPS_no_args_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_no_args_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_GET_NUMBER_OF_ASR_CC);

    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                  "eps manual reset cmd sent");
    UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_GET_NUMBER_OF_ASR,
                  "eps manual reset cmd");
    UtAssert_True(eps_i2c_data.txbuf[1] == 0,
                  "eps manual reset data");

    /* test cmd counters */
    //TODO:  Verify that this should have an incrementing counter?
    UtAssert_True(EPS_AppData.hk.cmd_counters.success == 21,
                  "eps cmd count success");
    UtAssert_True(EPS_AppData.hk.cmd_counters.error == 30,
                  "eps cmd count error");
    UtAssert_EventIDSent(EPS_CMD_GET_NUMBER_OF_ASR_EID, CFE_EVS_ERROR, "GET NUMBER OF ASR Event Sent");
}

static void EPS_Ground_Cmd_Test_GET_NUMBER_OF_MR(void)
{
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    EPS_no_args_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_no_args_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_GET_NUMBER_OF_MR_CC);

    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                  "eps manual reset cmd sent");
    UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_GET_NUMBER_OF_ASR,
                  "eps manual reset cmd");
    UtAssert_True(eps_i2c_data.txbuf[1] == 0,
                  "eps manual reset data");

    /* test cmd counters */
    //TODO:  Verify that this should have an incrementing counter?
    UtAssert_True(EPS_AppData.hk.cmd_counters.success == 21,
                  "eps cmd count success");
    UtAssert_True(EPS_AppData.hk.cmd_counters.error == 30,
                  "eps cmd count error");
    UtAssert_EventIDSent(EPS_CMD_GET_NUMBER_OF_MR_EID, CFE_EVS_ERROR, "GET NUMBER OF MR Event Sent");
}

static void EPS_Ground_Cmd_Test_GET_NUMBER_OF_CWDR(void)
{
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    EPS_no_args_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_no_args_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_GET_NUMBER_OF_CWDR_CC);

    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                  "eps manual reset cmd sent");
    UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_GET_NUMBER_OF_CWDR,
                  "eps manual reset cmd");
    UtAssert_True(eps_i2c_data.txbuf[1] == 0,
                  "eps manual reset data");

    /* test cmd counters */
    //TODO:  Verify that this should have an incrementing counter?
    UtAssert_True(EPS_AppData.hk.cmd_counters.success == 21,
                  "eps cmd count success");
    UtAssert_True(EPS_AppData.hk.cmd_counters.error == 30,
                  "eps cmd count error");
    UtAssert_EventIDSent(EPS_CMD_GET_NUMBER_OF_CWDR_EID, CFE_EVS_ERROR, "GET NUMBER OF CWDR Event Sent");
}

static void EPS_Ground_Cmd_Test_GET_ACTUAL_STATE_ALL_PDM(void)
{
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    EPS_no_args_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_no_args_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_GET_ACTUAL_STATE_ALL_PDM_CC);

    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                  "eps manual reset cmd sent");
    UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_GET_ACTUAL_STATE_ALL_PDM,
                  "eps manual reset cmd");
    UtAssert_True(eps_i2c_data.txbuf[1] == 0,
                  "eps manual reset data");

    /* test cmd counters */
    //TODO:  Verify that this should have an incrementing counter?
    UtAssert_True(EPS_AppData.hk.cmd_counters.success == 21,
                  "eps cmd count success");
    UtAssert_True(EPS_AppData.hk.cmd_counters.error == 30,
                  "eps cmd count error");
    UtAssert_EventIDSent(EPS_CMD_GET_ACTUAL_STATE_ALL_PDM_EID, CFE_EVS_ERROR, "GET ACTUAL STATE ALL PDM Event Sent");
}

static void EPS_Ground_Cmd_Test_GET_EXPECTED_STATE_ALL_PDM(void)
{
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    EPS_no_args_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_no_args_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_GET_EXPECTED_STATE_ALL_PDM_CC);

    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                  "eps manual reset cmd sent");
    UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_GET_EXPECTED_STATE_ALL_PDM,
                  "eps manual reset cmd");
    UtAssert_True(eps_i2c_data.txbuf[1] == 0,
                  "eps manual reset data");

    /* test cmd counters */
    //TODO:  Verify that this should have an incrementing counter?
    UtAssert_True(EPS_AppData.hk.cmd_counters.success == 21,
                  "eps cmd count success");
    UtAssert_True(EPS_AppData.hk.cmd_counters.error == 30,
                  "eps cmd count error");
    UtAssert_EventIDSent(EPS_CMD_GET_EXPECTED_STATE_ALL_PDM_EID, CFE_EVS_ERROR, "GET EXPECTED STATE ALL PDM Event Sent");
}

static void EPS_Ground_Cmd_Test_GET_INIT_STATE_ALL_PDM(void)
{
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    EPS_no_args_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_no_args_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_GET_INIT_STATE_ALL_PDM_CC);

    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                  "eps manual reset cmd sent");
    UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_GET_INIT_STATE_ALL_PDM,
                  "eps manual reset cmd");
    UtAssert_True(eps_i2c_data.txbuf[1] == 0,
                  "eps manual reset data");

    /* test cmd counters */
    //TODO:  Verify that this should have an incrementing counter?
    UtAssert_True(EPS_AppData.hk.cmd_counters.success == 21,
                  "eps cmd count success");
    UtAssert_True(EPS_AppData.hk.cmd_counters.error == 30,
                  "eps cmd count error");
    UtAssert_EventIDSent(EPS_CMD_GET_INIT_STATE_ALL_PDM_EID, CFE_EVS_ERROR, "GET INIT STATE ALL PDM Event Sent");
}

static void EPS_Ground_Cmd_Test_GET_PDM_N_ACTUAL_STATUS(void)
{
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    EPS_no_args_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_no_args_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_GET_PDM_N_ACTUAL_STATUS_CC);

    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                  "eps manual reset cmd sent");
    UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_GET_PDM_N_ACTUAL_STATUS,
                  "eps manual reset cmd");
    UtAssert_True(eps_i2c_data.txbuf[1] == 0,
                  "eps manual reset data");

    /* test cmd counters */
    //TODO:  Verify that this should have an incrementing counter?
    UtAssert_True(EPS_AppData.hk.cmd_counters.success == 21,
                  "eps cmd count success");
    UtAssert_True(EPS_AppData.hk.cmd_counters.error == 30,
                  "eps cmd count error");
    UtAssert_EventIDSent(EPS_CMD_GET_PDM_N_ACTUAL_STATUS_EID, CFE_EVS_ERROR, "GET PDM N ACTUAL STATUS Event Sent");
}

static void EPS_Ground_Cmd_Test_GET_PDM_N_TIMER_LIMIT(void)
{
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    EPS_no_args_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_no_args_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_GET_PDM_N_TIMER_LIMIT_CC);

    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                  "eps manual reset cmd sent");
    UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_GET_PDM_N_TIMER_LIMIT,
                  "eps manual reset cmd");
    UtAssert_True(eps_i2c_data.txbuf[1] == 0,
                  "eps manual reset data");

    /* test cmd counters */
    //TODO:  Verify that this should have an incrementing counter?
    UtAssert_True(EPS_AppData.hk.cmd_counters.success == 21,
                  "eps cmd count success");
    UtAssert_True(EPS_AppData.hk.cmd_counters.error == 30,
                  "eps cmd count error");
    UtAssert_EventIDSent(EPS_CMD_RESET_EID, CFE_EVS_ERROR, "GET PDM N TIMER LIMIT Event Sent");
}

static void EPS_Ground_Cmd_Test_GET_PDM_N_CURRENT_TIMER(void)
{
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    EPS_no_args_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_no_args_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_GET_PDM_N_CURRENT_TIMER_VAL_CC);

    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    UtAssert_True(eps_i2c_data.cmd_cnt == 1,
                  "eps manual reset cmd sent");
    UtAssert_True(eps_i2c_data.txbuf[0] == TEST_EPS_CMD_GET_PDM_N_CURRENT_TIMER_VAL,
                  "eps manual reset cmd");
    UtAssert_True(eps_i2c_data.txbuf[1] == 0,
                  "eps manual reset data");

    /* test cmd counters */
    //TODO:  Verify that this should have an incrementing counter?
    UtAssert_True(EPS_AppData.hk.cmd_counters.success == 21,
                  "eps cmd count success");
    UtAssert_True(EPS_AppData.hk.cmd_counters.error == 30,
                  "eps cmd count error");
    UtAssert_EventIDSent(EPS_CMD_GET_PDM_N_CURRENT_TIMER_VAL_EID, CFE_EVS_ERROR, "GET PDM N CURRENT TIMER Event Sent");
}

static void BAT_Ground_Cmd_Test_SET_HEATER_CONTROLLER_STATUS(void)
{
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    BAT_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(BAT_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, BAT_CMD_SET_HEATER_CONTROLLER_STATUS_CC);

    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    /* test cmd counters */
    UtAssert_True(EPS_AppData.hk.cmd_counters.success == 21,
                  "eps cmd count success");
    UtAssert_True(EPS_AppData.hk.cmd_counters.error == 30,
                  "eps cmd count error");
    UtAssert_EventIDSent(BAT_CMD_SET_HEATER_CONTROLLER_STATUS_EID, CFE_EVS_INFORMATION, "BAT SET HEATER CONTROLLER STATUS Event Sent");
}

static void BAT_Ground_Cmd_Test_MANUAL_RESET(void)
{
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    BAT_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(BAT_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, BAT_CMD_MANUAL_RESET_CC);

    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    /* test cmd counters */
    UtAssert_True(EPS_AppData.hk.cmd_counters.success == 21,
                  "eps cmd count success");
    UtAssert_True(EPS_AppData.hk.cmd_counters.error == 30,
                  "eps cmd count error");
    UtAssert_EventIDSent(BAT_CMD_MANUAL_RESET_EID, CFE_EVS_INFORMATION, "BAT MANUAL RESET Event Sent");
}

//TODO:  ---Should these become implemented
     /*case BAT_CMD_GET_LAST_ERROR_CC:
     case BAT_CMD_GET_VERSION_CC:
     case BAT_CMD_GET_CHECKSUM_CC:
     case BAT_CMD_GET_TELEMETRY_CC:
     case BAT_CMD_GET_NUMBER_OF_BOR_CC:
     case BAT_CMD_GET_NUMBER_OF_ASR_CC:
     case BAT_CMD_GET_NUMBER_OF_MR_CC:
     case BAT_CMD_GET_HEATER_CONTROLLER_STATUS_CC:*/

static void BAT_Ground_Cmd_Test_MULTIPLE_UNIMPLEMENTED(void)
{

    BAT_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(BAT_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, BAT_CMD_GET_BOARD_STATUS_CC);

    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    UtAssert_EventIDSent(EPS_CMD_ERR_EID, CFE_EVS_ERROR,  "BAT Event Sent");
}

static void EPS_Ground_Cmd_Test_DEFAULT(void)
{
    EPS_AppData.hk.cmd_counters.success = 20;
    EPS_AppData.hk.cmd_counters.error = 30;

    EPS_no_args_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_CMD_MID, sizeof(EPS_no_args_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, -9999);

    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    UtAssert_True(EPS_AppData.hk.cmd_counters.success == 20, "eps cmd count success");
    UtAssert_True(EPS_AppData.hk.cmd_counters.error == 31, "eps cmd count error");
    UtAssert_EventIDSent(EPS_CMD_ERR_EID, CFE_EVS_ERROR, "ERROR Event Sent");
}

static void EPS_Ground_Cmd_Test_PROCESS_DEFAULT(void)
{
    EPS_no_args_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, -9999, sizeof(EPS_no_args_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_NOOP_CC);

    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    UtAssert_EventIDSent(EPS_CMD_ERR_EID, CFE_EVS_ERROR, "DEFAULT COMMAND PROCESS ERROR Event Sent");
}

//TODO:  This doesn't currently test much.
int32 Ut_CFE_SB_SendMsg_Hook(CFE_SB_Msg_t *MsgPtr)
{
    SEND_message_sent++;
    return CFE_SUCCESS;
}

static void EPS_Ground_Cmd_Test_PROCESS_HOUSEKEEPING(void)
{
    SEND_message_sent = 0;
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_SENDMSG_INDEX, &Ut_CFE_SB_SendMsg_Hook);
    EPS_no_args_cmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, EPS_SEND_HK_MID, sizeof(EPS_no_args_cmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, EPS_CMD_NOOP_CC);

    EPS_AppData.msg_ptr = (CFE_SB_MsgPtr_t)&cmd;
    EPS_process_command_packet();

    UtAssert_True(SEND_message_sent == 1, "Incremented Message - Quick Test that function is called");
}

void EPS_Ground_Cmd_Test_AddTestCases(void)
{
    UtTest_Add(EPS_Ground_Cmd_Test_NOOP, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: NOOP");
    UtTest_Add(EPS_Ground_Cmd_Test_RESET_COUNTERS, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: RESET COUNTERS");
    UtTest_Add(EPS_Ground_Cmd_Test_SET_COM_WATCHDOG_PERIOD, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: SET COM WATCHDOG PERIOD");
    UtTest_Add(EPS_Ground_Cmd_Test_RESET_COM_WATCHDOG, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: RESET COM WATCHDOG");
    UtTest_Add(EPS_Ground_Cmd_Test_SWITCH_ON_ALL_PDM, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: SWITCH ON ALL PDM");
    UtTest_Add(EPS_Ground_Cmd_Test_SWITCH_OFF_ALL_PDM, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: SWITCH OFF ALL PDM");
    UtTest_Add(EPS_Ground_Cmd_Test_SET_ALL_PDM_INIT_STATE, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: SET ALL PDM TO INITIAL STATE");
    UtTest_Add(EPS_Ground_Cmd_Test_SWITCH_PDM_N_ON, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: SWITCH PDM N ON");
    UtTest_Add(EPS_Ground_Cmd_Test_SWITCH_PDM_N_OFF, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: SWITCH PDM N OFF");
    UtTest_Add(EPS_Ground_Cmd_Test_SET_PDM_N_INIT_STATE_ON, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: SET PDM N INITIAL STATE ON");
    UtTest_Add(EPS_Ground_Cmd_Test_SET_PDM_N_INIT_STATE_OFF, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: SET PDM N INITIAL STATE OFF");
    UtTest_Add(EPS_Ground_Cmd_Test_SET_PDM_N_TIMER_LIMIT, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: SET PDM N TIMER LIMIT");
    UtTest_Add(EPS_Ground_Cmd_Test_PCM_RESET, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: PCM RESET");
    UtTest_Add(EPS_Ground_Cmd_Test_MANUAL_RESET, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: MANUAL RESET");
    UtTest_Add(EPS_Ground_Cmd_Test_GET_BOARD_STATUS, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: GET BOARD STATUS");
    UtTest_Add(EPS_Ground_Cmd_Test_GET_LAST_ERROR, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: GET LAST ERROR");
    UtTest_Add(EPS_Ground_Cmd_Test_GET_VERSION, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: GET VERSION");
    UtTest_Add(EPS_Ground_Cmd_Test_GET_CHECKSUM, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: GET CHECKSUM");
    UtTest_Add(EPS_Ground_Cmd_Test_GET_TELEMETRY, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: GET TELEMETRY");
    UtTest_Add(EPS_Ground_Cmd_Test_GET_WATCHDOG, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: GET WATCHDOG");
    UtTest_Add(EPS_Ground_Cmd_Test_GET_NUMBER_OF_BOR, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: GET NUMBER OF BOR");
    UtTest_Add(EPS_Ground_Cmd_Test_GET_NUMBER_OF_ASR, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: GET NUMBER OF ASR");
    UtTest_Add(EPS_Ground_Cmd_Test_GET_NUMBER_OF_MR, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: GET NUMBER OF MR");
    UtTest_Add(EPS_Ground_Cmd_Test_GET_NUMBER_OF_CWDR, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: GET NUMBER OF CWDR");
    UtTest_Add(EPS_Ground_Cmd_Test_GET_ACTUAL_STATE_ALL_PDM, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: GET ACTUAL STATE ALL PDM");
    UtTest_Add(EPS_Ground_Cmd_Test_GET_EXPECTED_STATE_ALL_PDM, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: GET EXPECTED STATE ALL PDM");
    UtTest_Add(EPS_Ground_Cmd_Test_GET_INIT_STATE_ALL_PDM, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: GET INIT STATE ALL PDM");
    UtTest_Add(EPS_Ground_Cmd_Test_GET_PDM_N_ACTUAL_STATUS, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: GET PDM N ACTUAL STATUS");
    UtTest_Add(EPS_Ground_Cmd_Test_GET_PDM_N_TIMER_LIMIT, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: GET PDM N TIMER LIMIT");
    UtTest_Add(EPS_Ground_Cmd_Test_GET_PDM_N_CURRENT_TIMER, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: GET PDM N CURRENT TIMER");
    UtTest_Add(BAT_Ground_Cmd_Test_SET_HEATER_CONTROLLER_STATUS, EPS_Test_Setup, EPS_Test_TearDown,
               "bat ground command: SET HEATER CONTROLLER STATUS");
    UtTest_Add(BAT_Ground_Cmd_Test_MANUAL_RESET, EPS_Test_Setup, EPS_Test_TearDown,
               "bat ground command: MANUAL RESET");
    UtTest_Add(BAT_Ground_Cmd_Test_MULTIPLE_UNIMPLEMENTED, EPS_Test_Setup, EPS_Test_TearDown,
               "bat ground command: MULTIPLE UNIMPLEMENTED");
    UtTest_Add(EPS_Ground_Cmd_Test_DEFAULT, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: DEFAULT CASE");
    UtTest_Add(EPS_Ground_Cmd_Test_PROCESS_DEFAULT, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: PROCESS DEFAULT CASE");
    UtTest_Add(EPS_Ground_Cmd_Test_PROCESS_HOUSEKEEPING, EPS_Test_Setup, EPS_Test_TearDown,
               "eps ground command: PROCESS HOUSEKEEPING CASE");
}