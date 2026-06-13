/*
 * state_estimation.c
 *
 *  Created on: Mar 27, 2026
 *      Author: Sigmond
 */


#include "sensors.h"
#include "state_estimation.h"
#include "MadgwickAHRS.h"
#include <stdio.h>
#include "state.h"
#include "math.h"
#include "stm32h5xx_hal.h"

uint16_t last_sample_time;
uint16_t sample_time;
float dt;

extern uint8_t imu_ready;
extern uint8_t mag_ready;
extern uint8_t baro_ready;

float roll, pitch, yaw;

// ISA model
#define ISA_T0 288.15    // [K] sea level temp
#define ISA_L 0.0065     // [K/m] lapse rate
#define ISA_P0 101325.0  // [Pa] sea level pressure TODO: pressure unit conversion if needed
#define ISA_R 287.05   // gas constant

// Kalman state
typedef struct {
    float q[2]; // q = [alt_agl, vel_z]T
    float P[2][2]; // Covariance matrix
    float P_ground; // Ground pressure
} kalman_state_t;

static kalman_state_t kalman_state = {
    .q = {0.0f, 0.0f},
    .P = {{2.0f, 0.0f}, {0.0f, 0.5f}},
    .P_ground = 101325.0f // To be overwritten with average
};

// Kalman tuning
static const float sigma_acc = 0.5f; // IMU trust
static const float R_baro = 4.0f; // Baro trust (SD in meters, squared)

void state_estimation(float dt) {

//	float accel_b_data[3];
//	float omega_b_data[3];
//	arm_matrix_instance_f32 accel_b, omega_b;
//	arm_mat_init_f32(&accel_b, 3, 1, accel_b_data);
//	arm_mat_init_f32(&omega_b, 3, 1, omega_b_data);

	get_imu_b(global_state.accel_b, global_state.omega_b);

	launch_detect(global_state.accel_b[0]);

//	float mag_b_data[3];
//	arm_matrix_instance_f32 mag_b;
//	arm_mat_init_f32(&mag_b, 3, 1, mag_b_data);
//	float mag_b[3] = {0,0,0};
	get_mag_b(global_state.mag_b);
	mag_ready = 0; // disable magnetometer

	// TODO
//	if (sample_time < last_sample_time) {
//		dt = (0xFFFF - last_sample_time + sample_time); // [us]
//	} else {
//		dt = (sample_time - last_sample_time); // [us]
//	}
//	dt *= 0.000001f;
	// dt = 0.002f; // 500 Hz, hardcoded for now


	if (imu_ready)
	{
		imu_ready = 0;
		if (mag_ready) {
			mag_ready = 0;
			MadgwickAHRSupdate(global_state.omega_b[0], global_state.omega_b[1], global_state.omega_b[2],
					-global_state.accel_b[0], -global_state.accel_b[1], -global_state.accel_b[2],
					global_state.mag_b[0], global_state.mag_b[1], global_state.mag_b[2],
					dt);

		} else {
			MadgwickAHRSupdateIMU(global_state.omega_b[0], global_state.omega_b[1], global_state.omega_b[2],
								 -global_state.accel_b[0], -global_state.accel_b[1], -global_state.accel_b[2],
								 dt);
		}

		MadgwickQuaternionGet(global_state.quat); // output madgwick filter quaternion to state

		// Seemingly the madgwick filter already outputs the body to earth rotation
//		float q_star[4];
//		quat_conj(state.quat, q_star); // get conjugate of madgwick output
		// now q_star is the body to earth rotation

		quat_rot(global_state.accel_b, global_state.quat, global_state.accel_e); // rotate body accel by b to e rotation to get inertial acceleration

		global_state.accel_e[2] += GRAVITY; // get rid of gravity. TODO a constant maybe

		kalman_predict(global_state.accel_e[2], dt); // kalman prediction
	}

	if (baro_ready) {
		baro_ready = 0;
		kalman_update(global_state.pres_pa); // Kalman correction
	}

	// negatives to convert back from NED
	global_state.alt_agl = -kalman_state.q[0];
	global_state.vel_z = -kalman_state.q[1];
}

void get_imu_b(float out_accel_b[3], float out_omega_b[3]) {
	out_accel_b[0] = global_state.accel_ms2[2]; // body +X is now sensor +Z
	out_accel_b[1] = -global_state.accel_ms2[1]; // body +Y is now sensor -Y
	out_accel_b[2] = global_state.accel_ms2[0]; // body +Z is now sensor +X

	out_omega_b[0] = global_state.omega_rads[2]; // body +X is now sensor +Z
	out_omega_b[1] = -global_state.omega_rads[1]; // body +Y is now sensor -Y
	out_omega_b[2] = global_state.omega_rads[0]; // body +Z is now sensor +X
}

void get_mag_b(float out_mag_b[3]) {
	out_mag_b[0] = global_state.mag_mgauss[2]; // body +X is now sensor +Z
	out_mag_b[1] = -global_state.mag_mgauss[1]; // body +Y is now sensor -Y
	out_mag_b[2] = -global_state.mag_mgauss[0]; // body +Z is now sensor +X
}

