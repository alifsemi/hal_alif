zephyr_include_directories(
  plf
)

zephyr_sources(
  plf/alif_ble.c
  plf/hci_uart.c
  plf/host_timer_kernel.c
  plf/sync_timer.c
)

add_subdirectory_ifdef(CONFIG_ALIF_BLE_ROM_IMAGE_V1_2 v1_2)
