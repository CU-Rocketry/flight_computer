/*
 * state.h
 *
 *  Created on: Apr 14, 2026
 *      Author: Sigmond
 */

#ifndef INC_STATE_H_
#define INC_STATE_H_

#include <stdint.h>

#define GRAVITY 9.80665f

typedef struct {
	// Time
	uint32_t t; // [ms] since boot
	uint32_t launch_t; // [ms] launch detect
	uint32_t apogee_t;
	uint32_t land_t;
	uint32_t elapsed_t; // [ms] since launch detected

	// Launch detect
	uint32_t is_launched;
	uint32_t apogee_detect;
	uint32_t land_detect;

	// Power
	float sys_batt_v; // [V]
	float sys_batt_i; // [A]

    // Sensors
    float accel_ms2[3];
    float omega_rads[3];
    float mag_mgauss[3];
//    float pres_hpa;
    float pres_pa;

    // Body frame sensors
    float accel_b[3]; // [m/s/s] in body frame. *proper acceleration
    float omega_b[3]; // [rad/s] in body frame
    float mag_b[3]; // [mgauss] in body frame

    // State estimation
    float quat[4]; // body to inertial rotation already I think
    float accel_e[3]; // [m/s/s] in inertial frame

    float p_ground; // [Pa]
    float alt_agl; // [m] AGL with + up
    float vel_z; // [m] with + up

    uint8_t mode_override_en; // requests mode change (substitute for rotating selector)
    uint8_t mode_override; // mode to change to

    uint8_t launch_detect_en; // set to 1 to trigger launch detect

} state_t;

extern state_t global_state; // global instance

#endif /* INC_STATE_H_ */
