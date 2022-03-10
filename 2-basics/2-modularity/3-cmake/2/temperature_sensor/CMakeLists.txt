set(TARGET_NAME "temperature_sensor")
add_library(${TARGET_NAME} STATIC "")

target_sources(${TARGET_NAME}
	PRIVATE
	temperature_sensor.c
	)

target_include_directories(${TARGET_NAME}
	PUBLIC
	${CMAKE_CURRENT_LIST_DIR}
	)

