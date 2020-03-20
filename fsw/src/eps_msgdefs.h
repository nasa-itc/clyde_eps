#ifndef _EPS_MSGDEFS_H_
#define _EPS_MSGDEFS_H_


#define EPS_CMD_NOOP_CC             0
#define EPS_CMD_RESET_COUNTERS_CC   1

/*
** EPS command codes
*/

#define EPS_CMD_GET_BOARD_STATUS_CC             10
#define EPS_CMD_GET_LAST_ERROR_CC               11
#define EPS_CMD_GET_VERSION_CC                  12
#define EPS_CMD_GET_CHECKSUM_CC                 13
#define EPS_CMD_GET_TELEMETRY_CC                14
#define EPS_CMD_GET_COM_WATCHDOG_PERIOD_CC      15
#define EPS_CMD_SET_COM_WATCHDOG_PERIOD_CC      16
#define EPS_CMD_RESET_COM_WATCHDOG_CC           17
#define EPS_CMD_GET_NUMBER_OF_BOR_CC            18 //Brown-out Resets
#define EPS_CMD_GET_NUMBER_OF_ASR_CC            19 //Auto Software Resets
#define EPS_CMD_GET_NUMBER_OF_MR_CC             20 //Manual Resets
#define EPS_CMD_GET_NUMBER_OF_CWDR_CC           21 //Comms Watchdog Resets
#define EPS_CMD_SWITCH_ON_ALL_PDM_CC            22
#define EPS_CMD_SWITCH_OFF_ALL_PDM_CC           23
#define EPS_CMD_GET_ACTUAL_STATE_ALL_PDM_CC     24
#define EPS_CMD_GET_EXPECTED_STATE_ALL_PDM_CC   25
#define EPS_CMD_GET_INIT_STATE_ALL_PDM_CC       26
#define EPS_CMD_SET_ALL_PDM_TO_INIT_STATE_CC    27
#define EPS_CMD_SWITCH_PDM_N_ON_CC              28
#define EPS_CMD_SWITCH_PDM_N_OFF_CC             29
#define EPS_CMD_SET_PDM_N_INIT_STATE_ON_CC      30
#define EPS_CMD_SET_PDM_N_INIT_STATE_OFF_CC     31
#define EPS_CMD_GET_PDM_N_ACTUAL_STATUS_CC      32
#define EPS_CMD_SET_PDM_N_TIMER_LIMIT_CC        33
#define EPS_CMD_GET_PDM_N_TIMER_LIMIT_CC        34
#define EPS_CMD_GET_PDM_N_CURRENT_TIMER_VAL_CC  35
#define EPS_CMD_PCM_RESET_CC                    36
#define EPS_CMD_MANUAL_RESET_CC                 37

#define BAT_CMD_GET_BOARD_STATUS_CC             50
#define BAT_CMD_GET_LAST_ERROR_CC               51
#define BAT_CMD_GET_VERSION_CC                  52
#define BAT_CMD_GET_CHECKSUM_CC                 53
#define BAT_CMD_GET_TELEMETRY_CC                54
#define BAT_CMD_GET_NUMBER_OF_BOR_CC            55
#define BAT_CMD_GET_NUMBER_OF_ASR_CC            56
#define BAT_CMD_GET_NUMBER_OF_MR_CC             57
#define BAT_CMD_GET_HEATER_CONTROLLER_STATUS_CC 58
#define BAT_CMD_SET_HEATER_CONTROLLER_STATUS_CC 59
#define BAT_CMD_MANUAL_RESET_CC                 60

#endif
