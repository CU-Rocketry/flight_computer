/*
 * rgb_led.h
 *
 *  Created on: Apr 11, 2026
 *      Author: haileymeagher
 */

#ifndef INC_RGB_LED_H_
#define INC_RGB_LED_H_

typedef struct {
	TIM_HandleTypeDef* handle;
	uint32_t channel_r;
	uint32_t channel_g;
	uint32_t channel_b;
} rgb_led_t;

void rgb_led_set(rgb_led_t* led, uint32_t color) {
	// color should be in lower 24 bits
	uint8_t r = (color >> 16) & 0xFF;
	uint8_t g = (color >> 8) & 0xFF;
	uint8_t b = color & 0xFF;

	uint32_t ccr_r = 255 - r;
	uint32_t ccr_g = 255 - g;
	uint32_t ccr_b = 255 - b;

	// prevent 1 tick of on time
	if (ccr_r == 255) ccr_r = 256;
	if (ccr_g == 255) ccr_g = 256;
	if (ccr_b == 255) ccr_b = 256;

	// update duty cycles
	__HAL_TIM_SET_COMPARE(led->handle, led->channel_r, ccr_r);
	__HAL_TIM_SET_COMPARE(led->handle, led->channel_g, ccr_g);
	__HAL_TIM_SET_COMPARE(led->handle, led->channel_b, ccr_b);
}

void rgb_led_init(rgb_led_t* led) {
	rgb_led_set(led, 0x000000); // turn channels off

	// start PWM
	HAL_TIM_PWM_Start(led->handle, led->channel_r);
	HAL_TIM_PWM_Start(led->handle, led->channel_g);
	HAL_TIM_PWM_Start(led->handle, led->channel_b);
}


#endif /* INC_RGB_LED_H_ */
