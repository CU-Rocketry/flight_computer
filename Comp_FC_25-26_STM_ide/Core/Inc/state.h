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
	uint32_t elapsed_t; // [ms] since launch detected

	// Launch detect
	uint32_t is_launched;

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

    // Control
    // TODO
    float output; // 0 to 1 mapping to air brakes deployment range

    // Servo
    float servo_cmd; // [deg]
    float servo_fdbk; // [deg]

    uint8_t use_hil_data;

	uint8_t mode_override_en;
	uint8_t mode_override;

	uint8_t servo_cmd_en;
	float servo_cmd_override; // [deg]
} state_t;

extern state_t global_state; // global instance

#endif /* INC_STATE_H_ */
