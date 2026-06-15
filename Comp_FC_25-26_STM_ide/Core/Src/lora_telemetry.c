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
#include "state.h"
#include "state_estimation.h"
#include <string.h>
#include <stdio.h>

#include "sx126x_hal.h"
#include "packets.h"

#include "lora_telemetry.h"


//const void* ebyte; //need to declare this context for function use
extern SPI_HandleTypeDef hspi4; //RF spi


//setting up a read/write context for the sx126x radio
//unsure if this is actually needed or if I am tweaking

sx126x_ctx_t lora_radio;


static sx126x_ctx_t* radio;
//this will allow avoiding hardcoding into r/w functions, etc.

void LoRa_init(sx126x_ctx_t* context){

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

 //set interrupt parameters

 sx126x_clear_irq_status( radio, SX126X_IRQ_ALL );

 	 sx126x_set_dio_irq_params(
 	         radio, SX126X_IRQ_ALL,
 	         SX126X_IRQ_TX_DONE | SX126X_IRQ_RX_DONE | SX126X_IRQ_TIMEOUT | SX126X_IRQ_HEADER_ERROR | SX126X_IRQ_CRC_ERROR,
 	         SX126X_IRQ_NONE, SX126X_IRQ_NONE );

 	 sx126x_clear_irq_status( radio, SX126X_IRQ_ALL ); //clear status of interrupt pin


 printf("Module configured.\r\n");
 printf("YESSSSSSSSSSSSS :) \r\n");
 printf("Starting radio in continous rx mode...\r\n");

 int rx_flag = 0;
 sx126x_set_rx(radio, 0xFFFFFF); //set radio to continous rx mode

}

void sx126x_hal_wait_on_busy( const void* radio ){

	while(HAL_GPIO_ReadPin(RF_BUSY_GPIO_Port, RF_BUSY_Pin) == 1); //write while loop for while the pin is high (busy)
}



void telemetry_tx(const void *packet, sx126x_ctx_t *context)
{

	sx126x_set_standby(context, SX126X_STANDBY_CFG_XOSC); // ensure radio in standby mode before changning any settings

	// configuring packet type
	sx126x_pkt_params_lora_t pkt_params;

	pkt_params.preamble_len_in_symb = 12;			   // reccomended for lower spreading factors
	pkt_params.header_type = SX126X_LORA_PKT_EXPLICIT; // include header in transmission
	pkt_params.pld_len_in_bytes = sizeof(packet);
	pkt_params.crc_is_on = 1;		// crc off for now
	pkt_params.invert_iq_is_on = 0; // standard IQ setup

	sx126x_set_lora_pkt_params(context, &pkt_params); // setup packets

	// writing buffer for transmission into radio

	// package up packet into buffer

	static uint8_t buffer_tx[sizeof(packet)];
	memcpy(buffer_tx, &packet, sizeof(packet));
	sx126x_write_buffer(context, 0, buffer_tx, sizeof(packet));

	sx126x_set_tx(context, 0); // transmit

	// need to poll interrupt to see if transmit is done
	sx126x_irq_mask_t irq_status;

	while (irq_status != SX126X_IRQ_TX_DONE) // tell when done
	{
		sx126x_get_irq_status(context, &irq_status);
	}
}

//FOR RX
//need to poll interrupt pin -> do this by calling function in loop
//if detect interrupt on RX line -> decode
//may only need to do this on ground station

