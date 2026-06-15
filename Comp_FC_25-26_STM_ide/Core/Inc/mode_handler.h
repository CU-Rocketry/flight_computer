/*
 * mode_handler.h
 *
 *  Created on: Jun 14, 2026
 *      Author: haileymeagher
 */

#ifndef INC_MODE_HANDLER_H_
#define INC_MODE_HANDLER_H_

//meant to define the different modes of operation of the system

// Mode selection
enum Mode {

    //
    STARTUP = -1,
    //default mode
	IDLE = 0,
    LAUNCH_DETECT = 1,
    ASCENT = 2,
    DESCENT = 3,
    LANDED = 4,

    //Test Modes (define more here as needed)
    TEST_SENSORS = 5,
    TEST_LORA = 6,
    TEST_TELEMETRY = 7,
    TEST_UI = 8,
    TEST_FLASH = 9,
    TEST_GPS = 10,
    //
};

// do the things based on the current mode

//for ground station, update LEDS and Buzzer for each mode of the flight computer
void mode_current_handler(uint8_t mode);
void mode_transition_handler(uint8_t mode_prev, uint8_t mode_current);
void determine_rocket_state(state_t current_state);


#endif /* INC_MODE_HANDLER_H_ */
