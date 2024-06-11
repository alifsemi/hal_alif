# Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
# Use, distribution and modification of this code is permitted under the
# terms stated in the Alif Semiconductor Software License Agreement
#
# You should have received a copy of the Alif Semiconductor Software
# License Agreement with this file. If not, please write to:
# contact@alifsemi.com, or visit: https://alifsemi.com/license

zephyr_library()
zephyr_library_sources_ifdef(CONFIG_DT_HAS_ALIF_MRAM_FLASH_CONTROLLER_ENABLED common/src/mram_rw.c)

if(CONFIG_USE_ALIF_HAL_OSPI)
  zephyr_include_directories(
    drivers/ospi/include
    )

  zephyr_library_sources(drivers/ospi/src/ospi.c)
  zephyr_library_sources(drivers/ospi/src/ospi_hal.c)

endif()

if(CONFIG_HAS_ALIF_SE_SERVICES)
  zephyr_include_directories(
    se_services/include
    se_services/zephyr/include
    )
  zephyr_library_sources(se_services/zephyr/src/se_service.c)
endif()
zephyr_include_directories(
  common/include
)

add_subdirectory_ifdef(CONFIG_PM drivers/pm)
