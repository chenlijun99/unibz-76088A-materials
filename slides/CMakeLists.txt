cmake_minimum_required(VERSION 3.10)

project(UniBZ-76088A-slides
	# We're not using any programming language
	NONE)

function(ADD_SLIDE _DIRECTORY)
	set(BEAMER_OUTPUT ${CMAKE_BINARY_DIR}/output/beamer/${_DIRECTORY}_handout.pdf)

	set(COMMON_DEPENDENCIES
		common/assets/logo.png
		common/metadata.yaml
		common/header.tex
		common/epilogue.md
		common/references.bib
		common/ieee.csl
		)
	set(COMMON_PRE_FILES
		../common/metadata.yaml
		)
	set(COMMON_POST_FILES
		../common/epilogue.md
		)
	set(COMMON_ARGS
		-H ../common/header.tex
		--filter pandoc-citeproc
		)

	add_custom_command(OUTPUT ${BEAMER_OUTPUT}
		COMMAND pandoc -t beamer
		-V classoption=handout
		${COMMON_ARGS}
		${COMMON_PRE_FILES} main.md ${COMMON_POST_FILES}
		-o ${BEAMER_OUTPUT}
		DEPENDS ${COMMON_DEPENDENCIES} ${_DIRECTORY}/main.md
		WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${_DIRECTORY}
		VERBATIM
		)
	add_custom_target(${_DIRECTORY}_beamer_handout ALL DEPENDS ${BEAMER_OUTPUT})

	set(BEAMER_OUTPUT ${CMAKE_BINARY_DIR}/output/beamer/${_DIRECTORY}_slides.pdf)
	add_custom_command(OUTPUT ${BEAMER_OUTPUT}
		COMMAND pandoc -t beamer
		${COMMON_ARGS}
		${COMMON_PRE_FILES} main.md ${COMMON_POST_FILES}
		-o ${BEAMER_OUTPUT}
		DEPENDS ${COMMON_DEPENDENCIES} ${_DIRECTORY}/main.md
		WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${_DIRECTORY}
		VERBATIM
		)
	add_custom_target(${_DIRECTORY}_beamer_slides ALL DEPENDS ${BEAMER_OUTPUT})

	set(BEAMER_OUTPUT ${CMAKE_BINARY_DIR}/latex/${_DIRECTORY}.tex)
	add_custom_command(OUTPUT ${BEAMER_OUTPUT}
		COMMAND pandoc -t beamer
		${COMMON_ARGS}
		${COMMON_PRE_FILES} main.md ${COMMON_POST_FILES}
		-o ${BEAMER_OUTPUT}
		DEPENDS ${COMMON_DEPENDENCIES} ${_DIRECTORY}/main.md
		WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${_DIRECTORY}
		VERBATIM
		)
	add_custom_target(${_DIRECTORY}_beamer_latex DEPENDS ${BEAMER_OUTPUT})
endfunction()

add_slide(2022-03-03-introduction_to_embedded_programming)
add_slide(2022-03-16-practical_programming_in_c)
