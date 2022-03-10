# Set TARGET_NAME to "configuration"
#
# For the CMakeLists.txt of a subdirectory a new variable scope is created.
# All the variables from the parent scope are copied. 
# Changes to variables in this new scope does not affect the variables in the
# parent scope.
#
# So it's not a variable to re-define TARGET_NAME to another variable
set(TARGET_NAME "configuration")
# Create a static library with name ${TARGET_NAME}, i.e. configuration
add_library(${TARGET_NAME} STATIC "")

# Add common source file
target_sources(${TARGET_NAME}
	PRIVATE
	common.c
	)

# Add ${CMAKE_CURRENT_LIST_DIR} as include directory.
# 
# * CMAKE_CURRENT_LIST_DIR is a builtin variable of CMake whose value represent
# the directory of the current CMake file that is being executed.
# * PUBLIC means that: any target who **links** this library target must also use
# these include directories.
target_include_directories(${TARGET_NAME}
	PUBLIC
	${CMAKE_CURRENT_LIST_DIR}
	)

# Reminder: configuration.h includes temperature_sensor.h, since struct
# application_config contains struct temperature_sensor_config.
#
# Since the configuration module depends on the temperature_sensor module
# we link the temperature_sensor library target to this target.
# This way, anything that has been specified as PUBLIC (source files, include directories, link
# libraries) for the temperature_sensor library target will be added also to this target.
# 
# Meaning of PUBLIC: any target that links this target will also link all the targets linked to this target via PUBLIC.
target_link_libraries(${TARGET_NAME}
	PUBLIC
	temperature_sensor
	)

#
# Conditionally add the source file depending on the value of CONFIGURATION_FAKE_IMPL
#
if (CONFIGURATION_FAKE_IMPL STREQUAL "default")
	target_sources(${TARGET_NAME}
		PRIVATE
		fake_default.c
		)
elseif(CONFIGURATION_FAKE_IMPL STREQUAL "file")
	target_sources(${TARGET_NAME}
		PRIVATE
		fake_file.c
		)
elseif(CONFIGURATION_FAKE_IMPL STREQUAL "user_input")
	target_sources(${TARGET_NAME}
		PRIVATE
		fake_user_input.c
		)
else()
	message(FATAL_ERROR "Unsupported value \"${CONFIGURATION_FAKE_IMPL}\" for CONFIGURATION_FAKE_IMPL")
endif()
