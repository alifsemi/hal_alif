# The Alif Ethos-U cache-maintenance callbacks are Cortex-M specific: they use
# CMSIS-M SCB_* cache intrinsics (cmsis_core.h) and TCM local->global address
# remapping (soc_memory_map.h), neither of which exist on APSS (Cortex-A32).
# On APSS the tensor arena lives in non-cacheable SRAM1, so the Ethos-U driver's
# weak NOP cache ops and identity address remap are already correct.
if(CONFIG_CPU_CORTEX_M)
  zephyr_include_directories( include )
  zephyr_library()
  zephyr_library_sources(src/ethosu_callback.c)
endif()