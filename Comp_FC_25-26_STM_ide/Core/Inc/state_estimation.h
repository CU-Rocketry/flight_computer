/*
 * state_estimation.h
 *
 *  Created on: Mar 27, 2026
 *      Author: Sigmond
 */

#ifndef INC_STATE_ESTIMATION_H_
#define INC_STATE_ESTIMATION_H_

#include "arm_math.h"

void state_estimation(float dt);
//arm_status get_imu_b(arm_matrix_instance_f32 *out_accel, arm_matrix_instance_f32 *out_omega);
//arm_status get_mag_b(arm_matrix_instance_f32 *out_mag);

void get_imu_b(float out_accel_b[3], float out_omega_b[3]);
void get_mag_b(float out_mag_b[3]);

void quat_conj(float q[4], float q_star[4]);
void quat_rot(float v[3], float q[4], float v_out[3]);

void kalman_predict(float accel_z, float dt);
void kalman_update(float pressure);

void launch_detect(float accel_b_x);
void launch_detect_override(uint8_t state);

#endif /* INC_STATE_ESTIMATION_H_ */
