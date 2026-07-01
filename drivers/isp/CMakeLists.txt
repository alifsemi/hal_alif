if (CONFIG_USE_ALIF_ISP_LIB)
  target_link_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/isp_wrapper/Lib)

  zephyr_link_libraries(isp_gcc)

  zephyr_include_directories(isp_wrapper/inc
                             isp_wrapper/inc/lib)

  zephyr_library()
  zephyr_library_sources(isp_wrapper/src/isp_api_wrapper.c)

  if (CONFIG_ISP_SENSOR_OV5675)
    zephyr_library_sources(isp_wrapper/src/ov5675_isp_param_conf.c)
    zephyr_library_sources(isp_wrapper/src/ov5675_sensor_attributes.c)
  endif()

  if (CONFIG_ISP_SENSOR_ARX3A0)
    zephyr_library_sources(isp_wrapper/src/arx3a0_isp_param_conf.c)
    zephyr_library_sources(isp_wrapper/src/arx3a0_sensor_attributes.c)
  endif()
endif()
