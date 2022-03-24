idf_component_register(SRCS "main.c"
	REQUIRES led temperature_sensor
	# although esptool_py does not generate static library,
	# the component is needed for flashing related targets and file generation
	esptool_py
	)
