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


#endif /* INC_LORA_TELEMETRY_H_ */
