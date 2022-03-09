cmake_minimum_required(VERSION 3.5)

# The auto generated sdkconfig goes into the build directory
set(SDKCONFIG ${CMAKE_BINARY_DIR}/sdkconfig)
# The sdkconfig files that contain overrides of the default settings
set(SDKCONFIG_DEFAULTS
	${CMAKE_SOURCE_DIR}/sdkconfig.defaults
	)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(hello-world)
