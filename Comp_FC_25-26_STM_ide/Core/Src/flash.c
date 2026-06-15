/*
 * flash.c
 *
 *  Created on: Jun 4, 2026
 *      Author: haileymeagher
 */


#include "flash.h"
#include <string.h>
#include "state.h"
#include "packets.h"
#include <stdio.h>


// Flash hardware driver

// Reads JEDEC ID with 1-1-1 standard SPI mode
uint32_t flash_read_jedec_id(flash_t *flash) {
    XSPI_RegularCmdTypeDef sCommand = {0};
    uint8_t id_buf[3] = {0};

    // Configure the command for standard 1-bit SPI (1-1-1)
    sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
//    sCommand.FlashId            = HAL_QSPI_FLASH_ID_1; //not needed for modern xspi system
    sCommand.Instruction        = FLASH_CMD_READ_JEDEC_ID;
    sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionWidth    = HAL_XSPI_INSTRUCTION_8_BITS;
    sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE; // Read ID doesn't need an address
    sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
    sCommand.DataMode           = HAL_XSPI_DATA_1_LINE;  // Receive data on 1 line
    sCommand.DataLength         = 3;                     // We expect 3 bytes back
    sCommand.DummyCycles        = 0;
    sCommand.DQSMode            = HAL_XSPI_DQS_DISABLE;
    sCommand.SIOOMode           = HAL_XSPI_SIOO_INST_EVERY_CMD;

    // Send the instruction
    if (HAL_XSPI_Command(flash->hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return 0; // Command failed
    }

    // Receive the data
    if (HAL_XSPI_Receive(flash->hxspi, id_buf, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return 0; // Receive failed
    }

    // Combine the 3 bytes into a 32-bit integer (Manufacturer ID << 16 | Memory Type << 8 | Capacity)
    return (id_buf[0] << 16) | (id_buf[1] << 8) | id_buf[2];
}

uint8_t flash_read_status(flash_t *flash) {
    XSPI_RegularCmdTypeDef sCommand = {0};
    uint8_t status;

    sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
//    sCommand.FlashId            = HAL_XSPI_FLASH_ID_1; not needed
    sCommand.Instruction        = FLASH_CMD_READ_STATUS_1;
    sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionWidth    = HAL_XSPI_INSTRUCTION_8_BITS;
    sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
    sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
    sCommand.DataMode           = HAL_XSPI_DATA_1_LINE;
    sCommand.DataLength             = 1;
    sCommand.DummyCycles        = 0;
    sCommand.DQSMode            = HAL_XSPI_DQS_DISABLE;
    sCommand.SIOOMode           = HAL_XSPI_SIOO_INST_EVERY_CMD;

    HAL_XSPI_Command(flash->hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE);
    HAL_XSPI_Receive(flash->hxspi, &status, HAL_XSPI_TIMEOUT_DEFAULT_VALUE);

    return status;
}

void flash_wait_for_ready(flash_t *flash) {
    while ((flash_read_status(flash) & FLASH_SR1_BUSY) == FLASH_SR1_BUSY) { // poll busy bit until ready
        // TODO do we need a delay
    }
}

// 1 if busy, 0 if ready
uint8_t flash_is_ready(flash_t *flash) {
    return ((flash_read_status(flash) & FLASH_SR1_BUSY) == FLASH_SR1_BUSY);
}

void flash_write_enable(flash_t *flash) {
    XSPI_RegularCmdTypeDef sCommand = {0};

    sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
    //sCommand.FlashId            = HAL_XSPI_FLASH_ID_1;
    sCommand.Instruction        = FLASH_CMD_WRITE_ENABLE;
    sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionWidth    = HAL_XSPI_INSTRUCTION_8_BITS;
    sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
    sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
    sCommand.DataMode           = HAL_XSPI_DATA_NONE;
    sCommand.DummyCycles        = 0;
    sCommand.DQSMode            = HAL_XSPI_DQS_DISABLE;
    sCommand.SIOOMode           = HAL_XSPI_SIOO_INST_EVERY_CMD;

    HAL_XSPI_Command(flash->hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE);
}

void flash_erase_sector(flash_t *flash, uint32_t address) {
    XSPI_RegularCmdTypeDef sCommand = {0};

    flash_write_enable(flash);

    sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
    //sCommand.FlashId            = HAL_XSPI_FLASH_ID_1;
    sCommand.Instruction        = FLASH_CMD_SECTOR_ERASE_4K;
    sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionWidth    = HAL_XSPI_INSTRUCTION_8_BITS;
    sCommand.AddressMode        = HAL_XSPI_ADDRESS_1_LINE;
    sCommand.AddressWidth        = HAL_XSPI_ADDRESS_24_BITS;
    sCommand.Address            = address;
    sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
    sCommand.DataMode           = HAL_XSPI_DATA_NONE;
    sCommand.DummyCycles        = 0;
    sCommand.DQSMode            = HAL_XSPI_DQS_DISABLE;
    sCommand.SIOOMode           = HAL_XSPI_SIOO_INST_EVERY_CMD;

    HAL_XSPI_Command(flash->hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE);

    // Wait for the erase to complete before returning
    flash_wait_for_ready(flash);
}

void flash_erase_chip(flash_t *flash) {
    XSPI_RegularCmdTypeDef sCommand = {0};

    flash_write_enable(flash);

    sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
    //sCommand.FlashId            = HAL_XSPI_FLASH_ID_1;
    sCommand.Instruction        = FLASH_CMD_CHIP_ERASE;
    sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionWidth    = HAL_XSPI_INSTRUCTION_8_BITS;
    sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
    sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
    sCommand.DataMode           = HAL_XSPI_DATA_NONE;
    sCommand.DummyCycles        = 0;
    sCommand.DQSMode            = HAL_XSPI_DQS_DISABLE;
    sCommand.SIOOMode           = HAL_XSPI_SIOO_INST_EVERY_CMD;

    HAL_XSPI_Command(flash->hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE);

    // WARNING: Chip erase can take up to 40 seconds!
    flash_wait_for_ready(flash);
}

void flash_write_page(flash_t *flash, uint32_t address, uint8_t *data, uint32_t length) {
    XSPI_RegularCmdTypeDef sCommand = {0};

    // Ensure we don't try to write across a page boundary (256 bytes)
    if (length > 256) length = 256;

    flash_write_enable(flash);

    sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
    //sCommand.FlashId            = HAL_XSPI_FLASH_ID_1;
    sCommand.Instruction        = FLASH_CMD_PAGE_PROGRAM;
    sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionWidth    = HAL_XSPI_INSTRUCTION_8_BITS;
    sCommand.AddressMode        = HAL_XSPI_ADDRESS_1_LINE;
    sCommand.AddressWidth        = HAL_XSPI_ADDRESS_24_BITS;
    sCommand.Address            = address;
    sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
    sCommand.DataMode           = HAL_XSPI_DATA_1_LINE;
    sCommand.DataLength             = length;
    sCommand.DummyCycles        = 0;
    sCommand.DQSMode            = HAL_XSPI_DQS_DISABLE;
    sCommand.SIOOMode           = HAL_XSPI_SIOO_INST_EVERY_CMD;

    HAL_XSPI_Command(flash->hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE);
    HAL_XSPI_Transmit(flash->hxspi, data, HAL_XSPI_TIMEOUT_DEFAULT_VALUE);

    // Wait for the write to complete
    flash_wait_for_ready(flash);
}

void flash_write_page_dma(flash_t *flash, uint32_t address, uint8_t *data, uint32_t length) {
    XSPI_RegularCmdTypeDef sCommand = {0};

    if (length > 256) length = 256;

    flash_wait_for_ready(flash); // ensure write is already done

    flash_write_enable(flash);

    sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
    //sCommand.FlashId            = HAL_XSPI_FLASH_ID_1;
    sCommand.Instruction        = FLASH_CMD_PAGE_PROGRAM;
    sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionWidth    = HAL_XSPI_INSTRUCTION_8_BITS;
    sCommand.AddressMode        = HAL_XSPI_ADDRESS_1_LINE;
    sCommand.AddressWidth        = HAL_XSPI_ADDRESS_24_BITS;
    sCommand.Address            = address;
    sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
    sCommand.DataMode           = HAL_XSPI_DATA_1_LINE;
    sCommand.DataLength            = length;
    sCommand.DummyCycles        = 0;
    sCommand.DQSMode            = HAL_XSPI_DQS_DISABLE;
    sCommand.SIOOMode           = HAL_XSPI_SIOO_INST_EVERY_CMD;

    HAL_XSPI_Command(flash->hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE);

    HAL_XSPI_Transmit_DMA(flash->hxspi, data); // start DMA transmission (non blocking)
}

void flash_read_data(flash_t *flash, uint32_t address, uint8_t *data, uint32_t length) {
    XSPI_RegularCmdTypeDef sCommand = {0};

    sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
    //sCommand.FlashId            = HAL_XSPI_FLASH_ID_1;
    sCommand.Instruction        = FLASH_CMD_READ_DATA;
    sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionWidth    = HAL_XSPI_INSTRUCTION_8_BITS;
    sCommand.AddressMode        = HAL_XSPI_ADDRESS_1_LINE;
    sCommand.AddressWidth        = HAL_XSPI_ADDRESS_24_BITS;
    sCommand.Address            = address;
    sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
    sCommand.DataMode           = HAL_XSPI_DATA_1_LINE;
    sCommand.DataLength             = length;
    sCommand.DummyCycles        = 0;
    sCommand.DQSMode            = HAL_XSPI_DQS_DISABLE;
    sCommand.SIOOMode           = HAL_XSPI_SIOO_INST_EVERY_CMD;

    HAL_XSPI_Command(flash->hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE);
    HAL_XSPI_Receive(flash->hxspi, data, HAL_XSPI_TIMEOUT_DEFAULT_VALUE);
}

// Flash data packet and abstraction

void flash_packet_build(const state_t *current_state, flash_packet_t *packet) { //TODO edit packet build
	packet->t = current_state->t;
	packet->sys_batt_v = current_state->sys_batt_v;
	packet->sys_batt_i = current_state->sys_batt_i;

	memcpy(packet->accel_b, current_state->accel_b, sizeof(packet->accel_b));
	memcpy(packet->omega_b, current_state->omega_b, sizeof(packet->omega_b));
	memcpy(packet->mag_b, current_state->mag_b, sizeof(packet->mag_b));
	memcpy(packet->quat, current_state->quat, sizeof(packet->quat));
	memcpy(packet->accel_e, current_state->accel_e, sizeof(packet->accel_e));

	packet->p_ground = current_state->p_ground;
	packet->alt_agl = current_state->alt_agl;
	packet->vel_z = current_state->vel_z;
}

uint8_t flash_check_erased(flash_t *flash) {
    uint32_t total_size = 4 * 1024 * 1024; // 32 M-bit = 4 MB
    uint8_t buffer[4096]; // Read each 4KB sector
    uint32_t address = 0;

    while (address < total_size) {
        flash_read_data(flash, address, buffer, sizeof(buffer)); // read 4KB

        for (uint16_t i = 0; i < sizeof(buffer); i++) { // check all are 0xFF
            if (buffer[i] != 0xFF) {
                return 0; // immediately return false if non erased byte found
            }
        }
        address += sizeof(buffer); // move to next block
    }

    return 1; // if we make it, entire chip is erased
}

void flash_packet_write(flash_t *flash, const state_t *state, DCACHE_HandleTypeDef *dcache) { //TODO will need to edit flash packets
    if (flash->full) return;

    if (state->is_launched) {
		flash->prescaler_max = 1; // 100 Hz in flight
	} else {
		flash->prescaler_max = 10; // 10 Hz on pad
	}

    uint8_t should_write = 0;

    flash->prescaler_cnt++;
    if (flash->prescaler_cnt >= flash->prescaler_max) {
    	should_write = 1;
    	flash->prescaler_cnt = 0;
    }

    if (should_write) {
        if ((flash->address % 256) + sizeof(flash_packet_t) > 256) { // if next packet will cross page boundary
        	flash->address = (flash->address & ~0xFF) + 256; // zero out lower 8 bits of address and move ahead one page
        									                 // effectively skipping the rest of that page
        }

		if (flash->address + sizeof(flash_packet_t) <= (16 * 1024 * 1024)) { // if address not at end of memory yet
			flash_packet_build(state, &(flash->dma_packet));
			HAL_DCACHE_CleanByAddr(dcache, (uint32_t*)&(flash->dma_packet), sizeof(flash_packet_t));
			flash_wait_for_ready(flash); // wait for prev write to complete. should already be done but best to be safe
			flash_write_page_dma(flash, flash->address, (uint8_t*)&(flash->dma_packet), sizeof(flash_packet_t));

			flash->address += sizeof(flash_packet_t);

		} else { // full
			flash->full = 1;
			printf("Flash memory full! Logging stopped.\r\n");
		}
    }
}

void flash_counters_reset(flash_t *flash) {
    flash->address = 0;
    flash->full = 0;
    flash->prescaler_cnt = 0;
    flash->prescaler_max = 10; // default to 10 Hz
}

void flash_scan_memory_map(flash_t *flash) {
    uint32_t addr = 0;
    uint8_t buf[256];
    uint8_t current_state = 0; // 0 = start, 1 = data, 2 = empty (0xFF)
    uint32_t block_start = 0;

    printf("--- FLASH MEMORY TOPOLOGY MAP ---\r\n");

    while (addr < (16 * 1024 * 1024)) {
        flash_read_data(flash, addr, buf, 256);

        // Check if the entire 256-byte page is empty (0xFF)
        uint8_t is_empty = 1;
        for (int i = 0; i < 256; i++) {
            if (buf[i] != 0xFF) {
                is_empty = 0;
                break;
            }
        }

        uint8_t new_state = is_empty ? 2 : 1;

        if (current_state == 0) {
            current_state = new_state;
            block_start = addr;
        } else if (current_state != new_state) {
            if (current_state == 1) {
                printf("DATA BLOCK : 0x%06lX to 0x%06lX (%lu bytes)\r\n", block_start, addr - 1, addr - block_start);
            } else {
                printf("EMPTY BLOCK: 0x%06lX to 0x%06lX (%lu bytes)\r\n", block_start, addr - 1, addr - block_start);
            }
            current_state = new_state;
            block_start = addr;
        }
        addr += 256;
    }

    // Print the final block reaching the end of the chip
    if (current_state == 1) {
        printf("DATA BLOCK : 0x%06lX to 0x%06lX\r\n", block_start, addr - 1);
    } else {
        printf("EMPTY BLOCK: 0x%06lX to 0x%06lX\r\n", block_start, addr - 1);
    }
    printf("--- SCAN COMPLETE ---\r\n");
}
