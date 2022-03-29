cmake_minimum_required(VERSION 3.10)

project(UniBZ-76088A-slides
	# We're not using any programming language
	NONE)


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
	--pdf-engine-opt=-shell-escape
	)

# I don't know. Apparently in order to use minted in pandoc beamer I have to 
# pass through the intermidiate latex 
# See https://github.com/jgm/pandoc/issues/4721
function(ADD_BEAMER_USING_LATEX_INTERMEDIATE_TARGET _DIRECTORY _TARGET_NAME _EXTRA_OPTS)
	set(BEAMER_LATEX_OUTPUT ${CMAKE_BINARY_DIR}/output/latex/${_DIRECTORY}_${_TARGET_NAME}.tex)
	set(BEAMER_LATEX_TARGET ${_DIRECTORY}_${_TARGET_NAME}_latex)
	add_custom_command(OUTPUT ${BEAMER_LATEX_OUTPUT}
		COMMAND pandoc -t beamer
		${COMMON_ARGS}
		${_EXTRA_OPTS}
		${COMMON_PRE_FILES} main.md ${COMMON_POST_FILES}
		-o ${BEAMER_LATEX_OUTPUT}
		DEPENDS ${COMMON_DEPENDENCIES} ${_DIRECTORY}/main.md
		WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${_DIRECTORY}
		VERBATIM
		)
	add_custom_target(${BEAMER_LATEX_TARGET} DEPENDS ${BEAMER_LATEX_OUTPUT})

	add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/output/beamer/${_DIRECTORY}_${_TARGET_NAME}.pdf
		COMMAND latexmk -pdf
		--shell-escape
		-jobname=${_DIRECTORY}_${_TARGET_NAME}
		# Minted doesn't work well with output-directory...
		# -output-directory=${CMAKE_BINARY_DIR}/output/latex-build/
		${BEAMER_LATEX_OUTPUT}
		COMMAND cp ${CMAKE_CURRENT_LIST_DIR}/${_DIRECTORY}/${_DIRECTORY}_${_TARGET_NAME}.pdf
		${CMAKE_BINARY_DIR}/output/beamer/${_DIRECTORY}_${_TARGET_NAME}.pdf
		DEPENDS ${BEAMER_LATEX_OUTPUT}
		WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${_DIRECTORY}
		VERBATIM
		)
	add_custom_target(${_DIRECTORY}_${_TARGET_NAME}_pdf ALL DEPENDS ${CMAKE_BINARY_DIR}/output/beamer/${_DIRECTORY}_${_TARGET_NAME}.pdf)
endfunction()

# Sometimes passing through the intermidiate latex is not okay.
# In particular when SVGs are involved. Pandoc itself takes care of converting
# SVGs to other format when converting directly e.g. to PDF. But when
# converting to latex, Pandoc doesn't do this and handling SVGs in Latex
# is another small PITA.
function(ADD_BEAMER_TARGET _DIRECTORY _TARGET_NAME _EXTRA_OPTS)
	set(BEAMER_LATEX_OUTPUT ${CMAKE_BINARY_DIR}/output/latex/${_DIRECTORY}_${_TARGET_NAME}.tex)
	set(BEAMER_LATEX_TARGET ${_DIRECTORY}_${_TARGET_NAME}_latex)
	add_custom_command(OUTPUT ${BEAMER_LATEX_OUTPUT}
		COMMAND pandoc -t beamer
		${COMMON_ARGS}
		${_EXTRA_OPTS}
		${COMMON_PRE_FILES} main.md ${COMMON_POST_FILES}
		-o ${BEAMER_LATEX_OUTPUT}
		DEPENDS ${COMMON_DEPENDENCIES} ${_DIRECTORY}/main.md
		WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${_DIRECTORY}
		VERBATIM
		)
	add_custom_target(${BEAMER_LATEX_TARGET} DEPENDS ${BEAMER_LATEX_OUTPUT})

	set(BEAMER_OUTPUT ${CMAKE_BINARY_DIR}/output/beamer/${_DIRECTORY}_${_TARGET_NAME}.pdf)
	add_custom_command(OUTPUT ${BEAMER_OUTPUT}
		COMMAND pandoc -t beamer
		${COMMON_ARGS}
		${_EXTRA_OPTS}
		${COMMON_PRE_FILES} main.md ${COMMON_POST_FILES}
		-o ${BEAMER_OUTPUT}
		DEPENDS ${COMMON_DEPENDENCIES} ${_DIRECTORY}/main.md
		WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${_DIRECTORY}
		VERBATIM
		)
	add_custom_target(${_DIRECTORY}_${_TARGET_NAME}_pdf ALL DEPENDS ${BEAMER_OUTPUT})
endfunction()

function(ADD_DIRECT_SLIDE _DIRECTORY)
	add_beamer_target(${_DIRECTORY} "slides" "")
	add_beamer_target(${_DIRECTORY} "handout" "-V classoption=handout")
endfunction()

function(ADD_LATEX_INTERMEDIATE_SLIDE _DIRECTORY)
	add_beamer_using_latex_intermediate_target(${_DIRECTORY} "slides" "")
	add_beamer_using_latex_intermediate_target(${_DIRECTORY} "handout" "-V classoption=handout")
endfunction()

add_direct_slide(2022-03-03-introduction_to_embedded_programming)
add_direct_slide(2022-03-16-practical_programming_in_c)
add_latex_intermediate_slide(2022-03-30-programming_paradigms)
