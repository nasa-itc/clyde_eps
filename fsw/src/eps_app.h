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

#ifndef _EPS_APP_H_
#define _EPS_APP_H_

#include "cfe.h"
#include "eps_msg.h"

#define EPS_PIPE_NAME  "EPS_CMD_PIPE"
#define EPS_PIPE_DEPTH 32

#define EPS_CHECKSUM 0x1F890B93 //value taken from testing

typedef struct
{
    EPS_hk_tlm_t hk;
    
    CFE_SB_MsgPtr_t msg_ptr;          /* Software bus message pointer */
    CFE_SB_PipeId_t cmd_pipe;         /* EPS command pipe ID */
    uint32 run_status;                /* Run status for controlling the app state */
    uint8 heartbeat_counter;          /*heartbeat to ensure other major apps are operational*/
    uint8 hk_packets_requested;       /*used to compare with number of heartbeats from mgr*/

} EPS_AppData_t;

extern EPS_AppData_t EPS_AppData;

int32 EPS_app_init(void);
void EPS_process_command_packet(void);
void EPS_process_ground_command(void);
void EPS_report_housekeeping(void);
void EPS_reset_counters(void);
boolean EPS_verify_cmd_length(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

#endif