//typedef struct {
//
//	uint8_t pkt_type; // always 0x03 for cmd
//
//	    uint8_t mode_en; // requests mode change (substitute for rotating selector)
//	    uint8_t mode; // mode to change to
//
//	    uint8_t launch_detect_en; // set to 1 to trigger launch detect
//
//
//} command_packet_t;
//
//
//// TODO make function to enable radio config switch in idle mode if radio config is enabled
//
//typedef struct {
//
//	uint8_t pkt_type; //0x04 for radio packet
//
//	uint8_t reconfig;
//
//	uint8_t pkt_change_en;
//	sx126x_pkt_type_t rf_pkt_type;
//
//	// lora params for the mod params struct	//TODO if want to configure right, will need to check what type of packet we are trying to config... later
//	uint8_t lora_params_en;
//
//	sx126x_lora_sf_t sf;
//	sx126x_lora_bw_t bw;
//	sx126x_lora_cr_t cr;
//
//	uint8_t tx_params_en;
//	uint8_t pwr;
//
//    uint8_t set_rf_freq_en;
//	uint32_t freq;
//
//
//} radio_config; //this can be more fleshed out but is it for now

void telemetry_rx_decode(void *context){ //function to decode rx packets

	//decoding scheme for each type of packet, for fc this is command only, gs is telemetry data

//	void telemetry_parse_rx(cobs_uart_t *port, state_t *state) {
//	    uint8_t decoded_buf[256]; // set new length of longest packet
//	    uint16_t decoded_len = cobs_decode(port->rx_buf, port->rx_idx, decoded_buf); //idk what this command does tbh
//
//	    if (decoded_len > 0) {
//	        uint8_t pkt_type = decoded_buf[0];
//
//	        if (pkt_type == PKT_TYPE_CMD && decoded_len == sizeof(command_packet_t)) {
//				command_packet_t *cmd = (command_packet_t *)decoded_buf;
//
//			}
//	    }
//	}

	sx126x_rx_buffer_status_t *rx_buf_status;
	sx126x_get_rx_buffer_status(radio, rx_buf_status); //get buf status, pos and length

	uint8_t rx_len = rx_buf_status->pld_len_in_bytes;
	uint8_t buf_start = rx_buf_status->buffer_start_pointer;

	uint8_t rx_buf[rx_len];
	uint8_t pkt_type;

	if (rx_len > 0){
		pkt_type = rx_buf[0];
	};

	sx126x_read_buffer( &context, buf_start, rx_buf, rx_len );

	sx126x_pkt_status_lora_t pkt_status;
	sx126x_get_lora_pkt_status(&radio, &pkt_status);

	int8_t rssi_last_packet;
	rssi_last_packet = pkt_status.rssi_pkt_in_dbm;

	//rx for flight computer just needs to recieve command and radio configs
	if (pkt_type == PKT_TYPE_CMD && rx_len == sizeof(command_packet_t)){
		command_packet_t *cmd = (command_packet_t *)rx_buf;

		global_state.mode_override_en = cmd->mode_en; 					//detect change in mode in main loop
		global_state.mode_override = cmd->mode;

		if (cmd->launch_detect_en) {
						launch_detect_override(1);
		}																// detect change in launch
	} else if (pkt_type == PKT_TYPE_RADIO_CONFIG){
		radio_config_packet_t *radio_config = (radio_config_packet_t *)rx_buf;

		if(radio_config->reconfig){
			sx126x_set_standby(radio, SX126X_STANDBY_CFG_XOSC);
		}

		if (radio_config->lora_params_en){


	 	    sx126x_mod_params_lora_t reconfig_lora_params;

	 	   reconfig_lora_params.sf = radio_config->sf;
	 	   reconfig_lora_params.bw = radio_config->bw;
	 	   reconfig_lora_params.cr = radio_config->cr;
	 	   reconfig_lora_params.ldro = 0;

	 	    sx126x_set_lora_mod_params(radio, &reconfig_lora_params);

		}

		if (radio_config->tx_params_en){

			sx126x_set_tx_params(radio, radio_config->pwr, SX126X_RAMP_200_US);
		}

		if (radio_config->freq_en){
			sx126x_set_rf_freq(radio, radio_config->freq);
		}
	}
 //
};

