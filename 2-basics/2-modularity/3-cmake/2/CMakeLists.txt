# This is a built-in CMake commmand that specifies the minimum version
# of cmake that should be used.
# If a cmake with lower version is used, an error is raised.
#
# It is the first command that should be used in the top-level CMakeLists.txt
#
# We will use CMake version 3.5, which is also the version used by ESP-IDF.
#
# It is worth noting that "historically" there has been a paradigm shift
# in CMake which started from version 2.8 and arrived to a stable and
# "feature-full" state in 3.2. This is the "modern CMake", which resolves
# around targets.
# So when searching resources on Internet, mind the version that is being used.
cmake_minimum_required(VERSION 3.5)

# This is a built-in CMake commmand that sets up a project.
# The use of the project() command in the top-level CMakeLists.txt is special
# because it kicks off a series of checks that CMake performs to ensure that
# the compiler exists and are working fine.
# of cmake that should be used.
# If a cmake with lower version is used, an error is raised.
#
# See more https://cmake.org/cmake/help/v3.5/command/project.html
project(2-modularity)

# Define a variable called TARGET_NAME with value ${PROJECT_NAME}
# Variables in CMake work similarily to shells scripting (e.g. bash on Linux, batch on Windows):
# 	* basically variables are all strings.
# 	* to obtain their value, use the syntax ${VARIABLE_NAME}:
#
# PROJECT_NAME is a predefined variable of CMake that contains: the name of the
# project passed to the project command.
# So in this case the value of TARGET_NAME should be 2-modularity
set(TARGET_NAME ${PROJECT_NAME})

# Define a CACHE variable called CONFIGURATION_FAKE_IMPL and use the
# "user_input" string as its default value.
# We use this variable to decide which of the fake implementations to compile.
#
# * CACHE variables are preserved across cmake invokation. You can find them
# stored in the CMakeCache.txt file in the build directory.
# * CACHE variables are typically used as customization points for the build.
set(CONFIGURATION_FAKE_IMPL "user_input" CACHE STRING "")

# Create an executable target, whose name will be ${TARGET_NAME}
# All the sources files of the executable target will be compiled into object
# files add_dependencies then linked together to produce an executable file
# that has as name the name of the target.
#
# For what concerns us, the CMake world revolves around targets, which can be
# considered like objects in OOP (Object Oriented Programming).
# An executable is one type of target. Other targets are "static library",
# "shared library", etc.
add_executable(${TARGET_NAME})

# Add source files to the executable target that we just created
target_sources(${TARGET_NAME}
	PRIVATE
	main.c
	)

# Look for a CMakeLists.txt file in the given subdirectory and execute it
add_subdirectory(configuration)
add_subdirectory(spi_bus)
add_subdirectory(temperature_sensor)

# Since main.c uses all the libraries, link all the libraries created in the
# subdirectories
target_link_libraries(${TARGET_NAME}
	PRIVATE
	configuration
	spi_bus
	temperature_sensor
	)
