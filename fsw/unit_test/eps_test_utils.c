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

#include "eps_test_utils.h"

#include <eps_app.h>
#include <i2c_hooks.h>

#include <utassert.h>
#include <ut_cfe_es_stubs.h>
#include <ut_cfe_sb_stubs.h>
#include <ut_cfe_evs_stubs.h>
#include <ut_cfe_evs_hooks.h>
#include <ut_osapi_stubs.h>

extern int eps_cmd_mid_fail;
extern int eps_send_hk_fail;

/* prototypes */
static int eps_i2c_transaction(int handle, uint8_t addr, void * txbuf, uint8_t txlen,
                               void * rxbuf, uint8_t rxlen, uint16_t timeout);

/* eps i2c data */
i2c_data_t eps_i2c_data;
static uint8_t i2c_read_index = 0;

/* i2c hooks */
static i2c_hooks_t hooks = {
    .i2c_init_master_hook = NULL,
    .i2c_master_transaction_hook = eps_i2c_transaction,
};

/* i2c transaction hook */
/* this is a little more complicated for eps since it has to send 2 transactions (write/read) */
static int eps_i2c_transaction(int handle, uint8_t addr, void * txbuf, uint8_t txlen,
                               void * rxbuf, uint8_t rxlen, uint16_t timeout)
{
    /* verify buffers */
    if(txlen > 0) UtAssert_True(txbuf != NULL, "i2c txbuf != NULL");
    if(rxlen > 0) UtAssert_True(rxbuf != NULL, "i2c rxbuf != NULL");

    /* verify basic eps i2c params */
    UtAssert_True(handle == EPS_I2C_HANDLE, "eps i2c handle");
    UtAssert_True(addr == EPS_I2C_ADDRESS, "eps i2c address");

    /* save off i2c params */
    eps_i2c_data.cmd_cnt += 1;
    eps_i2c_data.timeout = timeout;

    /* save tx data for testing */
    if(txlen > 0)
    {
        uint16_t avail = EPS_I2C_BUF_MAX - eps_i2c_data.txlen;
        UtAssert_True(txlen <= avail, "i2c txbuf overflow");
        uint16_t len = (txlen > avail) ? avail : txlen;
        memcpy(eps_i2c_data.txbuf + eps_i2c_data.txlen, txbuf, len);
        eps_i2c_data.txlen += len;
    }

    /* only process on no error */
    if(eps_i2c_data.retcode == E_NO_ERR)
    {
        /* return rxbuf test data */
        if(rxlen > 0)
        {
            uint16_t avail = eps_i2c_data.rxlen - i2c_read_index;
            UtAssert_True(rxlen <= avail, "i2c rxbuf underflow");
            uint16_t len = (rxlen > avail) ? avail : rxlen;
            memcpy(rxbuf, eps_i2c_data.rxbuf + i2c_read_index, len);
            i2c_read_index += len;
        }
    }

    return eps_i2c_data.retcode;
}

void EPS_Test_Setup(void)
{
    /* initialize app data */
    CFE_PSP_MemSet(&EPS_AppData, 0, sizeof(EPS_AppData_t));

    /* initialize services */
    Ut_CFE_SB_Reset();
    Ut_CFE_ES_Reset();
    Ut_OSAPI_Reset();
    
	Ut_CFE_EVS_Reset();

    /* set i2c hooks */
    memset(&eps_i2c_data, 0, sizeof(i2c_data_t));
    i2c_read_index = 0;
    eps_i2c_data.retcode = E_NO_ERR;
    set_i2c_hooks(&hooks);
    
    eps_cmd_mid_fail = 0;
    eps_send_hk_fail = 0;
}

void EPS_Test_TearDown(void)
{
    set_i2c_hooks(NULL);
}

