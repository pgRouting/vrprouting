
SET(LOCAL_FILES
    _pgr_pickDeliver.sql
    pgr_pickDeliver.sql

    _pgr_pickDeliverEuclidean.sql
    pgr_pickDeliverEuclidean.sql

    _oneDepot.sql
    oneDepot.sql
    )

foreach (f ${LOCAL_FILES})
    configure_file(${f} ${f})
    list(APPEND PACKAGE_SQL_FILES  ${CMAKE_CURRENT_BINARY_DIR}/${f})
endforeach()

set(PROJECT_SQL_FILES ${PROJECT_SQL_FILES} ${PACKAGE_SQL_FILES} PARENT_SCOPE)
