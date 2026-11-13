/*
 * Copyright (c) 2019-2025, Jacques Gagnon
 * SPDX-License-Identifier: Apache-2.0
 */

// Start-up, the registered output, and the text hook. BlueRetro's own code prints with
// printf and ets_printf; the component is built with both renamed to br_printf.

#include <stdio.h>
#include <nvs_flash.h>
#include <freertos/FreeRTOS.h>

#include "blueretro.h"
#include "blueretro_priv.h"
#include "adapter/config.h"
#include "adapter/wired/wired.h"
#include "bluetooth/host.h"
#include "bluetooth/hci.h"
#include "system/fs.h"

static const blueretro_output_t *Output;
static volatile blueretro_log_t Log;
static volatile bool ConfigAccess = true;      // blueretro_config_access()
static volatile bool OtaAllowed;               // blueretro_allow_ota()
static volatile bool Running;                  // Bluetooth started

void blueretro_set_log(blueretro_log_t log)
{
    Log = log;
}

int br_printf(const char *fmt, ...)
{
    const blueretro_log_t log = Log;
    if (log == NULL || xPortInIsrContext()) {
        return 0;
    }
    va_list ap;
    va_start(ap, fmt);
    const int n = log(fmt, ap);
    va_end(ap);
    return n;
}

const char *br_output_name(void)
{
    return (Output != NULL && Output->name != NULL) ? Output->name : "";
}

void br_output_updated(uint8_t index)
{
    if (Output != NULL && Output->updated != NULL) {
        Output->updated(index);
    }
}

// ---- adapter/wired/wired.h: the output's side of the adapter

int32_t wired_meta_init(struct wired_ctrl *ctrl_data)
{
    if (Output == NULL || Output->meta_init == NULL) {
        return -1;
    }
    Output->meta_init(ctrl_data);
    return 0;
}

void IRAM_ATTR wired_init_buffer(int32_t dev_mode, struct wired_data *wired_data)
{
    if (Output != NULL && Output->init_buffer != NULL) {
        Output->init_buffer(dev_mode, wired_data);
    }
}

void wired_from_generic(int32_t dev_mode, struct wired_ctrl *ctrl_data, struct wired_data *wired_data)
{
    if (Output != NULL && Output->from_generic != NULL) {
        Output->from_generic(dev_mode, ctrl_data, wired_data);
    }
}

void wired_fb_to_generic(int32_t dev_mode, struct raw_fb *raw_fb_data, struct generic_fb *fb_data)
{
    if (Output != NULL && Output->fb_to_generic != NULL) {
        Output->fb_to_generic(dev_mode, raw_fb_data, fb_data);
    }
}

// ---- public

bool blueretro_start(const blueretro_output_t *output)
{
    if (output == NULL || Output != NULL) {
        return false;
    }
    Output = output;
    adapter_init();         // buffers and the feedback queue
    if (fs_init()) {
        br_printf("# BlueRetro: no SPIFFS partition\n");
        return false;
    }
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        err = nvs_flash_init();
    }
    if (err != ESP_OK) {
        br_printf("# BlueRetro: NVS init failed\n");
        return false;
    }
    config_init(DEFAULT_CFG);
    for (uint32_t i = 0; i < WIRED_MAX_DEV; i++) {
        adapter_init_buffer(i);
    }
    Running = bt_host_init() == 0;
    return Running;
}

void blueretro_feedback(struct raw_fb *fb)
{
    adapter_q_fb(fb);
}

void blueretro_pairing(bool on)
{
    if (on) {
        bt_hci_start_inquiry();
    } else {
        bt_hci_stop_inquiry();
    }
}

bool blueretro_is_pairing(void)
{
    return bt_hci_get_inquiry() != 0;
}

uint32_t blueretro_connected(void)
{
    return bt_host_get_flag_dev_cnt(BT_DEV_HID_INIT_DONE);
}

void blueretro_config_access(bool on)
{
    if (on == ConfigAccess) {
        return;
    }
    ConfigAccess = on;
    if (Running) {
        bt_hci_config_access_changed();
    }
}

bool blueretro_config_access_on(void)
{
    return ConfigAccess;
}

void blueretro_allow_ota(bool allow)
{
    OtaAllowed = allow;
}

bool br_config_access(void)
{
    return ConfigAccess;
}

bool br_ota_allowed(void)
{
    return OtaAllowed;
}
