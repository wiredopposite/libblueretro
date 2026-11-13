/*
 * Copyright (c) 2019-2025, Jacques Gagnon
 * SPDX-License-Identifier: Apache-2.0
 */

// No error LED in the component: the host firmware owns its LEDs and pins.

#include "led.h"

void err_led_init(uint32_t package) { (void)package; }
void err_led_cfg_update(void) { }
void err_led_set(void) { }
void err_led_clear(void) { }
void err_led_pulse(void) { }
uint32_t err_led_get_pin(void) { return 0; }
