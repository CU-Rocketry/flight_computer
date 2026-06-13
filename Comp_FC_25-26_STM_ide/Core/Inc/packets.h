/*
 * packets.h
 *
 *  Created on: May 29, 2026
 *      Author: haileymeagher
 */

#ifndef INC_PACKETS_H_
#define INC_PACKETS_H_

//types of packets needed
//telemetry, command, flash


// Telemetry packet types
typedef enum {
	PKT_TYPE_TELEMETRY = 0x01, // telemetry data stream
	PKT_TYPE_CMD = 0x03, // manual/override commands from control panel software
	PKT_TYPE_RADIO_CONFIG = 0x04,

} packet_type_t;

#pragma pack(push, 1) // so that there is no padding

// Telemetry
typedef struct {
	uint8_t pkt_type; // always 0x01 for telemetry

	// Time
	uint32_t t; // [ms] since boot

	// Launch detect
	uint32_t is_launched;
	uint32_t state;

	// Power
	float sys_batt_v; // [V]
	float sys_batt_i; // [A]

    // Sensors
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

    //gps location
    float gps_loc[3]; // lat, long, height

} telemetry_packet_t;



// Flash
// for 2 packets per 256 byte page we have max 128 bytes = 32 floats
typedef struct {

	uint8_t pkt_type; //flash 0x02 for flash
	uint32_t t; // [ms] since boot 4 bytes
	float sys_batt_v; // [V] 8
	float sys_batt_i; // [A] 12
	float accel_b[3]; // [m/s/s] in body frame. *proper acceleration 24
	float omega_b[3]; // [rad/s] in body frame 36
	float mag_b[3]; // [mgauss] in body frame 48
    float quat[4]; // body to inertial rotation already I think 64
    float accel_e[3]; // [m/s/s] in inertial frame 76
    float p_ground; // [Pa] 80
    float alt_agl; // [m] AGL with + up 84
    float vel_z; // [m] with + up 88
    float gps_loc[3]; //lat, long, height

} flash_packet_t;

// Command
typedef struct {

		uint8_t pkt_type; // always 0x03 for cmd

	    uint8_t mode_en; // requests mode change (substitute for rotating selector)
	    uint8_t mode; // mode to change to

	    uint8_t launch_detect_en; // set to 1 to trigger launch detect


} command_packet_t;


// TODO make function to enable radio config switch in idle mode if radio config is enabled

typedef struct {

	uint8_t pkt_type; //0x04 for radio packet

	uint8_t reconfig;

	uint8_t pkt_change_en;
	uint8_t rf_pkt_type;

	// lora params for the mod params struct	//TODO if want to configure right, will need to check what type of packet we are trying to config... later
	uint8_t lora_params_en;

	uint8_t sf;
	uint8_t bw;
	uint8_t cr;

	uint8_t tx_params_en;
	uint8_t pwr;

    uint8_t freq_en;
	uint32_t freq;


} radio_config_packet_t; //this can be more fleshed out but is it for now

#endif /* INC_PACKETS_H_ */
