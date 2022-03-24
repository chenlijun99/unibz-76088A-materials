idf_component_register(SRCS "temperature_sensor.c"
	INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}
	PRIV_REQUIRES vfs
	)
