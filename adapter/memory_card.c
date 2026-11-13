/*
 * Copyright (c) 2021-2025, Jacques Gagnon
 * SPDX-License-Identifier: Apache-2.0
 */

// No memory card emulation in the component: an output that wants one has its own.
// The BLE config interface still asks; it reads zeros.

#include <string.h>

#include "memory_card.h"

int32_t mc_init(void) { return -1; }
int32_t mc_init_mem(void) { return 0; }
void mc_storage_update(void) { }
void mc_read(uint32_t addr, uint8_t *data, uint32_t size) { (void)addr; memset(data, 0, size); }
void mc_write(uint32_t addr, uint8_t *data, uint32_t size) { (void)addr; (void)data; (void)size; }
uint8_t *mc_get_ptr(uint32_t addr) { (void)addr; return NULL; }
uint32_t mc_get_state(void) { return 0; }
bool mc_get_ready(void) { return false; }
