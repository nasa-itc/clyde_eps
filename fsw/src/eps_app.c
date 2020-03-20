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

#include "eps_app.h"
#include "eps_perfids.h"
#include "eps_msgids.h"
#include "eps_msg.h"
#include "eps_msgdefs.h"
#include "eps_events.h"
#include "eps_version.h"
#include "eps_lib.h"
#include "bat_lib.h"

EPS_AppData_t EPS_AppData;

static CFE_EVS_BinFilter_t  EPS_EventFilters[] =
{  /* Event ID      RateLimit BurstLimit */
    {EPS_INIT_EID,                            0x0000},
    {EPS_INIT_ERR_EID,                        0x0000},
    {EPS_ES_ERR_EID,                          0x0000},
    {EPS_SB_ERR_EID,                          0x0000}, 
    {EPS_CMD_ERR_EID,                         0x0000},
    {EPS_CHECKSUM_ERR_EID,                    0x0000},
    {EPS_I2C_ERR_EID,                         0x0000}, 
    {EPS_CMD_NOOP_EID,                        0x0000},
    {EPS_CMD_RESET_EID,                       0x0000},
    {EPS_LEN_ERR_EID,                         0x0000},
    {EPS_HEARTBEAT_ERR_EID,                   0x0000},
    {EPS_CMD_GET_BOARD_STATUS_EID,            0x0000},
    {EPS_CMD_GET_LAST_ERROR_EID,              0x0000},
    {EPS_CMD_GET_VERSION_EID,                 0x0000},
    {EPS_CMD_GET_CHECKSUM_EID,                0x0000},
    {EPS_CMD_GET_TELEMETRY_EID,               0x0000},
    {EPS_CMD_GET_COM_WATCHDOG_PERIOD_EID,     0x0000},
    {EPS_CMD_SET_COM_WATCHDOG_PERIOD_EID,     0x0000},
    {EPS_CMD_RESET_COM_WATCHDOG_EID,          0x0000},
    {EPS_CMD_GET_NUMBER_OF_BOR_EID,           0x0000}, 
    {EPS_CMD_GET_NUMBER_OF_ASR_EID,           0x0000},
    {EPS_CMD_GET_NUMBER_OF_MR_EID,            0x0000},
    {EPS_CMD_GET_NUMBER_OF_CWDR_EID,          0x0000},
    {EPS_CMD_SWITCH_ON_ALL_PDM_EID,           0x0000},
    {EPS_CMD_SWITCH_OFF_ALL_PDM_EID,          0x0000},
    {EPS_CMD_GET_ACTUAL_STATE_ALL_PDM_EID,    0x0000},
    {EPS_CMD_GET_EXPECTED_STATE_ALL_PDM_EID,  0x0000},
    {EPS_CMD_GET_INIT_STATE_ALL_PDM_EID,      0x0000},
    {EPS_CMD_SET_ALL_PDM_TO_INIT_STATE_EID,   0x0000},
    {EPS_CMD_SWITCH_PDM_N_ON_EID,             0x0000},
    {EPS_CMD_SWITCH_PDM_N_OFF_EID,            0x0000},
    {EPS_CMD_SET_PDM_N_INIT_STATE_ON_EID,     0x0000},
    {EPS_CMD_SET_PDM_N_INIT_STATE_OFF_EID,    0x0000},
    {EPS_CMD_GET_PDM_N_ACTUAL_STATUS_EID,     0x0000},
    {EPS_CMD_SET_PDM_N_TIMER_LIMIT_EID,       0x0000},
    {EPS_CMD_GET_PDM_N_TIMER_LIMIT_EID,       0x0000},
    {EPS_CMD_GET_PDM_N_CURRENT_TIMER_VAL_EID, 0x0000},
    {EPS_CMD_PCM_RESET_EID,                   0x0000},
    {EPS_CMD_MANUAL_RESET_EID,                0x0000},
    {BAT_CMD_SET_HEATER_CONTROLLER_STATUS_EID,0x0000},
    {BAT_CMD_MANUAL_RESET_EID,                0x0000},
    {EPSLIB_I2C_ERR_EID,                      0x0000},
    {EPSLIB_TLM_ERR_EID,                      0x0000},
    {EPSLIB_MUTEX_ERR_EID,                    0x0000}
};

