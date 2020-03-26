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

#ifndef _EPS_MSG_H_
#define _EPS_MSG_H_


#include "common_types.h"
#include "cfe_sb.h"
#include "eps/eps_3ua.h"
#include "eps/eps_bat.h"

typedef struct
{
    uint8 CmdHeader[CFE_SB_CMD_HDR_SIZE];

} EPS_no_args_cmd_t;


typedef struct
{
    uint8  CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint16 data;

} EPS_cmd_t;

typedef struct
{
    uint8  CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint16 data;

} BAT_cmd_t;

/*
** EPS battery charge regulator (BCR) data
*/
typedef struct
{
    float voltage;
    float current[2];
    float temp[2];

} OS_PACK EPS_bcr_data_t;

/*
** EPS power conditioning module (PCM) data
*/
typedef struct
{
    float current;
    float voltage;

} OS_PACK EPS_pcm_data_t;


typedef struct
{
    float current;
    float voltage;

} OS_PACK EPS_bat_pcm_data_t;

typedef struct
{
    float heater_status;
    float temp;

} OS_PACK EPS_bat_daughterboard_t;

/*
** EPS power distribution module (PDM) data
*/
typedef struct
{
    float current;
    float voltage;

} OS_PACK EPS_pdm_data_t;

typedef struct
{
    uint16 brownout;
    uint16 auto_software;
    uint16 manual;

} OS_PACK EPS_bat_resets_t;

/*
** EPS battery data
*/
typedef struct
{
    uint16 version;
    uint16 status;
    float output_voltage;
    float current_mag;
    float current_dir;
    float motherboard_temp;
    uint16 heater_controller_status;
    EPS_bat_resets_t resets;
    EPS_bat_pcm_data_t pcm_5v;
    EPS_bat_pcm_data_t pcm_3v3;
    EPS_bat_daughterboard_t db[BAT_DAUGHTERBOARD_COUNT];
    uint16 checksum;

} OS_PACK EPS_battery_data_t;

/*
 * EPS PDM States
 */
typedef struct
{
    uint32 actual;
    uint32 expected;
    uint32 initial;

} OS_PACK EPS_pdm_states_t;

/*
 * EPS Resets
 */
typedef struct
{
    uint16 brownout;
    uint16 auto_software;
    uint16 manual;
    uint16 comms_watchdog;

} OS_PACK EPS_resets_t;

typedef struct
{
    uint32 error;
    uint32 success;

} OS_PACK EPS_counters_t;

typedef struct
{
    uint8 TlmHeader[CFE_SB_TLM_HDR_SIZE];

    EPS_counters_t cmd_counters;

    uint16 version;
    uint8 board_status;
    uint16 checksum;
    uint16 comms_watchdog_period;
   
    float IIDiode;
    float VIDiode;
    float I3V3Draw;
    float I5VDraw; 

    float board_temp;

    EPS_bcr_data_t bcr[EPS_BCR_COUNT];
    EPS_pcm_data_t pcm[NUM_PCM_BUSES];
    EPS_pdm_data_t pdm[EPS_SWITCH_COUNT];
    EPS_pdm_states_t pdm_states;

    EPS_resets_t resets;

    int eps_i2c_status;
    int bat_i2c_status;

    EPS_battery_data_t battery[BAT_COUNT];

} OS_PACK EPS_hk_tlm_t;

#define EPS_HK_TLM_LNGTH sizeof(EPS_hk_tlm_t)

#endif

