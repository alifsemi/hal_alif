zephyr_include_directories(
  plf
)

zephyr_sources(
  plf/alif_ble.c
  plf/hci_uart.c
  plf/host_timer_kernel.c
  plf/sync_timer.c
)

zephyr_linker_sources_ifdef(CONFIG_ALIF_BLE_HOST_PATCHING SECTIONS ble_patch_info.ld)
zephyr_linker_sources_ifdef(CONFIG_ALIF_BLE_HOST_PATCHING RAM_SECTIONS ble_patch_ramable.ld)

add_subdirectory_ifdef(CONFIG_ALIF_BLE_ROM_IMAGE_V1_0 v1_0)
add_subdirectory_ifdef(CONFIG_ALIF_BLE_ROM_IMAGE_V1_2 v1_2)
