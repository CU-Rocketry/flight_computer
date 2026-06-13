/*
 * btn.h
 *
 *  Created on: May 9, 2026
 *      Author: Sigmond
 */

#ifndef INC_BTN_H_
#define INC_BTN_H_

#include "stm32h5xx_hal.h"
#include <stdint.h>

typedef struct {
	GPIO_TypeDef* port;
	uint16_t pin;
	uint8_t current;
	uint8_t prev;
} btn_t;

void btn_init(btn_t *btn, GPIO_TypeDef* port, uint16_t pin) {
	btn->port = port;
	btn->pin = pin;
	btn->current = (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_SET) ? 1 : 0;
	btn->prev = btn->current;
}

// Updates button driver internal state
// to be called in 100Hz loop
void btn_update(btn_t *btn) {
	btn->prev = btn->current;
	btn->current = (HAL_GPIO_ReadPin(btn->port, btn->pin) == GPIO_PIN_SET) ? 1 : 0;
}

// Returns 1 if the button is currently pressed
uint8_t btn_get(btn_t *btn) {
	return btn->current == 1;
}

// Returns 1 when just pressed, -1 when just released, and 0 when no change
int8_t btn_get_edge(btn_t *btn) {
	return btn->current - btn->prev;
}

#endif /* INC_BTN_H_ */
