
zephyr_include_directories(include)
zephyr_include_directories(
  ieee802154/include
  ieee802154/src
)
zephyr_sources(src/alif_ahi.c src/alif_mac154_api.c)

zephyr_library_sources_ifdef(CONFIG_IEEE802154_ALIF_TX_ENCRYPT
  src/alif_mac154_parser.c
  src/alif_mac154_key_storage.c
  src/alif_mac154_ccm_encode.c
)

if(CONFIG_OPENTHREAD)
  zephyr_sources(src/alif_ot_plf.c)
endif()

zephyr_library_sources_ifdef(CONFIG_INTEL_HDA
		${SOF_DRIVERS_PATH}/intel/hda/hda-dma.c
	)
if(CONFIG_IEEE802154_ALIF_TX_ENCRYPT)
zephyr_library_link_libraries_ifdef(CONFIG_MBEDTLS mbedTLS)
endif()

set(MAC154_LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib/lib802154.a)

if(CONFIG_FPU)
    if(CONFIG_FP_HARDABI)
      set(MAC154_LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib/lib802154_hf.a)
    endif()
endif()

zephyr_link_libraries(${MAC154_LIB})
