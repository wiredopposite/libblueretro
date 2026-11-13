# liblfds: only the bounded single-producer single-consumer queue (queue_bss)
set(LFDS "queue_bss/liblfds711")
set(LFDS_QUEUE "${LFDS}/src/lfds711_queue_bounded_singleproducer_singleconsumer")
set(LFDS_SRCS
    "${LFDS_QUEUE}/lfds711_queue_bounded_singleproducer_singleconsumer_cleanup.c"
    "${LFDS_QUEUE}/lfds711_queue_bounded_singleproducer_singleconsumer_dequeue.c"
    "${LFDS_QUEUE}/lfds711_queue_bounded_singleproducer_singleconsumer_enqueue.c"
    "${LFDS_QUEUE}/lfds711_queue_bounded_singleproducer_singleconsumer_init.c"
    "${LFDS_QUEUE}/lfds711_queue_bounded_singleproducer_singleconsumer_query.c"
)

idf_component_register(
    SRCS
        "blueretro.c"
        "adapter/adapter.c"
        "adapter/adapter_debug.c"
        "adapter/config.c"
        "adapter/gameid.c"
        "adapter/hid_parser.c"
        "adapter/kb_monitor.c"
        "adapter/macro.c"
        "adapter/mapping_quirks.c"
        "adapter/memory_card.c"
        "adapter/wireless/hid_generic.c"
        "adapter/wireless/ps.c"
        "adapter/wireless/ps3.c"
        "adapter/wireless/sw.c"
        "adapter/wireless/sw2.c"
        "adapter/wireless/wii.c"
        "adapter/wireless/wireless.c"
        "bluetooth/att.c"
        "bluetooth/att_cfg.c"
        "bluetooth/att_hid.c"
        "bluetooth/debug.c"
        "bluetooth/hci.c"
        "bluetooth/hidp/generic.c"
        "bluetooth/hidp/hidp.c"
        "bluetooth/hidp/ps.c"
        "bluetooth/hidp/ps3.c"
        "bluetooth/hidp/sw.c"
        "bluetooth/hidp/sw2.c"
        "bluetooth/hidp/wii.c"
        "bluetooth/host.c"
        "bluetooth/l2cap.c"
        "bluetooth/mon.c"
        "bluetooth/sdp.c"
        "bluetooth/smp.c"
        "system/delay.c"
        "system/fs.c"
        "system/led.c"
        "system/manager.c"
        "tools/devcrypto.c"
        "tools/stats.c"
        "zephyr/atomic.S"
        "queue_bss/queue_bss.c"
        ${LFDS_SRCS}
    INCLUDE_DIRS "include" "."
    PRIV_INCLUDE_DIRS "queue_bss"
        "${LFDS}/inc"
        "${LFDS}/src/lfds711_queue_bounded_singleproducer_singleconsumer"
        "${LFDS}/src/lfds711_freelist"
        "${LFDS}/src/lfds711_misc"
    REQUIRES bt esp_timer
    PRIV_REQUIRES driver spiffs app_update efuse nvs_flash esp_ringbuf
    LDFRAGMENTS linker.lf
)

target_compile_options(${COMPONENT_LIB} PRIVATE
    -Wno-address-of-packed-member
    -Wno-array-bounds
    -fno-builtin-memset
    -fno-builtin-memcpy
    # BlueRetro's text goes through blueretro_set_log(), never to the console
    "$<$<COMPILE_LANGUAGE:C>:SHELL:-include ${CMAKE_CURRENT_LIST_DIR}/blueretro_priv.h>"
)
target_compile_definitions(${COMPONENT_LIB} PUBLIC BLUERETRO)

# liblfds picks its port from these; the queue is placed in IRAM (linker.lf)
set_source_files_properties(${LFDS_SRCS} "queue_bss/queue_bss.c" PROPERTIES
    COMPILE_OPTIONS "-D__linux__;-D__mips__;-Wno-unknown-pragmas;-Wno-strict-aliasing;-Wno-unused-but-set-variable")
