/*
 * sensors.c
 *
 *  Created on: Apr 9, 2026
 *      Author: hmeag
 */

// includes


#include "sensors.h"
#include "main.h"

#include <string.h>
#include <stdio.h>

#include "lps22hh_reg.h"

// SPI handles
extern SPI_HandleTypeDef hspi2; // Baro and IMU
extern SPI_HandleTypeDef hspi3; // Magnetometer

// ST drivers
stmdev_ctx_t lps22hh_ctx;

// setup chip select pins
void spi_nss(SPI_HandleTypeDef handle, uint8_t level) {
	if (handle.Instance == hspi2) {
		HAL_GPIO_WritePin(BARO_CS_GPIO_Port, BARO_CS_Pin, level); //write pin to set level
	}
}

// ST driver platform functions
int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len)
{
	HAL_StatusTypeDef status = HAL_OK;

	spi_nss(handle, 0);
	status += HAL_SPI_Transmit(handle, &reg, 1, 1000);
	status += HAL_SPI_Transmit(handle, bufp, len, 1000);
	spi_nss(handle, 1);
	return status;
}

int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
	reg |= 0x80; // set MSB for read

	HAL_StatusTypeDef status = HAL_OK;
	status += HAL_SPI_Transmit(handle, &reg, 1, 1000);
	status += HAL_SPI_Receive(handle, bufp, len, 1000);
	return status;
}

void platform_delay(uint32_t millisec)
{
	HAL_Delay(millisec);
}

void baro_init() {
	// Setup baro driver device context
	lps22hh_ctx.write_reg = platform_write;
	lps22hh_ctx.read_reg = platform_read;
	lps22hh_ctx.mdelay = platform_delay;
	lps22hh_ctx.handle = &hspi2;

//	lps22hh_pin_int_route_t int_route;
	lps22hh_id_t id;



	lps22hh_device_id_get(&lps22hh_ctx, &id);
	if (id.whoami != LPS22HH_ID) {
		printf("LPS22HH whoami failed: %u, expected %u\r\n", id.whoami, LPS22HH_ID);
		while (1);
	}

//	lps22hh_init_set(&lps22hh_ctx, LPS22HH_RESET);
//	do {
//		lps22hh_status_get(&lps22hh_ctx, &status);
//	} while (status.sw_reset);
//	printf("LPS22 reset success\r\n");
//
//	bus_mode.filter = LPS22HH_FILTER_AUTO;
//	bus_mode.interface = LPS22HH_SPI_4W;
//	lps22hh_bus_mode_set(&lps22hh_ctx, &bus_mode);
//
//	md.odr = LPS22DF_100Hz;
//	md.avg = LPS22DF_64_AVG;
//	md.lpf = LPS22DF_LPF_ODR_DIV_9;
//	lps22hh_mode_set(&lps22hh_ctx, &md);
//
//	// Enable DRDY routed on INT pin
//	lps22hh_pin_int_route_get(&lps22hh_ctx, &int_route);
//	int_route.drdy_pres = PROPERTY_ENABLE;
//	lps22hh_pin_int_route_set(&lps22hh_ctx, &int_route);
}
