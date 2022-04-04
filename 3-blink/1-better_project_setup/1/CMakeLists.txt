cmake_minimum_required(VERSION 3.5)

# Add ../../components/ as component search dir
# This allows ESP-IDF to find "../../components/led_strip"
set(EXTRA_COMPONENT_DIRS ${CMAKE_CURRENT_LIST_DIR}/../../components/)

# We don't want all the components to be built! 
# Set main as the only required component.
# Additional components will be included as needed based on dependency graph
# starting from what main REQUIRES
set(COMPONENTS main
	# although esptool_py does not generate static library,
	# the component is needed for flashing-related targets and file generation
	esptool_py
	)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(blink)