void radio_reconfig(radio_config_packet_t *reconfig, sx126x_ctx_t *context)
{ // reconfig here is just the current_rf global state variable
	// check for reconfig flag just to double check
	if (radio_reconfig_flag == 1)
	{

		if (reconfig->device == context->device)
		{ // checks if reconfig device is the same as the current device
			// apply reconfigs to current radio
			sx126x_set_standby(&context, SX126X_STANDBY_CFG_XOSC);

			if (reconfig->lora_params_en == 1)
			{
				sx126x_mod_params_lora_t lora_params;

				lora_params.sf = SX126X_LORA_SF6;
				lora_params.bw = SX126X_LORA_BW_125;
				lora_params.cr = SX126X_LORA_CR_4_5;
				lora_params.ldro = 0;

				sx126x_set_lora_mod_params(context, &lora_params);
			}

			if (reconfig->freq_en == 1)
			{

				uint32_t freq;
				freq = reconfig->freq;

				sx126x_set_rf_freq(context, freq);
			}

			if (reconfig->tx_params_en == 1)
			{

				uint32_t pwr;
				pwr = reconfig->pwr;

				sx126x_set_tx_params(context, pwr, SX126X_RAMP_200_US);
			}
		}
		else if (reconfig->device != context->device)
		{ // if not equal to current device
			// send rf config telemetry packet to the needed radio
			telemetry_tx(reconfig, context); // send telemetry
		}

		radio_reconfig_flag = 0; // set flag to zero to demonstrated that the reconfig is done
	}
	else if (radio_reconfig_flag == 0)
	{ // do not reconfig, just skip
		return;
	}
}




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



void sx126x_irq_process( const void* context)
{

        sx126x_irq_mask_t irq_regs;
        sx126x_get_and_clear_irq_status( context, &irq_regs );

        if( ( irq_regs & SX126X_IRQ_TX_DONE ) == SX126X_IRQ_TX_DONE )
        {
            printf( "Tx done\n" );

            //after tx done, set radio back to continous polling
           	sx126x_set_rx(radio, 0xFFFFFF);


        }

        if( ( irq_regs & SX126X_IRQ_RX_DONE ) == SX126X_IRQ_RX_DONE )
        {
            printf( "Rx done\n" );
            telemetry_rx_decode(&context);


        }

        if( ( irq_regs & SX126X_IRQ_PREAMBLE_DETECTED ) == SX126X_IRQ_PREAMBLE_DETECTED )
        {
            printf( "Preamble detected\n" );

        }

        if( ( irq_regs & SX126X_IRQ_SYNC_WORD_VALID ) == SX126X_IRQ_SYNC_WORD_VALID )
        {
            printf( "Syncword valid\n" );

        }

        if( ( irq_regs & SX126X_IRQ_HEADER_VALID ) == SX126X_IRQ_HEADER_VALID )
        {
            printf( "Header valid\n" );

        }

        if( ( irq_regs & SX126X_IRQ_HEADER_ERROR ) == SX126X_IRQ_HEADER_ERROR )
        {
            printf( "Header error\n" );

        }

        if( ( irq_regs & SX126X_IRQ_CRC_ERROR ) == SX126X_IRQ_CRC_ERROR )
        {
            printf( "CRC error\n" );

        }

        if( ( irq_regs & SX126X_IRQ_CAD_DONE ) == SX126X_IRQ_CAD_DONE )
        {
            printf( "CAD done\n" );
            if( ( irq_regs & SX126X_IRQ_CAD_DETECTED ) == SX126X_IRQ_CAD_DETECTED )
            {
                printf( "Channel activity detected\n" );

            }
            else
            {
                printf( "No channel activity detected\n" );

            }
        }

        if( ( irq_regs & SX126X_IRQ_TIMEOUT ) == SX126X_IRQ_TIMEOUT )
        {
            printf( "Rx timeout\n" );

        }

        if( ( irq_regs & SX126X_IRQ_LR_FHSS_HOP ) == SX126X_IRQ_LR_FHSS_HOP )
        {
            printf( "FHSS hop done\n" );

        }

}

