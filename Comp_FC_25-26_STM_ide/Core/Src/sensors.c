/*
 * sensors.c
 *
 *  Created on: Apr 9, 2026
 *      Author: hmeag
 */

// includes
#include "sensors.h"
#include "main.h"
#include "lps22hh_reg.h"

#include <string.h>
#include <stdio.h>

// SPI handles
extern SPI_HandleTypeDef hspi2; // IMU and Baro
extern SPI_HandleTypeDef hspi3; // Magnetometer

// ST drivers
stmdev_ctx_t lps22hh_ctx;
stmdev_ctx_t lsm6dsv80x_ctx;
stmdev_ctx_t iis2mdc_ctx;

// Sensor readings
uint32_t pres_raw;
float pres_hpa;

int16_t accel_raw[3];
int16_t omega_raw[3];
float accel_ms2[3];
float omega_rads[3];

int16_t mag_raw[3];
float mag_mgauss[3];

// SPI DMA buffers
// Size = 1 for register + N bytes
uint8_t baro_tx_buf[4], baro_rx_buf[4]; // XL, L, and H
uint8_t imu_tx_buf[13], imu_rx_buf[13]; // Gyro 2 bytes * 3 axes + Accel 2 bytes * 3 axes
uint8_t mag_tx_buf[7], mag_rx_buf[7]; // X, Y, Z where each channel is 16 bits = 2 bytes

void get_accel_ms2(float *out) {
	out[0] = accel_ms2[0];
	out[1] = accel_ms2[1];
	out[2] = accel_ms2[2];
}

void get_omega_rads(float *out) {
	out[0] = omega_rads[0];
	out[1] = omega_rads[1];
	out[2] = omega_rads[2];
}

void get_mag_mgauss(float *out) {
	out[0] = mag_mgauss[0];
	out[1] = mag_mgauss[1];
	out[2] = mag_mgauss[2];
}

void get_pres_hpa(float *out) {
	*out = pres_hpa;
}

void spi_nss(SPI_HandleTypeDef *handle, uint8_t level) {
	if (handle->Instance == SPI2) {
		HAL_GPIO_WritePin(BARO_CS_GPIO_Port, BARO_CS_Pin, level);
		HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, level);
	} else if (handle->Instance == SPI3) {
		HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, level);
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

// DRDY handlers
// when data ready, init the DMA SPI transmission
void baro_int_drdy_handler()
{
	// initialize the SPI transmission
	memset(baro_tx_buf, 0, 4);
	baro_tx_buf[0] = LPS22HH_PRESS_OUT_XL | 0x80; // with MSB set for read
	HAL_SPI_TransmitReceive_DMA(lps22hh_ctx.handle, baro_tx_buf, baro_rx_buf, 4);
}

//void imu_int_drdy_handler() {
//	memset(imu_tx_buf, 0, 13);
//	imu_tx_buf[0] = LSM6DSV80X_OUTX_L_G | 0x80;
//	HAL_SPI_TransmitReceive_DMA(lsm6dsv80x_ctx.handle, imu_tx_buf, imu_rx_buf, 13);
//}
//
//void mag_int_drdy_handler() {
//	memset(mag_tx_buf, 0, 7);
//	mag_tx_buf[0] = IIS2MDC_OUTX_L_REG | 0x80;
//	HAL_SPI_TransmitReceive_DMA(iis2mdc_ctx.handle, mag_tx_buf, mag_rx_buf, 7);
//}

// SPI DMA done callbacks
// after data transfer process into units
void baro_spi_callback() {
	pres_raw = (uint32_t)(((uint32_t)baro_rx_buf[3] << 24) | ((uint32_t)baro_rx_buf[2] << 16) | ((uint32_t)baro_rx_buf[1] << 8));
	pres_hpa = lps22hh_from_lsb_to_hpa(pres_raw);
}

void imu_spi_callback() {

}


void mag_spi_callback() {

}

void sensors_init() {
	baro_init();
//	imu_init();
//	mag_init();


}

void baro_init() {
	// Setup baro driver device context
	lps22hh_ctx.write_reg = platform_write;
	lps22hh_ctx.read_reg = platform_read;
	lps22hh_ctx.mdelay = platform_delay;
	lps22hh_ctx.handle = &hspi2;

	lps22hh_pin_int_route_t int_route;
	lps22hh_sim_t md_spi;
	lps22hh_lpfp_cfg_t md_lpf;
	lps22hh_odr_t md_odr;

	uint8_t id;


	lps22hh_device_id_get(&lps22hh_ctx, &id);
	if (id != LPS22HH_ID) {
		printf("LPS22HH whoami failed: %u, expected %u\r\n", id, LPS22HH_ID);
		while (1);
	}

	// restore default config
	    lps22hh_reset_set(&lps22hh_ctx, 0);
	    uint8_t rst = 0x1;
	    while (rst)
	    {
	      lps22hh_reset_get(&lps22hh_ctx, &rst);
	    }
	    printf("LPS22 reset success\r\n");

	md_odr = LPS22HH_100_Hz;
	md_lpf = LPS22HH_LPF_ODR_DIV_9;

	lps22hh_spi_mode_set(&lps22hh_ctx, md_spi);
	lps22hh_lp_bandwidth_set(&lps22hh_ctx, md_lpf);
	lps22hh_data_rate_set(&lps22hh_ctx, md_odr);



	// Enable DRDY routed on INT pin
	lps22hh_pin_int_route_get(&lps22hh_ctx, &int_route);
	int_route.drdy_pres = PROPERTY_ENABLE;
	lps22hh_pin_int_route_set(&lps22hh_ctx, int_route);
}

void imu_init() {
	// Setup IMU driver device context
	lsm6dsv80x_ctx.write_reg = platform_write;
	lsm6dsv80x_ctx.read_reg = platform_read;
	lsm6dsv80x_ctx.mdelay = platform_delay;
	lsm6dsv80x_ctx.handle = &hspi2;
}

void mag_init() {
	// Setup magnetometer device driver context
	iis2mdc_ctx.write_reg = platform_write;
	iis2mdc_ctx.read_reg = platform_read;
	iis2mdc_ctx.mdelay = platform_delay;
	iis2mdc_ctx.handle = &hspi3;
}

