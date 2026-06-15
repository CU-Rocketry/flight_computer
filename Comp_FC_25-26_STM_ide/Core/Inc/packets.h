/*
 * packets.h
 *
 *  Created on: May 29, 2026
 *      Author: haileymeagher
 */

#ifndef INC_PACKETS_H_
#define INC_PACKETS_H_

#include "sx126x.h" //includ for rf packet params this may break things

//types of packets needed
//telemetry, command, flash

// Telemetry packet types
typedef enum {
	PKT_TYPE_TELEMETRY = 0x01, // telemetry data stream
	PKT_TYPE_LOG = 0x02,
	PKT_TYPE_CMD = 0x03, // manual/override commands from control panel software
	PKT_TYPE_RADIO_CONFIG = 0x04,

} packet_type_t;

//rf device type
typedef enum {
    FLIGHT_COMPUTER = 0x01,
	GROUND_STATION = 0x02,
} rf_device_t;

#pragma pack(push, 1) // so that there is no padding

// Telemetry
typedef struct {
	uint8_t pkt_type; // always 0x01 for telemetry 1

	// Time
	uint32_t t; // [ms] since boot 4

	// Launch detect
	uint8_t state; //will display if rocket is pre-launch, ascent, descent, or landed 1

	// Power
	float sys_batt_v; // [V] 4

    uint16_t alt_agl; // [m] AGL with + up 2
    float vel_z; // [m] with + up 4

    //gps location
    uint32_t gps_lat; // lat 4
	uint32_t gps_long; // long 4

	int8_t rssi; //1

} telemetry_packet_t;

typedef struct {
    uint8_t pkt_type; // always 0x02 for log
    uint8_t lvl; // log_lvl_t 0 to 3 for debug, info, warning, error
    char message[126]; // there's extra space even in just the first 254 bytes then
} log_packet_t;

// Command
typedef struct {

		uint8_t pkt_type; // always 0x03 for cmd

	    uint8_t mode_en; // requests mode change (substitute for rotating selector)
	    uint8_t mode; // mode to change to

	    uint8_t launch_detect_en; // set to 1 to trigger launch detect


} command_packet_t;

typedef struct {
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
    float gps_lat;
    float gps_long;

    uint32_t state;

	uint32_t launch_t; // [ms] launch detect
	uint32_t apogee_t;
	uint32_t land_t;
	uint32_t elapsed_t; // [ms] since launch detected

	// Launch detect
	uint32_t is_launched;
	uint32_t apogee_detect;
	uint32_t land_detect;

} flash_packet_t;


// TODO make function to enable radio config switch in idle mode if radio config is enabled

typedef struct {

	uint8_t pkt_type; //0x04 for radio packet

	uint8_t reconfig;
	uint8_t device; //DEVICE IS THE DEVICE YOU ARE TRYING TO TARGET

	uint8_t pkt_change_en;
	uint8_t rf_pkt_type;

	// lora params for the mod params struct	//TODO if want to configure right, will need to check what type of packet we are trying to config... later
	uint8_t lora_params_en;

    sx126x_lora_sf_t sf;    //!< LoRa Spreading Factor
    sx126x_lora_bw_t bw;    //!< LoRa Bandwidth
    sx126x_lora_cr_t cr;    //!< LoRa Coding Rate

	uint8_t tx_params_en;
	uint8_t pwr;

    uint8_t freq_en;
	uint32_t freq;


} radio_config_packet_t; //this can be more fleshed out but is it for now

extern radio_config_packet_t current_radio_config;
extern radio_config_packet_t update_radio_config;

extern uint8_t radio_reconfig_flag;

#endif /* INC_PACKETS_H_ */