void clyde_eps_AppMain(void)
{
    int32 status;
    EPS_AppData.run_status = CFE_ES_APP_RUN;

    /* Create the first performance log entry */
    CFE_ES_PerfLogEntry(EPS_APP_PERF_ID);

    /* 
     * Perform EPS app initialization. If the init fails, set the run_status to 
     * CFE_ES_APP_ERROR and the app will not enter the RunLoop.
     */
    if ( (status = EPS_app_init()) != CFE_SUCCESS)
    {
        OS_printf("EPS: init failed (status: 0x%08X)", status);
        EPS_AppData.run_status = CFE_ES_APP_ERROR;
    }

    /*
     * EPS application main loop. Call CFE_ES_RunLoop to check for changes in 
     * the application status. If there is a request to kill this app, it will
     * be passed in through the RunLoop call. 
     */
    while(CFE_ES_RunLoop(&EPS_AppData.run_status) == TRUE)
    {
        /* Stop perf log entry prior to waiting for software bus message */
        CFE_ES_PerfLogExit(EPS_APP_PERF_ID);

        /* Pend forever on receipt of software bus message */
        status = CFE_SB_RcvMsg(&EPS_AppData.msg_ptr, EPS_AppData.cmd_pipe, CFE_SB_PEND_FOREVER);
        
        /* Performance log entry stamp */
        CFE_ES_PerfLogEntry(EPS_APP_PERF_ID);

        /* Check the return status from the SB */
        if(status == CFE_SUCCESS)
        {
            /*
             * Process SB message. If there are fatal errors in command processing the command can alter
             * the global run_status variable to exit the main event loop. 
             */
            EPS_process_command_packet();
        }      
        else
        {
            CFE_EVS_SendEvent(EPS_SB_ERR_EID, CFE_EVS_ERROR, "EPS: Error - failed to received SB message (status: 0x%08X)", status);
        }
    }

    /* exit EPS app */
    CFE_ES_ExitApp(EPS_AppData.run_status);
}


int32 EPS_app_init(void)
{
    int32 status;

    /* 
     * Register application, allowing cFE to map the task to various executive service
     * resources and OS resources. 
     */

    if ( (status = CFE_ES_RegisterApp()) != CFE_SUCCESS)
    {
        OS_printf("EPS: Error - app failed to register with ES (status: 0x%08X)", status);
        return status;
    }

    /* ===============================================================================
     * Register event filter table with cFE
     * It was decided in the SEN/IMU code review (2016/12/2) that EVS failing should 
     * not cause the app to exit. Therefor status is not set.
     *================================================================================
     */
    if (CFE_EVS_Register(EPS_EventFilters, sizeof(EPS_EventFilters)/sizeof(CFE_EVS_BinFilter_t), CFE_EVS_BINARY_FILTER) != CFE_SUCCESS)
    {
        OS_printf("EPS: Error - app failed to register with EVS (status: 0x%08X)", status);
    }

    /* The EPS application must request the cFE to create a software bus pipe */
    if ( (status = CFE_SB_CreatePipe(&EPS_AppData.cmd_pipe, EPS_PIPE_DEPTH, EPS_PIPE_NAME)) != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(EPS_SB_ERR_EID, CFE_EVS_ERROR, "EPS: Error - failed to create pipe (status: 0x%08X)", status);
        return status;
    }

    /* 
     * SB Message Subscription. The EPS app must inform cFE which SB messages it expects to 
     * receive and on which pipe. 
     */
    if ( (status = CFE_SB_Subscribe(EPS_CMD_MID, EPS_AppData.cmd_pipe)) != CFE_SUCCESS) /*Ground Command Packets*/
    {
        CFE_EVS_SendEvent(EPS_SB_ERR_EID, CFE_EVS_ERROR, "EPS: Error - failed to subscribe to EPS_CMD_MID (status: 0x%08X)", status);
        return status;
    }

    if ( (status = CFE_SB_Subscribe(EPS_SEND_HK_MID, EPS_AppData.cmd_pipe)) != CFE_SUCCESS) /*Housekeeping (HK) requests*/
    {
        CFE_EVS_SendEvent(EPS_SB_ERR_EID, CFE_EVS_ERROR, "EPS: Error - failed to subscribe to EPS_SEND_HK_MID (status: 0x%08X)", status);
        return status;
    }

    if ( (status = CFE_SB_Subscribe(EPS_HEARTBEAT_MID, EPS_AppData.cmd_pipe)) != CFE_SUCCESS) /*Heartbeat sent by MGR*/
    {
        CFE_EVS_SendEvent(EPS_SB_ERR_EID, CFE_EVS_ERROR, "EPS: Error, failed to subscribe to EPS_HEARTBEAT_MID (status: 0x%08X)", status);
        return status;
    }

    /* 
     * Init housekeeping packet (clear user data area). SB messages generated by the EPS must be 
     * initialized so they contain appropriate header information and are formatted properly. 
     */
    CFE_SB_InitMsg(&EPS_AppData.hk, EPS_HK_TLM_MID, EPS_HK_TLM_LNGTH, TRUE);

    /* read all initial static hardware info (no need to read these static values periodically) */
    EPS_get_version(&EPS_AppData.hk.version);
    BAT_get_version(&EPS_AppData.hk.battery[BAT_CS05602].version, BAT_CS05602);
    BAT_get_version(&EPS_AppData.hk.battery[BAT_CS05876].version, BAT_CS05876);

    /*reset heartbeat counters*/
    EPS_AppData.hk_packets_requested = 0;
    EPS_AppData.heartbeat_counter = 0;

    /* EPS app startup event message. Notify operators that the task has initialized. */
    CFE_EVS_SendEvent(EPS_INIT_EID, CFE_EVS_INFORMATION,
                      "EPS Initialized. Version %d.%d.%d.%d",
                      EPS_MAJOR_VERSION,
                      EPS_MINOR_VERSION, 
                      EPS_REVISION, 
                      EPS_MISSION_REV);

    return status;
}


