SET(LOCAL_FILES
    doc-vrp_knapsack
    doc-vrp_multiple_knapsack
    doc-vrp_bin_packing
    )

foreach (f ${LOCAL_FILES})
    configure_file("${f}.result" "${PGR_DOCUMENTATION_SOURCE_DIR}/${f}.queries")
    list(APPEND LOCAL_DOC_FILES  "${PGR_DOCUMENTATION_SOURCE_DIR}/${f}.queries")
endforeach()
set(PROJECT_DOC_FILES ${PROJECT_DOC_FILES} ${LOCAL_DOC_FILES} PARENT_SCOPE)