set(TARGET_NAME "spi_bus")
add_library(${TARGET_NAME} STATIC "")

target_sources(${TARGET_NAME}
	PRIVATE
	spi_bus.c
	)

target_include_directories(${TARGET_NAME}
	PUBLIC
	${CMAKE_CURRENT_LIST_DIR}
	)
