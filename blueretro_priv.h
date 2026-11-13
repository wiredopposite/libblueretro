/*
 * Copyright (c) 2019-2025, Jacques Gagnon
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLUERETRO_PRIV_H_
#define _BLUERETRO_PRIV_H_

#include <stdint.h>
#include <stdio.h>
#include <esp32/rom/ets_sys.h>

// Included ahead of every source (CMakeLists.txt): BlueRetro's printf and ets_printf go
// to the hook set with blueretro_set_log(), never to the console
int br_printf(const char *fmt, ...) __attribute__((__format__(__printf__, 1, 2)));
#define printf(...) br_printf(__VA_ARGS__)
#define ets_printf(...) br_printf(__VA_ARGS__)

// The registered output's name, for the Bluetooth device name
const char *br_output_name(void);
// The adapter changed an output's buffer
void br_output_updated(uint8_t index);

// blueretro_config_access(), blueretro_allow_ota()
bool br_config_access(void);
bool br_ota_allowed(void);
// hci.c: advertising to match br_config_access()
void bt_hci_config_access_changed(void);

#endif /* _BLUERETRO_PRIV_H_ */
