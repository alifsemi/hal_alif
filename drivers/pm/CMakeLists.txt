#
# Copyright (C) 2024  Alif Semiconductor
#
# SPDX-License-Identifier: Apache-2.0
#

zephyr_include_directories_ifdef(CONFIG_PM_RTSS
  rtss
)

zephyr_sources_ifdef(CONFIG_PM_RTSS rtss/pm_rtss.c)