//indexing starts at zero
//u cross v


// q* = [q0, -q1, -q2, -q3] at least in the representation we have
void quat_conj(float q[4], float q_star[4])
{
	q_star[0] =  q[0];
	q_star[1] = -q[1];
	q_star[2] = -q[2];
	q_star[3] = -q[3];
}

// rotates a vector by quaternion
// see https://danceswithcode.net/engineeringnotes/quaternions/quaternions.html
void quat_rot(float v[3], float q[4], float v_out[3]) {
	v_out[0] = v[0] * (1.0f - 2.0f * (q[2]*q[2] + q[3]*q[3])) +
			   v[1] * (2.0f * (q[1]*q[2] - q[0]*q[3])) +
			   v[2] * (2.0f * (q[1]*q[3] + q[0]*q[2]));

	v_out[1] = v[0] * (2.0f * (q[1]*q[2] + q[0]*q[3])) +
			   v[1] * (1.0f - 2.0f * (q[1]*q[1] + q[3]*q[3])) +
			   v[2] * (2.0f * (q[2]*q[3] - q[0]*q[1]));

	v_out[2] = v[0] * (2.0f * (q[1]*q[3] - q[0]*q[2])) +
			   v[1] * (2.0f * (q[2]*q[3] + q[0]*q[1])) +
			   v[2] * (1.0f - 2.0f * (q[1]*q[1] + q[2]*q[2]));
}



//initial conditions

//start timer once the G-force on the rocket is 5G or greater

//If function runs 5 times - then it's been 1/100th of a second
//Connected to 500 Hz frequency

void launch_detect(float accel_b_x) {
	if (global_state.is_launched) { // if already launched don't keep checking bc you can't unlaunch
		return;
	}

	static uint8_t trig_cnt = 0; // how many samples greater than threshold, static so it stays between function calls

	const float LAUNCH_THRESH_MS2 = 5.0f * GRAVITY;
	const uint8_t MIN_TRIG_CNT = 10; // 10 samples at 500Hz = 20ms

	if (accel_b_x >= LAUNCH_THRESH_MS2) { // check if body X accel exceeds threshold
		trig_cnt++;

		if (trig_cnt >= MIN_TRIG_CNT) { // if acceleration above threshold for consecutive samples
			global_state.is_launched = 1;
			global_state.launch_t = HAL_GetTick();
		}

	} else { // accel below thresh so counter reset
		trig_cnt = 0;
	}
}



void launch_detect_override(uint8_t is_launched) {
	global_state.is_launched = (is_launched > 0) ? 1 : 0; // force launch detect to 1 or 0
}

void kalman_predict(float accel_z, float dt) {
    if (!global_state.is_launched) { // if not launched, prevent drift
        kalman_state.q[0] = 0.0f;
        kalman_state.q[1] = 0.0f;
        kalman_state.P[0][0] = 2.0f; kalman_state.P[0][1] = 0.0f;
        kalman_state.P[1][0] = 0.0f; kalman_state.P[1][1] = 0.5f;
        return;
    }

    // prediction for q = F*x + B*u
    // position += velocity*dt + 0.5*accel*dt^2
    // velocity += accel*dt
    kalman_state.q[0] = kalman_state.q[0] + kalman_state.q[1] * dt + 0.5f * accel_z * dt * dt;
    kalman_state.q[1] = kalman_state.q[1] + accel_z * dt;

    // prediction for covariance P = F*P*F' + Q
    // Q = Q00 Q01
    //     Q10 Q11
    // which is unconventional but matches zero indexed arrays
    float Q00 = (dt * dt * dt * dt / 4.0f) * (sigma_acc * sigma_acc);
    float Q01 = (dt * dt * dt / 2.0f) * (sigma_acc * sigma_acc);
    float Q10 = Q01;
    float Q11 = (dt * dt) * (sigma_acc * sigma_acc);

    // P = P00 P01
    //     P10 P11
    float P00 = kalman_state.P[0][0];
    float P01 = kalman_state.P[0][1];
    float P10 = kalman_state.P[1][0];
    float P11 = kalman_state.P[1][1];

    kalman_state.P[0][0] = P00 + dt * P10 + dt * (P01 + dt * P11) + Q00;
    kalman_state.P[0][1] = P01 + dt * P11 + Q01;
    kalman_state.P[1][0] = P10 + dt * P11 + Q10;
    kalman_state.P[1][1] = P11 + Q11;
}