void EPS_process_command_packet(void)
{
    /* 
     * cFE SB functions are used to extract the message ID. This way, the cFE is not required
     * to use the same message header structure for all missions and platforms. 
     */
    CFE_SB_MsgId_t msg_id = CFE_SB_GetMsgId(EPS_AppData.msg_ptr);
    switch(msg_id)
    {

    case EPS_HEARTBEAT_MID:
        EPS_AppData.heartbeat_counter++;
        break;

        /* process ground commands */
    case EPS_CMD_MID:
        EPS_process_ground_command();
        break;

        /* request to send EPS housekeeping (HK) data */
    case EPS_SEND_HK_MID:
        EPS_AppData.hk_packets_requested++;

        //If the difference between the EPS hk packets requested and the MGR heartbeat are greater
        //than 1 then we aren't receiving heartbeats from MGR and/or Cadet. The buffer of 1 was 
        //added to allow things to settle at startup. Due to the order of the schedule EPS HK is
        //currently requested before MGR or Cadet.
        if ((EPS_AppData.hk_packets_requested - EPS_AppData.heartbeat_counter) > 10)
        {
            CFE_EVS_SendEvent(EPS_HEARTBEAT_ERR_EID, CFE_EVS_ERROR, "EPS: Error, missing hearbeat resetting EPS PCM channels");
            uint8 pcms = 0;
            pcms |= 0x1; //reset battery V bus
            pcms |= 0x2; //reset 5V bus
            pcms |= 0x4; //reset 3.3V bus
            pcms |= 0x8; //reset 12V bus
            EPS_reset_PCM(pcms);
        }
        else
        {
            EPS_report_housekeeping();
            if (EPS_AppData.heartbeat_counter != 0)
            {
                //keep the counters from growing large
                EPS_AppData.hk_packets_requested = 0;
                EPS_AppData.heartbeat_counter = 0;
            }
        }
        break;

        /* invalid message ID */
    default:
        CFE_EVS_SendEvent(EPS_CMD_ERR_EID, CFE_EVS_ERROR, "EPS: Error - invalid message id (mid: %i)", msg_id);
    }
}


