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

const void* ebyte;

void LoRa_init(){


//set packet type (must be done first)

 sx126x_pkt_type_t pkt;
 pkt = SX126X_PKT_TYPE_LORA;

sx126x_set_pkt_type(ebyte, pkt);

}
