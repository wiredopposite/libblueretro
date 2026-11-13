# libblueretro

[BlueRetro](https://github.com/darthcloud/BlueRetro)'s Bluetooth host and controller support as an ESP-IDF component, for the ESP32. Bluetooth controllers go in; BlueRetro's generic controller state comes out, mapped by its stored configuration. Your firmware decides what that state becomes, through an output you register.

It runs as ordinary FreeRTOS tasks on core 0, next to the rest of your firmware. BlueRetro's bare-metal core 1, the console wire drivers and the board code are not included.

## Supported controllers

Those BlueRetro supports over Bluetooth Classic and BLE, among them PS3, PS4, PS5, Xbox One and Series (Bluetooth firmware), Switch Pro, Switch 2, Joy-Con, Wii and Wii U Pro, 8BitDo and generic HID pads, keyboards and mice.

## Adding it

As a submodule in your project's `components` directory:

```
git submodule add https://github.com/wiredopposite/libblueretro.git components/blueretro
```

The component's name is the directory's, `blueretro`. Add it to your component's `REQUIRES`.

Requirements:

| | |
|---|---|
| ESP-IDF | 5.5 |
| Target | ESP32 (Bluetooth Classic and BLE) |
| sdkconfig | `CONFIG_BT_ENABLED=y`, `CONFIG_BT_CONTROLLER_ONLY=y`, `CONFIG_BTDM_CTRL_MODE_BTDM=y`, `CONFIG_SPIFFS_MAX_PARTITIONS=1` (settings below) |
| Partitions | A SPIFFS partition (the first one is used) for the configuration and pairing keys |

Settings BlueRetro is tested with:

```
CONFIG_BT_ENABLED=y
CONFIG_BT_CONTROLLER_ONLY=y
CONFIG_BTDM_CTRL_MODE_BTDM=y
CONFIG_BTDM_CTRL_BLE_MAX_CONN=7
CONFIG_BTDM_CTRL_BR_EDR_MAX_ACL_CONN=7
CONFIG_BTDM_CTRL_BR_EDR_SCO_DATA_PATH_HCI=y
CONFIG_BTDM_CTRL_AUTO_LATENCY=y
CONFIG_BTDM_CTRL_HLI=n
CONFIG_BTDM_CTRL_MODEM_SLEEP=n
CONFIG_ESP_COEX_SW_COEXIST_ENABLE=n
CONFIG_SPIFFS_MAX_PARTITIONS=1
```

## Using it

```c
#include "blueretro.h"

static const blueretro_output_t output = {
    .name          = "MyDev",          // in the Bluetooth name: BlueRetro_MyDev_XXXX
    .meta_init     = my_meta_init,     // axis ranges and masks your output supports
    .init_buffer   = my_init_buffer,   // an output back to idle
    .from_generic  = my_from_generic,  // generic state -> your format, in wired_data->output
    .fb_to_generic = my_fb_to_generic, // your feedback (rumble, LEDs) -> generic
    .updated       = my_updated,       // an output's state changed
};

blueretro_set_log(my_vprintf);         // optional; without it the component is silent
blueretro_start(&output);
```

- **Outputs** are BlueRetro's wired ports, `WIRED_MAX_DEV` of them. Each Bluetooth controller drives the output its configuration assigns (the first free one by default). `wired_adapter.data[i]` holds an output's buffer and whether a controller is connected (`connected`).
- **Callbacks** run on the Bluetooth tasks (core 0). `from_generic` reads `struct wired_ctrl`, which lives in IRAM: read it through a `volatile` pointer so the compiler can't narrow word loads to byte loads, which fault there. Build with `-fno-builtin-memset -fno-builtin-memcpy` for the same reason.
- **Feedback**: `blueretro_feedback()` queues a `struct raw_fb` for an output (`header.wired_id`) from any task; your `fb_to_generic` turns it into rumble or LED state for the controller.
- **Pairing**: inquiry starts at boot when no controller is known. `blueretro_pairing(true/false)` starts or stops it; `blueretro_connected()` counts controllers.
- **Logging**: BlueRetro's `printf` and `ets_printf` go to the hook given to `blueretro_set_log()`, never straight to the console, so the UART stays free for your own protocol. Nothing is printed from interrupts.
- **Configuration**: BlueRetro's web configuration page (over BLE) works as usual; settings are kept in the SPIFFS partition.

`blueretro.h` is the API. The headers under `adapter/` (generic state, feedback types) are needed by outputs; the rest is internal.

## Changes from BlueRetro

- An ESP-IDF component instead of an application. The output is registered at run time instead of chosen at build time, and the wired drivers (`main/wired`, `main/adapter/wired/*`), the bare-metal app core, the core 0 stall, the FPGA loader, board and LED pins, and the system manager's power and button handling are removed. Macros and BLE commands still restart, factory-reset and toggle pairing.
- Memory card emulation is a stub (it would take 128 KB of RAM); the BLE interface reads zeros.
- Text goes through `blueretro_set_log()`.
- Feedback can be queued from more than one task.
- `queue_bss` is part of the component, with only the liblfds queue it uses.
- Builds without warnings on GCC 14 (`-Wno-array-bounds` for an intentional read past a struct in the Switch driver).

## License

Apache-2.0, see [LICENSE](LICENSE) and [NOTICE](NOTICE). liblfds (`queue_bss/liblfds711`) is released by its author without a license (public domain).
