zephyr_include_directories(include)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/rom_ble_stack.patch.elf)
	message(STATUS "Found ROM BLE stack patch: Linking it")
	zephyr_link_libraries(${CMAKE_CURRENT_SOURCE_DIR}/rom_ble_stack.patch.elf)
	zephyr_linker_sources(SECTIONS ../ble_patch_info.ld)
	zephyr_linker_sources(RAM_SECTIONS ../ble_patch_ramable.ld)
endif()
