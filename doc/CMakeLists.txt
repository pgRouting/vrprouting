#---------------------------------------------
# Checking for prerequisites
#---------------------------------------------
find_program(SPHINX_EXECUTABLE
  NAMES sphinx-build
  DOC "Sphinx Documentation Builder (sphinx-doc.org)"
)

if (NOT SPHINX_EXECUTABLE)
  message(WARNING "Sphinx not found.
  Building without documentation")
  return()
endif()

set(SPHINX_FLAGS "" CACHE STRING "Flags to pass to sphinx-build")
separate_arguments(sphinx_flags UNIX_COMMAND "${SPHINX_FLAGS}")

option(SPHINX_HTML "Set ON|OFF (default=OFF) to build Documentation library tree as HTML" ON)
option(SPHINX_PDF "Set ON|OFF (default=OFF) to build Documentation library tree as PDF" OFF)
option(SPHINX_LOCALE "Set ON|OFF (default=OFF) to build the locale files" OFF)
option(SPHINX_LINKCHECK "Set ON|OFF (default=OFF) to build Documentation library tree as LINKCHECK page" OFF)

# get the ON options
foreach(opt SPHINX_HTML SPHINX_LOCALE SPHINX_LINKCHECK)
  if (${${opt}})
    string(TOLOWER ${opt} val)
    string(REPLACE "sphinx_" "" val ${val})
    list(APPEND PROJECT_DOC_TARGETS ${val})
  endif()
endforeach()

message(STATUS "PROJECT_DOC_TARGETS = ${PROJECT_DOC_TARGETS}")
if("${PROJECT_DOC_TARGETS}" STREQUAL "")
  message(WARNING "No documentation targets found
  Building without documentation")
  return()
endif()

add_custom_target(doc ALL COMMENT "Building documentation with Sphinx")

#---------------------------------------------
# LANGUAGES SETTINGS
#---------------------------------------------
set(PROJECT_LANGUAGES "en")

foreach(lang ${PROJECT_LANGUAGES})
  string(TOUPPER ${lang} val)
  if (lang STREQUAL "en")
    option(${val} "Set ON|OFF (default=ON) build ${lang} Documentation" ON)
  else()
    option(${val} "Set ON|OFF (default=OFF) build ${lang} Documentation" OFF)
  endif()
endforeach()

#---------------------------------------------
# Catching the language options to be build
#---------------------------------------------
foreach(lang ${PROJECT_LANGUAGES})
  string(TOUPPER ${lang} opt)
  if (${opt})
    list(APPEND PROJECT_BUILD_LANGUAGES ${lang})
  endif()
endforeach()

if("${PROJECT_BUILD_LANGUAGES}" STREQUAL "")
  message(WARNING "No documentation languages found
  Building without documentation")
  return()
endif()

message(STATUS "PROJECT_BUILD_LANGUAGES = ${PROJECT_BUILD_LANGUAGES}")


# configured documentation tools and intermediate build results
set(PGR_DOCUMENTATION_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/_build")
set(PGR_DOCUMENTATION_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}")

message(STATUS "PGR_DOCUMENTATION_BUILD_DIR = ${PGR_DOCUMENTATION_BUILD_DIR}")
message(STATUS "PGR_DOCUMENTATION_SOURCE_DIR = ${PGR_DOCUMENTATION_SOURCE_DIR}")


#---------------------------------------------
# Adding documentation directories
#---------------------------------------------
foreach (subdir ${PROJECT_DOC_DIRECTORIES})
  add_subdirectory("${subdir}")
  if (EXISTS "${CMAKE_SOURCE_DIR}/docqueries/${subdir}")
    add_subdirectory("${CMAKE_SOURCE_DIR}/docqueries/${subdir}" "build/doc/${subdir}")
  endif()
endforeach()


configure_file("../tools/testers/sampledata.sql" "${PGR_DOCUMENTATION_SOURCE_DIR}/sampledata/sampledata.sql")


add_subdirectory("_static")
add_subdirectory("_templates")

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in"
  "${PGR_DOCUMENTATION_SOURCE_DIR}/conf.py"
  @ONLY)

# Add documentation to targets


string(REPLACE ";" "," SPHINXINTL_LANGUAGE "${PROJECT_BUILD_LANGUAGES}")
message(STATUS "SPHINXINTL_LANGUAGE=${SPHINXINTL_LANGUAGE}")

foreach (format ${PROJECT_DOC_TARGETS})

  if ("${format}" STREQUAL "locale")

    find_program(SPHINX_INTL_EXECUTABLE
      NAMES sphinx-intl
      DOC "Sphinx Documentation Builder (sphinx-doc.org)"
    )

    if (SPHINX_INTL_EXECUTABLE)

      add_custom_target(locale
        COMMAND ${SPHINX_EXECUTABLE}
        -b gettext
        -c "${PGR_DOCUMENTATION_SOURCE_DIR}"
        "${PGR_DOCUMENTATION_SOURCE_DIR}"
        "${CMAKE_SOURCE_DIR}/locale/pot"

        COMMAND ${SPHINX_INTL_EXECUTABLE} update
        -d ${CMAKE_SOURCE_DIR}/locale
        -l ${SPHINXINTL_LANGUAGE} > locale_changes.txt

        COMMENT "Generating POT and PO files"
        SOURCES ${PROJECT_DOC_FILES})

    else()

      message(WARNING "sphinx-intl not found.
      Not building the pot and po locale files")

    endif()

  else()

    add_custom_target("${format}"
      COMMENT "Building ${format} documentation with Sphinx")

    configure_file(
      "${CMAKE_CURRENT_SOURCE_DIR}/forward.html"
      "${PGR_DOCUMENTATION_BUILD_DIR}/${format}/index.html")

    foreach (lang ${PROJECT_BUILD_LANGUAGES})
      add_custom_target(
        "${format}-${lang}"
        COMMAND ${SPHINX_EXECUTABLE}
        -c ${CMAKE_CURRENT_BINARY_DIR}
        -d ${CMAKE_CURRENT_BINARY_DIR}/doctrees
        -b ${format}
        -D language='${lang}'
        ${sphinx_flags}
        "${PGR_DOCUMENTATION_SOURCE_DIR}"
        "${PGR_DOCUMENTATION_BUILD_DIR}/${format}/${lang}"

        COMMENT "Generating ${format}/${lang} documentation"
        SOURCES ${PROJECT_DOC_FILES}  ${PROJECT_IMG_FILES}
        DEPENDS ${PROJECT_DOC_FILES}  ${PROJECT_IMG_FILES}
      )
      add_dependencies(${format} "${format}-${lang}")

    endforeach()
    add_dependencies(doc "${format}")

  endif()
endforeach()
