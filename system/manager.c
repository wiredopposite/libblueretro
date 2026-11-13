/*
 * Copyright (c) 2019-2025, Jacques Gagnon
 * SPDX-License-Identifier: Apache-2.0
 */

// The commands BlueRetro's macros and BLE config send. The component has no adapter
// buttons or power pins, so power commands only drop Bluetooth.

#include <esp_system.h>

#include "bluetooth/host.h"
#include "bluetooth/hci.h"
#include "system/fs.h"
#include "manager.h"

void sys_mgr_init(uint32_t package)
{
    (void)package;
}

void sys_mgr_cmd(uint8_t cmd)
{
    switch (cmd) {
    case SYS_MGR_CMD_INQ_TOOGLE:
        if (bt_hci_get_inquiry()) {
            bt_hci_stop_inquiry();
        } else {
            bt_hci_start_inquiry();
        }
        break;
    case SYS_MGR_CMD_PWR_OFF:
    case SYS_MGR_CMD_DEEP_SLEEP:
        bt_host_disconnect_all();
        break;
    case SYS_MGR_CMD_FACTORY_RST:
        fs_reset();
        bt_host_disconnect_all();
        esp_restart();
        break;
    case SYS_MGR_CMD_RST:
    case SYS_MGR_CMD_ADAPTER_RST:
        bt_host_disconnect_all();
        esp_restart();
        break;
    default:
        break;
    }
}
