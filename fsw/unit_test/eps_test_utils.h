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

#include <stdint.h>
#include <string.h>

/* libutil */
#include <util/error.h>

/* eps i2c params */
#define EPS_I2C_ADDRESS 0x2b
#define EPS_I2C_HANDLE 0
#define EPS_I2C_BUF_MAX 16

/* eps i2c cmds */
#define TEST_EPS_CMD_GET_BOARD_STATUS                0x01
#define TEST_EPS_CMD_GET_LAST_ERROR                  0x03
#define TEST_EPS_CMD_GET_VERSION                     0x04
#define TEST_EPS_CMD_GET_CHECKSUM                    0x05
#define TEST_EPS_CMD_GET_TELEMETRY                   0x10
#define TEST_EPS_CMD_GET_COM_WATCHDOG_PERIOD         0x20
#define TEST_EPS_CMD_SET_COM_WATCHDOG_PERIOD         0x21
#define TEST_EPS_CMD_RESET_COM_WATCHDOG              0x22
#define TEST_EPS_CMD_GET_NUMBER_OF_BOR               0x31
#define TEST_EPS_CMD_GET_NUMBER_OF_ASR               0x32
#define TEST_EPS_CMD_GET_NUMBER_OF_MR                0x33
#define TEST_EPS_CMD_GET_NUMBER_OF_CWDR              0x34
#define TEST_EPS_CMD_SWITCH_ON_ALL_PDM               0x40
#define TEST_EPS_CMD_SWITCH_OFF_ALL_PDM              0x41
#define TEST_EPS_CMD_GET_ACTUAL_STATE_ALL_PDM        0x42
#define TEST_EPS_CMD_GET_EXPECTED_STATE_ALL_PDM      0x43
#define TEST_EPS_CMD_GET_INIT_STATE_ALL_PDM          0x44
#define TEST_EPS_CMD_SET_ALL_PDM_TO_INIT_STATE       0x45
#define TEST_EPS_CMD_SWITCH_PDM_N_ON                 0x50 
#define TEST_EPS_CMD_SWITCH_PDM_N_OFF                0x51
#define TEST_EPS_CMD_SET_PDM_N_INIT_STATE_ON         0x52
#define TEST_EPS_CMD_SET_PDM_N_INIT_STATE_OFF        0x53
#define TEST_EPS_CMD_GET_PDM_N_ACTUAL_STATUS         0x54
#define TEST_EPS_CMD_SET_PDM_N_TIMER_LIMIT           0x60
#define TEST_EPS_CMD_GET_PDM_N_TIMER_LIMIT           0x61
#define TEST_EPS_CMD_GET_PDM_N_CURRENT_TIMER_VAL     0x62
#define TEST_EPS_CMD_PCM_RESET                       0x70 
#define TEST_EPS_CMD_MANUAL_RESET                    0x80

/* test setup/teardown */
void EPS_Test_Setup(void);
void EPS_Test_TearDown(void);

/* i2c data */
typedef struct
{
    uint8_t cmd_cnt;
    uint8_t txlen;
    uint8_t txbuf[EPS_I2C_BUF_MAX];
    uint8_t rxlen;
    const uint8_t* rxbuf;
    uint16_t timeout;
    int retcode;
} i2c_data_t;

/* eps app i2c data */
extern i2c_data_t eps_i2c_data;

