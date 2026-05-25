/*
 * lora_telemetry.c
 *
 *  Created on: Apr 11, 2026
 *      Author: haileymeagher
 */

//operation of E22 433Mhz Lora module
//sx126 chip
#include "sx126x.h"

#include "main.h"
#include <string.h>
#include <stdio.h>

const void* ebyte; //need to declare this context for function use


//setting up a read/write context for the sx126x radio
//unsure if this is actually needed or if I am tweaking

typedef int32_t (*sx126x_write_ptr)(
		void *context,
		    uint8_t *command,
		    uint16_t command_length,
		    uint8_t *data,
		    uint16_t data_length);

typedef int32_t (*sx126x_read_ptr)(
    void *context,
    uint8_t *command,
    uint16_t command_length,
    uint8_t *data,
    uint16_t data_length);

typedef struct
{
  /** Component mandatory fields **/
  sx126x_write_ptr  write_reg;
  sx126x_read_ptr   read_reg;
  /** Component optional fields **/
  /** Customizable optional pointer **/
  void *context;

  /** private data **/
  void *priv_data;
} sx126x_ctx_t;



void LoRa_init(){

//set packet type (must be done first)

 sx126x_pkt_type_t pkt;
 pkt = SX126X_PKT_TYPE_LORA;

sx126x_set_pkt_type(ebyte, pkt);

}


//for tomorrow:
//write and read to a register
//configure radio frequency and LoRa parameters
//send some sort of data over rf



/**
 * Radio data transfer - write
 *
 * @remark Shall be implemented by the user
 *
 * @param [in] context          Radio implementation parameters
 * @param [in] command          Pointer to the buffer to be transmitted
 * @param [in] command_length   Buffer size to be transmitted
 * @param [in] data             Pointer to the buffer to be transmitted
 * @param [in] data_length      Buffer size to be transmitted
 *
 * @returns Operation status
 */
sx126x_hal_status_t sx126x_hal_write( const void* context, const uint8_t* command, const uint16_t command_length,
                                      const uint8_t* data, const uint16_t data_length ) {

	HAL_StatusTypeDef status = HAL_OK;
		HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, 0);
		status += HAL_SPI_Transmit(handle, &reg, 1, 1000);
		status += HAL_SPI_Transmit(handle, bufp, len, 1000);
		HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, 1);
		return status;

}

/**
 * Radio data transfer - read
 *
 * @remark Shall be implemented by the user
 *
 * @param [in] context          Radio implementation parameters
 * @param [in] command          Pointer to the buffer to be transmitted
 * @param [in] command_length   Buffer size to be transmitted
 * @param [in] data             Pointer to the buffer to be received
 * @param [in] data_length      Buffer size to be received
 *
 * @returns Operation status
 */
sx126x_hal_status_t sx126x_hal_read( const void* context, const uint8_t* command, const uint16_t command_length,
                                     uint8_t* data, const uint16_t data_length ){

	const sx126x_hal_context_t* sx126x_context = ( const sx126x_hal_context_t* ) context;

	reg |= 0x80; // set MSB for read

		HAL_StatusTypeDef status = HAL_OK;
		HAL_GPIO_WritePin(RF_CS_GPIO_Port, MAG_CS_Pin, 0);
		status += HAL_SPI_Transmit(handle, &reg, 1, 1000);
		status += HAL_SPI_Receive(handle, bufp, len, 1000);
		HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, 1);
		return status;

}

sx126x_hal_status_t sx126x_hal_write( const void* context, const uint8_t* command, const uint16_t command_length,
                                      const uint8_t* data, const uint16_t data_length )

{
    const sx126x_hal_context_t* sx126x_context = ( const sx126x_hal_context_t* ) context;

    sx126x_hal_wait_on_busy( sx126x_context );

    smtc_hal_mcu_gpio_set_state( sx126x_context->nss.inst, SMTC_HAL_MCU_GPIO_STATE_LOW );
    smtc_hal_mcu_spi_rw_buffer( sx126x_context->spi.inst, command, NULL, command_length );
    smtc_hal_mcu_spi_rw_buffer( sx126x_context->spi.inst, data, NULL, data_length );
    smtc_hal_mcu_gpio_set_state( sx126x_context->nss.inst, SMTC_HAL_MCU_GPIO_STATE_HIGH );

    return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_read( const void* context, const uint8_t* command, const uint16_t command_length,
                                     uint8_t* data, const uint16_t data_length )
{
    const sx126x_hal_context_t* sx126x_context = ( const sx126x_hal_context_t* ) context;

    sx126x_hal_wait_on_busy( sx126x_context );

    smtc_hal_mcu_gpio_set_state( sx126x_context->nss.inst, SMTC_HAL_MCU_GPIO_STATE_LOW );
    smtc_hal_mcu_spi_rw_buffer( sx126x_context->spi.inst, command, NULL, command_length );
    smtc_hal_mcu_spi_rw_buffer( sx126x_context->spi.inst, NULL, data, data_length );
    smtc_hal_mcu_gpio_set_state( sx126x_context->nss.inst, SMTC_HAL_MCU_GPIO_STATE_HIGH );

    return SX126X_HAL_STATUS_OK;
}
