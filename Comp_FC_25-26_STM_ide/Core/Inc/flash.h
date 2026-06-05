/*
 * flash.h
 *
 *  Created on: Jun 4, 2026
 *      Author: haileymeagher
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include "state.h"
#include "packets.h"
#include "stm32h7xx_hal.h"

#define FLASH_CMD_WRITE_ENABLE       0x06
#define FLASH_CMD_READ_STATUS_1      0x05
#define FLASH_CMD_READ_JEDEC_ID      0x9F
#define FLASH_CMD_READ_DATA          0x03
#define FLASH_CMD_PAGE_PROGRAM       0x02
#define FLASH_CMD_SECTOR_ERASE_4K    0x20
#define FLASH_CMD_CHIP_ERASE         0xC7

#define FLASH_SR1_BUSY               0x01 // Write In Progress bit

#define W25Q32JV_JEDEC_ID 0xEF4016 // 0xEF for Winbond, 0x40 memory type, 0x16 capacity

typedef struct {
    OSPI_HandleTypeDef *hospi;

    uint32_t address; // current write address
    uint8_t full; // 0 if flash not full, 1 if full
    uint8_t prescaler_max; // 1, 10, or 100 for 100 Hz, 10 Hz, or 1 Hz logging, respectively
    uint8_t prescaler_cnt;
    flash_packet_t dma_packet;
} flash_t;

// Flash hardware driver
uint32_t flash_read_jedec_id(flash_t *flash);
uint8_t flash_read_status(flash_t *flash);
void flash_wait_for_ready(flash_t *flash);
uint8_t flash_is_ready(flash_t *flash);
void flash_write_enable(flash_t *flash);
void flash_erase_sector(flash_t *flash, uint32_t address);
void flash_erase_chip(flash_t *flash);
void flash_write_page(flash_t *flash, uint32_t address, uint8_t *data, uint32_t length);
void flash_write_page_dma(flash_t *flash, uint32_t address, uint8_t *data, uint32_t length);
void flash_read_data(flash_t *flash, uint32_t address, uint8_t *data, uint32_t length);

// Flash abstraction
void flash_packet_build(const state_t *current_state, flash_packet_t *packet);
uint8_t flash_check_erased(flash_t *flash);
void flash_packet_write(flash_t *flash, const state_t *state);
void flash_counters_reset(flash_t *flash);


#endif /* INC_FLASH_H_ */
