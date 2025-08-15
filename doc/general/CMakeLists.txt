
SET(LOCAL_FILES
  concepts.rst
  index.rst
  introduction.rst
  functions.rst
  installation.rst
  release_notes.rst
  sampledata.rst
  support.rst
)

foreach (f ${LOCAL_FILES})
  configure_file(${f} "${PGR_DOCUMENTATION_SOURCE_DIR}/${f}")
  list(APPEND LOCAL_DOC_FILES  ${PGR_DOCUMENTATION_SOURCE_DIR}/${f})
endforeach()

set(PROJECT_DOC_FILES ${PROJECT_DOC_FILES} ${LOCAL_DOC_FILES} PARENT_SCOPE)

