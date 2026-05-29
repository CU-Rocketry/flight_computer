/*
 * lora_telemetry.h
 *
 *  Created on: May 25, 2026
 *      Author: haileymeagher
 */

#ifndef INC_LORA_TELEMETRY_H_
#define INC_LORA_TELEMETRY_H_

void LoRa_init(void);
void telemetry_tx(void);
void packet_build(void);
void telemetry_rx(void);
void rf_int_drdy_handler(void);
void sx126x_irq_process(void);


#endif /* INC_LORA_TELEMETRY_H_ */
