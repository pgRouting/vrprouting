SET(LOCAL_FILES
    _vrp_vroom.sql
    vrp_vroomPlain.sql
    vrp_vroomJobsPlain.sql
    vrp_vroomShipmentsPlain.sql
    )

foreach (f ${LOCAL_FILES})
    configure_file(${f} ${f})
    list(APPEND PACKAGE_SQL_FILES  ${CMAKE_CURRENT_BINARY_DIR}/${f})
endforeach()

set(PROJECT_SQL_FILES ${PROJECT_SQL_FILES} ${PACKAGE_SQL_FILES} PARENT_SCOPE)
