/*
 * sensors.h
 *
 *  Created on: Apr 9, 2026
 *      Author: hmeag
 */

#ifndef INC_SENSORS_H_
#define INC_SENSORS_H_

void get_accel_ms2(float *out);
void get_omega_rads(float *out);
void get_mag_mgauss(float *out);
void get_pres_hpa(float *out);

void baro_int_drdy_handler(void);
void imu_int_drdy_handler(void);
void mag_int_drdy_handler(void);

void baro_spi_callback(void);
void imu_spi_callback(void);
void mag_spi_callback(void);

void sensors_init(void);
void baro_init(void);
void imu_init(void);
void mag_init(void);

#endif /* SRC_SENSORS_H_ */

