/**
 *
 * \file
 *
 * \brief This module contains NMC1000 bus wrapper APIs implementation.
 *
 * Copyright (c) 2016-2017 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>     /* Included for uint_t */
#include "bsp/include/nm_bsp.h"
#include "common/include/nm_common.h"
#include "bus_wrapper/include/nm_bus_wrapper.h"
#include "conf_winc.h"
#include "stm32u5xx_hal.h"
#include "LibIncludes/LibIncludes.h"

#define NM_BUS_MAX_TRX_SZ	256

/* spi_rw variables */
static uint8 spiDummyBuf[300] = {0};

tstrNmBusCapabilities egstrNmBusCapabilities =
{
	NM_BUS_MAX_TRX_SZ
};

#ifdef CONF_WINC_USE_SPI
/*
*	@fn		spi_select_slave
*	@brief	Select slave chip select: true - select, false - deselect
*	@return	None
*/
static void spi_select_slave(const uint8_t select)
{
    if (select)
    {
        HAL_GPIO_WritePin(SPI_WIFI_CS_GPIO_PORT,SPI_WIFI_CS_PIN,GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(SPI_WIFI_CS_GPIO_PORT,SPI_WIFI_CS_PIN,GPIO_PIN_SET);
    }
}

/*
*	@fn		spi_rw
*	@brief	transmit and/or receive data buffer via spi
*	@return	status
*/
static sint8 spi_rw(uint8* pu8Mosi, uint8* pu8Miso, uint16 u16Sz)
{
   HAL_StatusTypeDef status;
   
    /* Start SPI transaction - polling method */
  	spi_select_slave(true);
    
    
    /* Transmit/Recieve */
    if (pu8Mosi == NULL)
	{
		status = HAL_SPI_TransmitReceive(&WIFI_SPI_HANDLE,spiDummyBuf,pu8Miso,u16Sz,1000);
    }
    else if(pu8Miso == NULL)
    {
        status = HAL_SPI_TransmitReceive(&WIFI_SPI_HANDLE,pu8Mosi,spiDummyBuf,u16Sz,1000);
        memset(spiDummyBuf,0, u16Sz);
    }
    else
    {     
        status = HAL_SPI_TransmitReceive(&WIFI_SPI_HANDLE,pu8Mosi,pu8Miso,u16Sz,1000);
    } 
    
    /* Handle Transmit/Recieve error. Deselect before returning even on failure - leaving
     * WIFI_nCS asserted here left it low forever (nothing else ever raises it), so every
     * later SPI2 transaction - including the ExtFlash driver's, which shares this bus, see
     * ExtFlash.h "Bus sharing" - was also clocked into the WINC1500, corrupting both. Local
     * patch to vendored code; see TODO.md "SPI2 sharing between ExtFlash and WiFi..."
     * (2026-08-31). */
    spi_select_slave(false);

    if (status != HAL_OK)
    {
        M2M_ERR("%s: HAL_SPI_TransmitReceive failed. error (%d)\n",__FUNCTION__,status);
        return status;
    }

	return M2M_SUCCESS;
}
#endif //CONF_WINC_USE_SPI

void nm_bus_wifi_spi_init(SPI_HandleTypeDef *hspi)
{
	// will be done in main.c, cube generated stuff
}


/*
*	@fn		nm_bus_init
*	@brief	Initialize the bus wrapper
*	@return	M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
*/
sint8 nm_bus_init(void *pvinit)
{
	sint8 result = M2M_SUCCESS;
	// init is called by main.c in cube generated code!
	// HAL_SPI_MspInit(&WIFI_SPI_HANDLE);
	return result;
}


/*
*	@fn		nm_bus_ioctl
*	@brief	send/receive from the bus
*	@param[IN]	u8Cmd
*					IOCTL command for the operation
*	@param[IN]	pvParameter
*					Arbitrary parameter depenging on IOCTL
*	@return	M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
*	@note	For SPI only, it's important to be able to send/receive at the same time
*/
sint8 nm_bus_ioctl(uint8 u8Cmd, void* pvParameter)
{
	sint8 s8Ret = 0;
	switch(u8Cmd)
	{
		case NM_BUS_IOCTL_RW: {
			tstrNmSpiRw *pstrParam = (tstrNmSpiRw *)pvParameter;
			s8Ret = spi_rw(pstrParam->pu8InBuf, pstrParam->pu8OutBuf, pstrParam->u16Sz);
		}
		break;
		default:
			s8Ret = -1;
			M2M_ERR("invalide ioclt cmd\n");
			break;
	}

	return s8Ret;
}

/*
*	@fn		nm_bus_deinit
*	@brief	De-initialize the bus wrapper
*/
sint8 nm_bus_deinit(void)
{
	return M2M_SUCCESS;
}

/*
*	@fn			nm_bus_reinit
*	@brief		re-initialize the bus wrapper
*	@param [in]	void *config
*					re-init configuration data
*	@return		M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
*/
sint8 nm_bus_reinit(void* config)
{
	return M2M_SUCCESS;
}