void EPS_process_ground_command(void)
{

    /* process command code */
    uint16 cmd_code = CFE_SB_GetCmdCode(EPS_AppData.msg_ptr);

    switch(cmd_code)
    {
        /* general app commands */
    case EPS_CMD_NOOP_CC:
        if (EPS_verify_cmd_length(EPS_AppData.msg_ptr, sizeof(EPS_no_args_cmd_t)))
        {
            EPS_AppData.hk.cmd_counters.success++;
            CFE_EVS_SendEvent(EPS_CMD_NOOP_EID, CFE_EVS_INFORMATION, 
                              "EPS: Info, NOOP_CC received");
        }
        break;
        /* EPS commands */
    case EPS_CMD_RESET_COUNTERS_CC:
        if (EPS_verify_cmd_length(EPS_AppData.msg_ptr, sizeof(EPS_no_args_cmd_t)))
        {
            EPS_reset_counters();
            CFE_EVS_SendEvent(EPS_CMD_RESET_EID, CFE_EVS_INFORMATION, 
                              "EPS: Info, RESET_COUNTERS_CC received");
        }
        break;

    case EPS_CMD_GET_BOARD_STATUS_CC:
        CFE_EVS_SendEvent(EPS_CMD_GET_BOARD_STATUS_EID, CFE_EVS_ERROR, 
                          "EPS: Error - EPS_CMD_GET_BOARD_STATUS not implemented");
        break;

    case EPS_CMD_GET_LAST_ERROR_CC:
        CFE_EVS_SendEvent(EPS_CMD_GET_LAST_ERROR_EID, CFE_EVS_ERROR, 
                          "EPS: Error - EPS_CMD_GET_LAST_ERROR not implemented");
        break;

    case EPS_CMD_GET_VERSION_CC: 
        CFE_EVS_SendEvent(EPS_CMD_GET_VERSION_EID, CFE_EVS_ERROR, 
                          "EPS: Error - EPS_CMD_GET_VERSION not implemented");
        break;                

    case EPS_CMD_GET_CHECKSUM_CC:   
        CFE_EVS_SendEvent(EPS_CMD_GET_CHECKSUM_EID, CFE_EVS_ERROR, 
                          "EPS: Error - EPS_CMD_GET_CHECKSUM not implemented");
        break;                

    case EPS_CMD_GET_TELEMETRY_CC:     
        CFE_EVS_SendEvent(EPS_CMD_GET_TELEMETRY_EID, CFE_EVS_ERROR, 
                          "EPS: Error - EPS_CMD_GET_TELEMETRY not implemented");
        break;  

    case EPS_CMD_GET_COM_WATCHDOG_PERIOD_CC:      
        CFE_EVS_SendEvent(EPS_CMD_GET_COM_WATCHDOG_PERIOD_EID, CFE_EVS_ERROR, 
                          "EPS: Error - EPS_CMD_GET_COM_WATCHDOG_PERIOD not implemented");
        break;  

    case EPS_CMD_SET_COM_WATCHDOG_PERIOD_CC:   
        if (EPS_verify_cmd_length(EPS_AppData.msg_ptr, sizeof(EPS_cmd_t)))
        {
            EPS_cmd_t *cmd;
            cmd = (EPS_cmd_t*)EPS_AppData.msg_ptr; 
            EPS_AppData.hk.cmd_counters.success++;
            CFE_EVS_SendEvent(EPS_CMD_SET_COM_WATCHDOG_PERIOD_EID, CFE_EVS_INFORMATION, 
                              "EPS: Info, EPS_CMD_SET_COM_WATCHDOG_PERIOD received (period: %u)", cmd->data);
            if (EPS_set_comms_watchdog_period(cmd->data) == OS_ERROR)
                CFE_EVS_SendEvent(EPS_I2C_ERR_EID, CFE_EVS_ERROR, "EPS: EPS_set_comms_watchdog_period: I2C error");
        }
        break;  

    case EPS_CMD_RESET_COM_WATCHDOG_CC:           
        if (EPS_verify_cmd_length(EPS_AppData.msg_ptr, sizeof(EPS_no_args_cmd_t)))
        {
            EPS_AppData.hk.cmd_counters.success++;
            CFE_EVS_SendEvent(EPS_CMD_RESET_COM_WATCHDOG_EID, CFE_EVS_INFORMATION, 
                              "EPS: Info, RESET_COM_WATCHDOG received");
            if (EPS_reset_comms_watchdog() == OS_ERROR)
                CFE_EVS_SendEvent(EPS_I2C_ERR_EID, CFE_EVS_ERROR, "EPS: EPS_reset_comms_watchdog: I2C error");
        }
        break;  

    case EPS_CMD_GET_NUMBER_OF_BOR_CC:            
        CFE_EVS_SendEvent(EPS_CMD_GET_NUMBER_OF_BOR_EID, CFE_EVS_ERROR, 
                          "EPS: Error - EPS_CMD_GET_NUMBER_OF_BOR not implemented");
        break;  

    case EPS_CMD_GET_NUMBER_OF_ASR_CC:           
        CFE_EVS_SendEvent(EPS_CMD_GET_NUMBER_OF_ASR_EID, CFE_EVS_ERROR, 
                          "EPS: Error - EPS_CMD_GET_NUMBER_OF_ASR not implemented");
        break;  

    case EPS_CMD_GET_NUMBER_OF_MR_CC:             
        CFE_EVS_SendEvent(EPS_CMD_GET_NUMBER_OF_MR_EID, CFE_EVS_ERROR, 
                          "EPS: Error - EPS_CMD_GET_NUMBER_OF_MR not implemented");
        break;  

    case EPS_CMD_GET_NUMBER_OF_CWDR_CC:           
        CFE_EVS_SendEvent(EPS_CMD_GET_NUMBER_OF_CWDR_EID, CFE_EVS_ERROR, 
                          "EPS: Error - EPS_CMD_GET_NUMBER_OF_CWDR not implemented");
        break;  

    case EPS_CMD_SWITCH_ON_ALL_PDM_CC:
        if (EPS_verify_cmd_length(EPS_AppData.msg_ptr, sizeof(EPS_no_args_cmd_t)))
        {
            EPS_AppData.hk.cmd_counters.success++;
            CFE_EVS_SendEvent(EPS_CMD_SWITCH_ON_ALL_PDM_EID, CFE_EVS_INFORMATION, 
                              "EPS: Info, SWITCH_ON_ALL_PDM received");
            if (EPS_switch_all_PDM_on() == OS_ERROR)
                CFE_EVS_SendEvent(EPS_I2C_ERR_EID, CFE_EVS_ERROR, "EPS: EPS_switch_all_PDM_on: I2C error");
            
        }
        break;  

    case EPS_CMD_SWITCH_OFF_ALL_PDM_CC:
        if (EPS_verify_cmd_length(EPS_AppData.msg_ptr, sizeof(EPS_no_args_cmd_t)))
        {
            EPS_AppData.hk.cmd_counters.success++;
            CFE_EVS_SendEvent(EPS_CMD_SWITCH_OFF_ALL_PDM_EID, CFE_EVS_INFORMATION, 
                              "EPS: Info, SWITCH_OFF_ALL_PDM received");
            if (EPS_switch_all_PDM_off() == OS_ERROR)
                CFE_EVS_SendEvent(EPS_I2C_ERR_EID, CFE_EVS_ERROR, "EPS: EPS_switch_all_PDM_off: I2C error");
        }
        break;  

    case EPS_CMD_GET_ACTUAL_STATE_ALL_PDM_CC:    
        CFE_EVS_SendEvent(EPS_CMD_GET_ACTUAL_STATE_ALL_PDM_EID, CFE_EVS_ERROR, 
                          "EPS: Error - EPS_CMD_GET_ACTUAL_STATE_ALL_PDM not implemented");
        break;   

    case EPS_CMD_GET_EXPECTED_STATE_ALL_PDM_CC:   
        CFE_EVS_SendEvent(EPS_CMD_GET_EXPECTED_STATE_ALL_PDM_EID, CFE_EVS_ERROR, 
                          "EPS: Error - EPS_CMD_GET_EXPECTED_STATE_ALL_PDM not implemented");
        break;  

    case EPS_CMD_GET_INIT_STATE_ALL_PDM_CC:       
        CFE_EVS_SendEvent(EPS_CMD_GET_INIT_STATE_ALL_PDM_EID, CFE_EVS_ERROR, 
                          "EPS: Error - EPS_CMD_GET_INIT_STATE_ALL_PDM not implemented");
        break;  

    case EPS_CMD_SET_ALL_PDM_TO_INIT_STATE_CC:
        if (EPS_verify_cmd_length(EPS_AppData.msg_ptr, sizeof(EPS_no_args_cmd_t)))
        {
            EPS_AppData.hk.cmd_counters.success++;
            CFE_EVS_SendEvent(EPS_CMD_SET_ALL_PDM_TO_INIT_STATE_EID, CFE_EVS_INFORMATION, 
                              "EPS: Info, SET_ALL_PDM_TO_INIT_STATE received");
            if (EPS_set_all_PDM_init_state() == OS_ERROR)
                CFE_EVS_SendEvent(EPS_I2C_ERR_EID, CFE_EVS_ERROR, "EPS: EPS_set_all_PDM_init_state: I2C error");
        }
        break;  

    case EPS_CMD_SWITCH_PDM_N_ON_CC: 
        if (EPS_verify_cmd_length(EPS_AppData.msg_ptr, sizeof(EPS_cmd_t)))
        {
            EPS_cmd_t *cmd;
            cmd = (EPS_cmd_t*)EPS_AppData.msg_ptr;
            EPS_AppData.hk.cmd_counters.success++;
            CFE_EVS_SendEvent(EPS_CMD_SWITCH_PDM_N_ON_EID, CFE_EVS_INFORMATION, 
                              "EPS: Info, SWITCH_PDM_N_ON received (PDM: %u)", cmd->data);   
            if (EPS_switch_PDM_on(cmd->data) == OS_ERROR)
                CFE_EVS_SendEvent(EPS_I2C_ERR_EID, CFE_EVS_ERROR, "EPS: EPS_switch_PDM_on (PDM: %u): I2C error", cmd->data);
        }
        break;  

    case EPS_CMD_SWITCH_PDM_N_OFF_CC:
        if (EPS_verify_cmd_length(EPS_AppData.msg_ptr, sizeof(EPS_cmd_t)))
        {
            EPS_cmd_t *cmd;
            cmd = (EPS_cmd_t*)EPS_AppData.msg_ptr;
            EPS_AppData.hk.cmd_counters.success++;
            CFE_EVS_SendEvent(EPS_CMD_SWITCH_PDM_N_OFF_EID, CFE_EVS_INFORMATION, 
                              "EPS: Info, SWITCH_PDM_N_OFF received (PDM: %u)", cmd->data);        
            if (EPS_switch_PDM_off(cmd->data) == OS_ERROR)
                CFE_EVS_SendEvent(EPS_I2C_ERR_EID, CFE_EVS_ERROR, "EPS: EPS_switch_PDM_off (PDM: %u): I2C error", cmd->data);
        }
        break;  

    case EPS_CMD_SET_PDM_N_INIT_STATE_ON_CC:
        if (EPS_verify_cmd_length(EPS_AppData.msg_ptr, sizeof(EPS_cmd_t)))
        {
            EPS_cmd_t *cmd;
            cmd = (EPS_cmd_t*)EPS_AppData.msg_ptr;
            EPS_AppData.hk.cmd_counters.success++; 
            CFE_EVS_SendEvent(EPS_CMD_SET_PDM_N_INIT_STATE_ON_EID, CFE_EVS_INFORMATION, 
                              "EPS: Info, SET_PDM_N_INIT_STATE_ON receieved (PDM: %u)", cmd->data);       
            if (EPS_set_PDM_init_state_on(cmd->data) == OS_ERROR)
                CFE_EVS_SendEvent(EPS_I2C_ERR_EID, CFE_EVS_ERROR, "EPS: EPS_set_PDM_init_state_on (PDM: %u): I2C error", cmd->data);
        }
        break;  

    case EPS_CMD_SET_PDM_N_INIT_STATE_OFF_CC:
        if (EPS_verify_cmd_length(EPS_AppData.msg_ptr, sizeof(EPS_cmd_t)))
        {
            EPS_cmd_t *cmd;
            cmd = (EPS_cmd_t*)EPS_AppData.msg_ptr;
            EPS_AppData.hk.cmd_counters.success++;
            CFE_EVS_SendEvent(EPS_CMD_SET_PDM_N_INIT_STATE_OFF_EID, CFE_EVS_INFORMATION, 
                              "EPS: Info, SET_PDM_N_INIT_STATE_OFF received (PDM: %u)", cmd->data);     
            if (EPS_set_PDM_init_state_off(cmd->data) == OS_ERROR)
                CFE_EVS_SendEvent(EPS_I2C_ERR_EID, CFE_EVS_ERROR, "EPS: EPS_set_PDM_init_state_off (PDM: %u): I2C error", cmd->data);
        }
        break;  

    case EPS_CMD_GET_PDM_N_ACTUAL_STATUS_CC:    
        CFE_EVS_SendEvent(EPS_CMD_GET_PDM_N_ACTUAL_STATUS_EID, CFE_EVS_ERROR, 
                          "EPS: Error - EPS_CMD_GET_PDM_N_ACTUAL_STATUS not implemented");
        break;  

    case EPS_CMD_SET_PDM_N_TIMER_LIMIT_CC:   
        if (EPS_verify_cmd_length(EPS_AppData.msg_ptr, sizeof(EPS_cmd_t)))
        {
            EPS_cmd_t *cmd;
            cmd = (EPS_cmd_t*)EPS_AppData.msg_ptr; 
            EPS_AppData.hk.cmd_counters.success++;  
            CFE_EVS_SendEvent(EPS_CMD_SET_PDM_N_TIMER_LIMIT_EID, CFE_EVS_INFORMATION, 
                              "EPS: Info, SET_PDM_N_TIMER_LIMIT received (Data: %u)", cmd->data);
            if (EPS_set_PDM_timer_limit(cmd->data) == OS_ERROR)
                CFE_EVS_SendEvent(EPS_I2C_ERR_EID, CFE_EVS_ERROR, "EPS: EPS_set_PDM_timer_limit (PDM: %u): I2C error", cmd->data);
        }
        break;  

    case EPS_CMD_GET_PDM_N_TIMER_LIMIT_CC:     
        CFE_EVS_SendEvent(EPS_CMD_RESET_EID, CFE_EVS_ERROR, 
                          "EPS: Error - EPS_CMD_GET_PDM_N_TIMER_LIMIT not implemented");
        break;  

    case EPS_CMD_GET_PDM_N_CURRENT_TIMER_VAL_CC:  
        CFE_EVS_SendEvent(EPS_CMD_GET_PDM_N_CURRENT_TIMER_VAL_EID, CFE_EVS_ERROR, 
                          "EPS: Error - EPS_CMD_GET_PDM_N_CURRENT_TIMER_VAL not implemented");
        break;  

    case EPS_CMD_PCM_RESET_CC:
        if (EPS_verify_cmd_length(EPS_AppData.msg_ptr, sizeof(EPS_cmd_t)))
        {
            EPS_cmd_t *cmd;
            cmd = (EPS_cmd_t*)EPS_AppData.msg_ptr;
            EPS_AppData.hk.cmd_counters.success++;                  
            CFE_EVS_SendEvent(EPS_CMD_PCM_RESET_EID, CFE_EVS_INFORMATION, 
                              "EPS: Info, PCM_RESET received (PCM: %u)", cmd->data);   
            if (EPS_reset_PCM(cmd->data) == OS_ERROR)
                CFE_EVS_SendEvent(EPS_I2C_ERR_EID, CFE_EVS_ERROR, "EPS: EPS_reset_PCM (PCM: %u): I2C error", cmd->data);
        }
        break;  

    case EPS_CMD_MANUAL_RESET_CC:                 
        if (EPS_verify_cmd_length(EPS_AppData.msg_ptr, sizeof(EPS_no_args_cmd_t)))
        {
            EPS_AppData.hk.cmd_counters.success++;
            CFE_EVS_SendEvent(EPS_CMD_MANUAL_RESET_EID, CFE_EVS_INFORMATION, 
                              "EPS: Info, MANUAL_RESET received");   
            if (EPS_manual_reset() == OS_ERROR)
                CFE_EVS_SendEvent(EPS_I2C_ERR_EID, CFE_EVS_ERROR, "EPS: EPS_manual_reset: I2C error");
        }
        break;   

    case BAT_CMD_SET_HEATER_CONTROLLER_STATUS_CC:
        if (EPS_verify_cmd_length(EPS_AppData.msg_ptr, sizeof(BAT_cmd_t)))
        {
            BAT_cmd_t *cmd;
            cmd = (BAT_cmd_t*)EPS_AppData.msg_ptr;
            uint8 heater_code = (uint8)((cmd->data & 0xFF00) >> 8);
            uint8 battery = (uint8)(cmd->data & 0x00FF);
            EPS_AppData.hk.cmd_counters.success++;
            CFE_EVS_SendEvent(BAT_CMD_SET_HEATER_CONTROLLER_STATUS_EID, CFE_EVS_INFORMATION, 
                              "EPS: Info, SET_HEATER_CONTROLLER_STATUS received (battery: %u status: %u)", battery, heater_code);   
            if (BAT_set_heater_controller_status(heater_code, battery) == OS_ERROR)
                CFE_EVS_SendEvent(EPS_I2C_ERR_EID, CFE_EVS_ERROR, "EPS: BAT_set_heater_controller_status: I2C error");
        }
        break;

    case BAT_CMD_MANUAL_RESET_CC:                 
        if (EPS_verify_cmd_length(EPS_AppData.msg_ptr, sizeof(BAT_cmd_t)))
        {
            BAT_cmd_t *cmd;
            cmd = (BAT_cmd_t*)EPS_AppData.msg_ptr;
            uint8 battery = (uint8)(cmd->data & 0x00FF);
            EPS_AppData.hk.cmd_counters.success++;
            CFE_EVS_SendEvent(BAT_CMD_MANUAL_RESET_EID, CFE_EVS_INFORMATION, 
                              "EPS: Info, BAT_MANUAL_RESET received (battery: %u)", battery);   
            if (BAT_manual_reset(battery) == OS_ERROR)
                CFE_EVS_SendEvent(EPS_I2C_ERR_EID, CFE_EVS_ERROR, "EPS: BAT_manual_reset: I2C error");
        }
        break;   

    case BAT_CMD_GET_BOARD_STATUS_CC:
    case BAT_CMD_GET_LAST_ERROR_CC:
    case BAT_CMD_GET_VERSION_CC:
    case BAT_CMD_GET_CHECKSUM_CC:
    case BAT_CMD_GET_TELEMETRY_CC:
    case BAT_CMD_GET_NUMBER_OF_BOR_CC:
    case BAT_CMD_GET_NUMBER_OF_ASR_CC:
    case BAT_CMD_GET_NUMBER_OF_MR_CC:
    case BAT_CMD_GET_HEATER_CONTROLLER_STATUS_CC:
        CFE_EVS_SendEvent(EPS_CMD_ERR_EID, CFE_EVS_ERROR, 
                          "EPS: Error - BAT command code (CC: %i) not implemented", cmd_code);
        break;
               
    default:
        EPS_AppData.hk.cmd_counters.error++;
        CFE_EVS_SendEvent(EPS_CMD_ERR_EID, CFE_EVS_ERROR, 
                          "EPS: Error - invalid command code (mid: %i, cc: %i)",
                          CFE_SB_GetMsgId(EPS_AppData.msg_ptr), cmd_code);
        break;
    }
}


