/*
 * Copyright (c) 2019-2025, Jacques Gagnon
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLUERETRO_H_
#define _BLUERETRO_H_

// BlueRetro's Bluetooth host and controller drivers as a component: Bluetooth
// controllers in, BlueRetro's generic controller state out (struct wired_ctrl, mapped by
// the stored configuration). What that state becomes is up to the output the user
// registers; the component has no output of its own.
//
// The Bluetooth tasks run on core 0. Output callbacks are called from them.

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

#include "adapter/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    // Short system name in the adapter's Bluetooth name ("BlueRetro_<name>_XXXX", <= 6)
    const char *name;
    // Axis ranges and the button/axis masks the output supports, for every output index
    void (*meta_init)(struct wired_ctrl *ctrl_data);
    // An output's buffer back to idle (start, controller gone)
    void (*init_buffer)(int32_t dev_mode, struct wired_data *wired_data);
    // Generic state to the output's format, in wired_data->output
    void (*from_generic)(int32_t dev_mode, struct wired_ctrl *ctrl_data, struct wired_data *wired_data);
    // Feedback queued with blueretro_feedback() to generic feedback (rumble, LEDs)
    void (*fb_to_generic)(int32_t dev_mode, struct raw_fb *raw_fb_data, struct generic_fb *fb_data);
    // After init_buffer or from_generic for that output index
    void (*updated)(uint8_t index);
} blueretro_output_t;

// Where the component's text goes (it never prints by itself). NULL: dropped.
typedef int (*blueretro_log_t)(const char *fmt, va_list ap);
void blueretro_set_log(blueretro_log_t log);

// Mounts the first SPIFFS partition for the configuration and pairing keys, initialises
// NVS if needed, and starts Bluetooth. The output must stay valid.
bool blueretro_start(const blueretro_output_t *output);

// Feedback for an output index (raw_fb.header.wired_id), turned into generic feedback by
// the output's fb_to_generic. Any task.
void blueretro_feedback(struct raw_fb *fb);

// Inquiry: looking for new controllers to pair
void blueretro_pairing(bool on);
bool blueretro_is_pairing(void);
// Controllers connected now
uint32_t blueretro_connected(void);

#ifdef __cplusplus
}
#endif

#endif /* _BLUERETRO_H_ */
