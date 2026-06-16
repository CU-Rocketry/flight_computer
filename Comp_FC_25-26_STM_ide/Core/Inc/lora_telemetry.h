/*
 * lora_telemetry.h
 *
 *  Created on: May 25, 2026
 *      Author: haileymeagher
 */

#ifndef INC_LORA_TELEMETRY_H_
#define INC_LORA_TELEMETRY_H_

#include "sx126x.h"
#include "packets.h"
#include "state.h"

extern telemetry_packet_t telemetry_rx;

typedef struct {
    // GPIO Ports and Pins
      //pointer to spi class
	SPI_HandleTypeDef* hspi;
    rf_device_t         device;
} sx126x_ctx_t;


void telemetry_tx_string(const char *message, sx126x_ctx_t *context);
void LoRa_init(sx126x_ctx_t* context);
void telemetry_rx_decode(void *context);
void telemetry_tx(const void *packet, sx126x_ctx_t *context);
void rf_int_drdy_handler(void);
void radio_reconfig( radio_config_packet_t* reconfig, sx126x_ctx_t* context);
void sx126x_irq_process(  const void* context);

#endif /* INC_LORA_TELEMETRY_H_ */

