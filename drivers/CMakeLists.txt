add_subdirectory_ifdef(CONFIG_USE_ALIF_HAL_OSPI     ospi)

add_subdirectory_ifdef(CONFIG_ARM_ETHOS_U           ethos_u)

add_subdirectory_ifdef(CONFIG_USE_ALIF_HAL_UTIMER   utimer)

add_subdirectory(dma_event_router)

add_subdirectory_ifdef(CONFIG_USE_ALIF_HAL_ANALOG   analog)