void kalman_update(float pressure) {
    if (!global_state.is_launched) { // before launch update ground pressure
        kalman_state.P_ground = 0.99f * kalman_state.P_ground + 0.01f * pressure; // low pass filter (LPF)
        global_state.p_ground = kalman_state.P_ground;
        return;
    }

    // use ISA model to convert to a height
    float exponent = (ISA_R * ISA_L) / GRAVITY;
    float h_agl_pres = (ISA_T0 / ISA_L) * (1.0f - powf((pressure / kalman_state.P_ground), exponent));
    float z_meas = -h_agl_pres; // NED coordinate system

    // Innovation (residual) y = z_meas - H*q
    // H = [1, 0], so H*q is just q[0]
    float y = z_meas - kalman_state.q[0];

    // Innovation covariance S = H*P*H' + R
    float S = kalman_state.P[0][0] + R_baro;

    // Kalman gain K = P*H' / S
    float K0 = kalman_state.P[0][0] / S;
    float K1 = kalman_state.P[1][0] / S;

    // state update x = x + K*y
    kalman_state.q[0] = kalman_state.q[0] + K0 * y;
    kalman_state.q[1] = kalman_state.q[1] + K1 * y;

    // covariance update P = (I - K*H)*P
    float P00 = kalman_state.P[0][0];
    float P01 = kalman_state.P[0][1];

    kalman_state.P[0][0] = P00 - K0 * P00;
    kalman_state.P[0][1] = P01 - K0 * P01;
    kalman_state.P[1][0] = kalman_state.P[1][0] - K1 * P00;
    kalman_state.P[1][1] = kalman_state.P[1][1] - K1 * P01;
}

void apogee_detect(float vel_z){

	if (global_state.apogee_detect) { // if already launched don't keep checking bc we only reach apogee once
				return;
			}

	static uint8_t trig_cnt = 0; // how many samples greater than threshold, static so it stays between function calls

		const float APOGEE_THRESH_MAX_MS = 1;
		const float APOGEE_THRESH_MIN_MS = -1;
		const uint8_t MIN_TRIG_CNT = 20; // 10 samples at 500Hz = 250ms

		if (vel_z >= APOGEE_THRESH_MIN_MS && vel_z <= APOGEE_THRESH_MAX_MS) { // check if body velocity is within the bounds

			if (trig_cnt >= MIN_TRIG_CNT) { // if acceleration above threshold for consecutive samples
				global_state.apogee_detect = 1;
				global_state.apogee_t = HAL_GetTick();
			}

		} else { // vel not in thresh so counter reset
			trig_cnt = 0;
		}

}

void land_detect(float vel_z, float accel_b_x){

	if (global_state.land_detect) { // if already launched don't keep checking bc we only take off once
			return;
		}

	static uint8_t trig_cnt = 0; // how many samples greater than threshold, static so it stays between function calls

		const float LAND_THRESH_MAX_MS = 0.5;
		const float LAND_THRESH_MIN_MS = -0.5;
		const float LAND_THRESH_MAX_MS2 = 0.5;
		const float LAND_THRESH_MIN_MS2 = -0.5;
		const uint8_t MIN_TRIG_CNT = 5000; // 5000 samples at 500Hz = 10 seconds

		if (vel_z >= LAND_THRESH_MIN_MS && vel_z <= LAND_THRESH_MAX_MS) { // check if body velocity is within the bounds

			if(accel_b_x >= LAND_THRESH_MIN_MS2 && accel_b_x <= LAND_THRESH_MAX_MS2 )

			if (trig_cnt >= MIN_TRIG_CNT) { // if acceleration above threshold for consecutive samples
				global_state.apogee_detect = 1;
				global_state.apogee_t = HAL_GetTick();
			}

		} else { //  not in thresh so counter reset
			trig_cnt = 0;
		}

}
//HAL_GetTick is uint32_t data type

//if (accel>=launch_accel_ms2)
//if (start_time==0.0f)


//old stuff below



//void cross_prod(arm_matrix_instance_f32 *a,arm_matrix_instance_f32 *b, arm_matrix_instance_f32 *out)
//{
//	float a_data[3]=a->pData;
//	float b_data[3]=b->pData;
//	float out_data[3];
//	out_data[0]=(a_data[1]*b_data[2]-b_data[1]*a_data[2]);
//	out_data[1]=-(a_data[0]*b_data[2]-b_data[0]*a_data[2]);
//	out_data[2]=a_data[0]*b_data[1]-b_data[0]*a_data[1];
//
//	arm_matrix_instance_f32 out_mat;
//
//		arm_mat_init_f32(*out_mat,3,1,out_data);
//		out=out_mat;
//		//cross product
//}
//s=pData[0];
//pData=[scalar,i,j,k]
//scalar=s
//vector being rotated=pData[1],pData[2],pData[3];


//v= i,j,k components of original vector

//r=vector components of quaternion we're rotating about
//q=quaternion=[0,r]


//s=scalar


//v=3 dim vector

//v=pData[1],pData[2],pData[3];

//unit length quaternion:
//q=(r,s);




//q[0]=0,pData[1],pData[2],pData[3];
//unit quaternion is normalized/divided by its length
//magq=arm_sqrt_f32(q[0]);
//q=(1/magq)*q[0];




//components)*quaternion vector components+(s^2-quat vector dotted w/ quat vector)*original vector components)
//+2s(quat vector comps cross original vector comps)


//rot_vector=arm_mult_f32(2(arm_dot_prod_f32(u,v)))+arm_mult_f32(s*s-arm_dot_prod_f32(q,q),v)+2*s*cross_prod(u,v);

