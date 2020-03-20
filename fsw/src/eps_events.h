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

#ifndef _EPS_EVENTS_H_
#define _EPS_EVENTS_H_

/*
** General events
*/
#define EPS_INIT_EID        1

/*
** Error events
*/
#define EPS_INIT_ERR_EID      5
#define EPS_ES_ERR_EID        6
#define EPS_SB_ERR_EID        7
#define EPS_CMD_ERR_EID       8
#define EPS_CHECKSUM_ERR_EID  9
#define EPS_I2C_ERR_EID       10
#define EPS_HEARTBEAT_ERR_EID 11

/*
** General command events
*/
#define EPS_CMD_NOOP_EID    12
#define EPS_CMD_RESET_EID   13
#define EPS_LEN_ERR_EID     14

/*
** EPS command events
*/
#define EPS_CMD_GET_BOARD_STATUS_EID             20
#define EPS_CMD_GET_LAST_ERROR_EID               21
#define EPS_CMD_GET_VERSION_EID                  22
#define EPS_CMD_GET_CHECKSUM_EID                 23
#define EPS_CMD_GET_TELEMETRY_EID                24
#define EPS_CMD_GET_COM_WATCHDOG_PERIOD_EID      25
#define EPS_CMD_SET_COM_WATCHDOG_PERIOD_EID      26
#define EPS_CMD_RESET_COM_WATCHDOG_EID           27
#define EPS_CMD_GET_NUMBER_OF_BOR_EID            28 //Brown-out Resets
#define EPS_CMD_GET_NUMBER_OF_ASR_EID            29 //Auto Software Resets
#define EPS_CMD_GET_NUMBER_OF_MR_EID             30 //Manual Resets
#define EPS_CMD_GET_NUMBER_OF_CWDR_EID           31 //Comms Watchdog Resets
#define EPS_CMD_SWITCH_ON_ALL_PDM_EID            32
#define EPS_CMD_SWITCH_OFF_ALL_PDM_EID           33
#define EPS_CMD_GET_ACTUAL_STATE_ALL_PDM_EID     34
#define EPS_CMD_GET_EXPECTED_STATE_ALL_PDM_EID   35
#define EPS_CMD_GET_INIT_STATE_ALL_PDM_EID       36
#define EPS_CMD_SET_ALL_PDM_TO_INIT_STATE_EID    37
#define EPS_CMD_SWITCH_PDM_N_ON_EID              38 
#define EPS_CMD_SWITCH_PDM_N_OFF_EID             39
#define EPS_CMD_SET_PDM_N_INIT_STATE_ON_EID      40
#define EPS_CMD_SET_PDM_N_INIT_STATE_OFF_EID     41
#define EPS_CMD_GET_PDM_N_ACTUAL_STATUS_EID      42
#define EPS_CMD_SET_PDM_N_TIMER_LIMIT_EID        43
#define EPS_CMD_GET_PDM_N_TIMER_LIMIT_EID        44
#define EPS_CMD_GET_PDM_N_CURRENT_TIMER_VAL_EID  45
#define EPS_CMD_PCM_RESET_EID                    46 
#define EPS_CMD_MANUAL_RESET_EID                 47

/*
** Battery command events
*/
#define BAT_CMD_SET_HEATER_CONTROLLER_STATUS_EID 60
#define BAT_CMD_MANUAL_RESET_EID                 61

#endif

