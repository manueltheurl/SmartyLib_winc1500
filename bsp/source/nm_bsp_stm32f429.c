
/**
 *
 * \file
 *
 * \brief This module contains SAMD21 BSP APIs implementation.
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

#include "bsp/include/nm_bsp.h"
#include "common/include/nm_common.h"
#include "conf_winc.h"
#include "stm32u5xx_hal.h"


/*
 *	@fn		init_chip_pins
 *	@brief	Initialize reset, chip enable and wake pin
 */
static void init_chip_pins(void)
{
    /* Initialize WiFi GPIO pins */
	// gpios will be initialized in main.c with cube generated code
    
    /* Set INIT value */
    //HAL_GPIO_WritePin(GPIOA,CONF_WINC_PIN_POWER_ENABLE,GPIO_PIN_RESET);
    //HAL_GPIO_WritePin(CONF_WINC_PORT_LEVEL_SHIFTER_ENABLE,CONF_WINC_PIN_LEVEL_SHIFTER_ENABLE,GPIO_PIN_SET);
    HAL_GPIO_WritePin(WIFI_ENABLE_PORT,WIFI_ENABLE_PIN,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(WIFI_RESET_PORT,WIFI_RESET_PIN,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(WIFI_SPI_CS_PORT,WIFI_SPI_CS_PIN,GPIO_PIN_SET);  // not chip selected


}

/*
 *	@fn		nm_bsp_init
 *	@brief	Initialize BSP
 *	@return	0 in case of success and -1 in case of failure
 */
sint8 nm_bsp_init(void)
{
	/* Initialize chip IOs. */
	init_chip_pins();

	/* Perform chip reset. */
	nm_bsp_reset();

	return M2M_SUCCESS;
}

/**
 *	@fn		nm_bsp_deinit
 *	@brief	De-Initialize BSP
 *	@return	0 in case of success and -1 in case of failure
 */
sint8 nm_bsp_deinit(void)
{
    /* De-Initialize WiFi GPIO pins */
     /* Reset the chip enable and chip reset pins */
    //HAL_GPIO_WritePin(GPIOA,CONF_WINC_PIN_POWER_ENABLE,GPIO_PIN_RESET);
    //HAL_GPIO_WritePin(CONF_WINC_PORT_LEVEL_SHIFTER_ENABLE,CONF_WINC_PIN_LEVEL_SHIFTER_ENABLE,GPIO_PIN_SET);
    //HAL_GPIO_WritePin(GPIOA,CONF_WINC_PIN_CHIP_ENABLE,GPIO_PIN_RESET);
	
    HAL_GPIO_WritePin(WIFI_ENABLE_PORT,WIFI_ENABLE_PIN,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(WIFI_RESET_PORT,WIFI_RESET_PIN,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(WIFI_SPI_CS_PORT,WIFI_SPI_CS_PIN,GPIO_PIN_SET);  // not chip selected
    nm_bsp_sleep(10);

	return M2M_SUCCESS;
}

/**
 *	@fn		nm_bsp_reset
 *	@brief	Reset NMC1500 SoC by setting CHIP_EN and RESET_N signals low,
 *           CHIP_EN high then RESET_N high
 */
void nm_bsp_reset(void)
{
    /* Set Power sequence of GPIO lines */
    /* -------------------------------- */

    /* Power enable (3.3V) - 3V3_DC2DC_EN output */
    //HAL_GPIO_WritePin(GPIOA,CONF_WINC_PIN_POWER_ENABLE,GPIO_PIN_SET);
    //HAL_Delay(100);

    /* Level Shifter Translate enable - LEVEL_TRNSLT_EN output */
    //HAL_GPIO_WritePin(CONF_WINC_PORT_LEVEL_SHIFTER_ENABLE,CONF_WINC_PIN_LEVEL_SHIFTER_ENABLE,GPIO_PIN_RESET);
    //HAL_Delay(100);

    /* Set CHIP enable */
   // HAL_GPIO_WritePin(GPIOA,CONF_WINC_PIN_CHIP_ENABLE,GPIO_PIN_SET);
	HAL_GPIO_WritePin(WIFI_ENABLE_PORT,WIFI_ENABLE_PIN,GPIO_PIN_SET);
    HAL_Delay(100);

    /* Set RSTN 1.8V */
    HAL_GPIO_WritePin(WIFI_RESET_PORT,WIFI_RESET_PIN,GPIO_PIN_SET);

    HAL_Delay(100);

}

/*
 *	@fn		nm_bsp_sleep
 *	@brief	Sleep in units of mSec
 *	@param[IN]	u32TimeMsec
 *				Time in milliseconds
 */
void nm_bsp_sleep(uint32 u32TimeMsec)
{
    /* use FreeRTOS delay */
    HAL_Delay(u32TimeMsec);
}

/*
 *	@fn		nm_bsp_register_isr
 *	@brief	Register interrupt service routine
 *	@param[IN]	pfIsr
 *				Pointer to ISR handler
 */
void nm_bsp_register_isr(tpfNmBspIsr pfIsr)
{
	// interrupt pin will be registered in main.c, with cube generated code

}

/*
 *	@fn		nm_bsp_interrupt_ctrl
 *	@brief	Enable/Disable interrupts
 *	@param[IN]	u8Enable
 *				'0' disable interrupts. '1' enable interrupts
 */
void nm_bsp_interrupt_ctrl(uint8 u8Enable)
{
    if (1 == u8Enable)
    {
        HAL_NVIC_SetPriority((IRQn_Type)(CONF_WINC_EXTI_IRQN), 0x01, 0);
        HAL_NVIC_EnableIRQ((IRQn_Type)(CONF_WINC_EXTI_IRQN));
    }
    else
    {
        HAL_NVIC_DisableIRQ((IRQn_Type)(CONF_WINC_EXTI_IRQN));
    }
}