void EPS_report_housekeeping(void)
{
    int status = OS_SUCCESS;
    int bat_status = OS_SUCCESS;

    /* start housekeeping (HK) perf log entry */
    CFE_ES_PerfLogEntry(EPS_HK_PERF_ID);

    /* EPS board status */
    //TODO do we care which i2c calls fail? If so we can add uint64_t bit mask
    status += EPS_get_board_status(&EPS_AppData.hk.board_status);
    status += EPS_get_comms_watchdog_period(&EPS_AppData.hk.comms_watchdog_period);
    status += EPS_get_num_brownout_resets(&EPS_AppData.hk.resets.brownout);
    status += EPS_get_num_auto_software_resets(&EPS_AppData.hk.resets.auto_software);
    status += EPS_get_num_manual_resets(&EPS_AppData.hk.resets.manual);
    status += EPS_get_num_comms_watchdog_resets(&EPS_AppData.hk.resets.comms_watchdog);
    status += EPS_get_all_PDM_actual_state(&EPS_AppData.hk.pdm_states.actual);
    status += EPS_get_all_PDM_expected_state(&EPS_AppData.hk.pdm_states.expected);
    status += EPS_get_PDM_init_state(&EPS_AppData.hk.pdm_states.initial);

    /* EPS battery charge regulator (BCR) telemetry */
    EPS_channel_t chan[] = {CHANNEL_VBCR1, CHANNEL_VBCR2, CHANNEL_VBCR3, CHANNEL_VBCR4, CHANNEL_VBCR5};

    int i;
    for(i = 0; i < EPS_BCR_COUNT; i++)
    {
        status += EPS_get_telemetry(chan[i], &EPS_AppData.hk.bcr[i].voltage);
        status += EPS_get_telemetry(chan[i] + 4, &EPS_AppData.hk.bcr[i].current[0]);
        status += EPS_get_telemetry(chan[i] + 5, &EPS_AppData.hk.bcr[i].current[1]);
        status += EPS_get_telemetry(chan[i] + 8, &EPS_AppData.hk.bcr[i].temp[0]);
        status += EPS_get_telemetry(chan[i] + 9, &EPS_AppData.hk.bcr[i].temp[1]);  
    }

    status += EPS_get_telemetry(CHANNEL_IIDIODE_OUT, &EPS_AppData.hk.IIDiode);

    float current_vidiode = EPS_AppData.hk.VIDiode;
    if ( (status += EPS_get_telemetry(CHANNEL_VIDIODE_OUT, &EPS_AppData.hk.VIDiode)) == OS_ERROR)
    {
        EPS_AppData.hk.VIDiode = current_vidiode;
    }

    status += EPS_get_telemetry(CHANNEL_I3V3_DRW, &EPS_AppData.hk.I3V3Draw);   
    status += EPS_get_telemetry(CHANNEL_I5V_DRW, &EPS_AppData.hk.I5VDraw);
        
    /* EPS power conditioning module (PCM) telemetry */
    status += EPS_get_telemetry(CHANNEL_IPCM12V, &EPS_AppData.hk.pcm[PCM_BUS_12V].current);
    status += EPS_get_telemetry(CHANNEL_VPCM12V, &EPS_AppData.hk.pcm[PCM_BUS_12V].voltage);
    status += EPS_get_telemetry(CHANNEL_IPCMBATV, &EPS_AppData.hk.pcm[PCM_BUS_BAT].current);
    status += EPS_get_telemetry(CHANNEL_VPCMBATV, &EPS_AppData.hk.pcm[PCM_BUS_BAT].voltage);
    status += EPS_get_telemetry(CHANNEL_IPCM5V, &EPS_AppData.hk.pcm[PCM_BUS_5V].current);
    status += EPS_get_telemetry(CHANNEL_VPCM5V, &EPS_AppData.hk.pcm[PCM_BUS_5V].voltage);
    status += EPS_get_telemetry(CHANNEL_IPCM3V3, &EPS_AppData.hk.pcm[PCM_BUS_3V].current);
    status += EPS_get_telemetry(CHANNEL_VPCM3V3, &EPS_AppData.hk.pcm[PCM_BUS_3V].voltage);

    /* EPS power distribution module (PDM) telemetry */
    for(i = 0; i < EPS_SWITCH_COUNT; i++) 
    {
        status += EPS_get_telemetry(CHANNEL_VSW1 + (16 * i), &EPS_AppData.hk.pdm[i].voltage);
        status += EPS_get_telemetry(CHANNEL_ISW1 + (16 * i), &EPS_AppData.hk.pdm[i].current);
    }

    /* EPS board temperature */
    status += EPS_get_telemetry(CHANNEL_TBRD, &EPS_AppData.hk.board_temp);
    
    /* Collect battery telemetry and status */
    int b;
    for (b = 0; b < BAT_COUNT; b++)
    {
        bat_status += BAT_get_board_status(&EPS_AppData.hk.battery[b].status, b);
        bat_status += BAT_get_num_brownout_resets(&EPS_AppData.hk.battery[b].resets.brownout, b);
        bat_status += BAT_get_num_auto_software_resets(&EPS_AppData.hk.battery[b].resets.auto_software, b);
        bat_status += BAT_get_num_manual_resets(&EPS_AppData.hk.battery[b].resets.manual, b);
        bat_status += BAT_get_heater_controller_status(&EPS_AppData.hk.battery[b].heater_controller_status, b);
        bat_status += BAT_get_checksum(&EPS_AppData.hk.battery[b].checksum, b);
        bat_status += BAT_get_telemetry(TLM_VBAT, &EPS_AppData.hk.battery[b].output_voltage, b);
        bat_status += BAT_get_telemetry(TLM_IBAT, &EPS_AppData.hk.battery[b].current_mag, b);
        bat_status += BAT_get_telemetry(TLM_IDIRBAT, &EPS_AppData.hk.battery[b].current_dir, b);
        bat_status += BAT_get_telemetry(TLM_TBRD, &EPS_AppData.hk.battery[b].motherboard_temp, b);
        bat_status += BAT_get_telemetry(TLM_IPCM5V, &EPS_AppData.hk.battery[b].pcm_5v.current, b);
        bat_status += BAT_get_telemetry(TLM_VPCM5V, &EPS_AppData.hk.battery[b].pcm_5v.voltage, b);
        bat_status += BAT_get_telemetry(TLM_IPCM3V3, &EPS_AppData.hk.battery[b].pcm_3v3.current, b);
        bat_status += BAT_get_telemetry(TLM_VPCM3V3, &EPS_AppData.hk.battery[b].pcm_3v3.voltage, b);

        int d;
        uint16 current_channel = TLM_TBAT1;
        for (d = 0; d < BAT_DAUGHTERBOARD_COUNT; d++)
        {
            bat_status += BAT_get_telemetry(current_channel, &EPS_AppData.hk.battery[b].db[d].temp, b);
            current_channel++;
            bat_status += BAT_get_telemetry(current_channel, &EPS_AppData.hk.battery[b].db[d].heater_status, b);
            current_channel++;        
        }
    }

    if (status != OS_SUCCESS)
        CFE_EVS_SendEvent(EPS_I2C_ERR_EID, CFE_EVS_ERROR, "EPS: Error - EPS i2c call failed while constructing HK packet (status: %d)", status);

    if (bat_status != OS_SUCCESS)
        CFE_EVS_SendEvent(EPS_I2C_ERR_EID, CFE_EVS_ERROR, "EPS: Error - BAT i2c call failed while constructing HK packet (bat_status: %d)", bat_status);

    EPS_AppData.hk.eps_i2c_status = status;
    EPS_AppData.hk.bat_i2c_status = bat_status;

    /* set timestamp and send HK data */
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t*)&EPS_AppData.hk);
    CFE_SB_SendMsg((CFE_SB_Msg_t*)&EPS_AppData.hk);
    
    /* stop housekeeping (HK) perf log entry */
    CFE_ES_PerfLogExit(EPS_HK_PERF_ID);
}


void EPS_reset_counters(void)
{
    EPS_AppData.hk.cmd_counters.error = 0;
    EPS_AppData.hk.cmd_counters.success = 0;
}

boolean EPS_verify_cmd_length(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength)
{     
    boolean result = TRUE;
    uint16 ActualLength = CFE_SB_GetTotalMsgLength(msg);

    /*
    ** Verify the command packet length.
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_SB_MsgId_t MessageID   = CFE_SB_GetMsgId(msg);
        uint16         CommandCode = CFE_SB_GetCmdCode(msg);

        CFE_EVS_SendEvent(EPS_LEN_ERR_EID, CFE_EVS_ERROR,
                          "Error, invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
                          MessageID, CommandCode, ActualLength, ExpectedLength);

        result = FALSE;
        EPS_AppData.hk.cmd_counters.error++;
    }

    return result;
}
