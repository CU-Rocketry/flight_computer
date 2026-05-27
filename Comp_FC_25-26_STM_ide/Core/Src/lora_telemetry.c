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

#include "sx126x_hal.h"


//const void* ebyte; //need to declare this context for function use
extern SPI_HandleTypeDef hspi4; //RF spi


//setting up a read/write context for the sx126x radio
//unsure if this is actually needed or if I am tweaking

typedef struct {
    SPI_HandleTypeDef* hspi;       // Pointer to the STM32 SPI handle

    // GPIO Ports and Pins
    GPIO_TypeDef* cs_port;   // Chip Select (NSS) Port
    uint16_t      cs_pin;    // Chip Select Pin

    GPIO_TypeDef* busy_port;  // BUSY Port
    uint16_t           busy_pin;   // BUSY Pin

    GPIO_TypeDef* reset_port; // Reset Port
    uint16_t           reset_pin;  // Reset Pin

    GPIO_TypeDef* dio1_port;  // DIO1 Interrupt Port
    uint16_t           dio1_pin;   // DIO1 Interrupt Pin
} sx126x_ctx_t;

sx126x_ctx_t lora_radio;

static sx126x_ctx_t* radio;
//this will allow avoiding hardcoding into r/w functions, etc.

void LoRa_init(){

	//allow module to boot up by pulling rst pin low
	HAL_GPIO_WritePin(RF_RESET_GPIO_Port, RF_RESET_Pin, 0);
	HAL_Delay(100); // Hold low for at least 100 microseconds (20ms is safe)
	HAL_GPIO_WritePin(RF_RESET_GPIO_Port, RF_RESET_Pin, 1);
	HAL_Delay(20); // Hold low for at least 100 microseconds (20ms is safe)

	//try to test spi communication

	uint8_t cmd_buffer[4] = { 0x1D, 0x07, 0x41, 0x00};
	uint8_t register_value = 0x00;

	sx126x_hal_read(radio, cmd_buffer, 4, &register_value, 1);

	printf("Correct value: 36\r\n");
	printf("Read: %d\r\n", register_value);

//set device into standby mode for programming
	sx126x_set_standby(radio, SX126X_STANDBY_CFG_XOSC);

//set packet type (must be done first)
 sx126x_set_pkt_type(radio, 1);
 HAL_Delay(20);
	//confirm configuration of radio

	 sx126x_pkt_type_t pkt_type;
	 sx126x_get_pkt_type(radio, &pkt_type);

 if(pkt_type == 1){
 	 printf("Telemetry is in LoRa mode\r\n");
 	 printf("Continuing initialization...\r\n");

 	//to initialize
 	const uint32_t freq = 434000000;
 	sx126x_set_rf_freq(radio, freq);

 	int8_t pwr = 15; //power in dbm
 	sx126x_set_tx_params(radio, pwr, SX126X_RAMP_200_US);

 	    sx126x_mod_params_lora_t lora_params;

 	    lora_params.sf = SX126X_LORA_SF6;
 	    lora_params.bw = SX126X_LORA_BW_125;
 	    lora_params.cr = SX126X_LORA_CR_4_5;
 	    lora_params.ldro = 0;

 	sx126x_set_lora_mod_params(radio, &lora_params);

 	// sx126x_lora_cr_t cr;
 	// sx126x_get_lora_params_from_header(radio, *cr, crc);
 	//
 	// printf("CR mode:%d\r\n", cr);
  }
 	 else {
 		 printf("Telemetry is not in LoRa mode\r\n");
 		 printf("Telemetry mode: %d\r\n", pkt_type);
  }

 printf("Module configured.\r\n");
 printf("YESSSSSSSSSSSSS :) \r\n");
}

void sx126x_hal_wait_on_busy( const void* radio ){

	while(HAL_GPIO_ReadPin(RF_BUSY_GPIO_Port, RF_BUSY_Pin) == 1); //write while loop for while the pin is high (busy)
}

//TO DO
// write funtion for recieving/transmitting
//detect if channel is taken

//void packet_build(){} build packet function



void telemetry_tx(uint8_t packet){

	//configuring packet type
	sx126x_pkt_params_lora_t pkt_params;

	pkt_params.preamble_len_in_symb = 12; //reccomended for lower spreading factors
	pkt_params.header_type = 0; //include header in transmission
	pkt_params.pld_len_in_bytes = sizeof(packet);
	pkt_params.crc_is_on = 1; //crc off for now
	pkt_params.invert_iq_is_on = 0; //standard IQ setup

	sx126x_set_lora_pkt_params(radio, &pkt_params);

} //basic tx function

//steps:
//-> write buffer for data
//-> will need to configure packet
//-> transmit data
//-> reserve memory for rx
//-> open up for recieving


//void telemetry_rx(){} basic rx function

//writing a basic write/recieve script


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
			sx126x_hal_wait_on_busy(NULL);

			HAL_GPIO_WritePin(RF_CS_GPIO_Port, RF_CS_Pin, 0);
			status += HAL_SPI_Transmit(&hspi4, command, command_length, 1000); //this transmits the command (opcode)

			status += HAL_SPI_Transmit(&hspi4, data, data_length, 1000); //transmit data from device

			HAL_GPIO_WritePin(RF_CS_GPIO_Port, RF_CS_Pin, 1);

		return SX126X_HAL_STATUS_OK;

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


		HAL_StatusTypeDef status = HAL_OK;
		sx126x_hal_wait_on_busy(NULL);

		HAL_GPIO_WritePin(RF_CS_GPIO_Port, RF_CS_Pin, 0);
		status += HAL_SPI_Transmit(&hspi4, command, command_length, 1000); //this transmits the command (opcode, and register addr))

		//remove dummy byte for now
		//uint8_t dummy = 0x00;
		//status += HAL_SPI_Transmit(&hspi4, &dummy, 1, 1000); //have to send dummy byte for this chip

		status += HAL_SPI_Receive(&hspi4, data, data_length, 1000);
		HAL_GPIO_WritePin(RF_CS_GPIO_Port, RF_CS_Pin, 1);
		return SX126X_HAL_STATUS_OK;

}


