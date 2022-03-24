cmake_minimum_required(VERSION 3.5)

# The auto generated sdkconfig goes into the build directory
set(SDKCONFIG ${CMAKE_BINARY_DIR}/sdkconfig)
# The sdkconfig files that contain overrides of the default settings
set(SDKCONFIG_DEFAULTS
	${CMAKE_SOURCE_DIR}/sdkconfig.defaults
	)

set(EXTRA_COMPONENT_DIRS
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/src/components
	)

# We don't want all the components to be built! 
# Set main as the only required component.
# additional components will be included as needed based on dependency graph
set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(3-project_template)
