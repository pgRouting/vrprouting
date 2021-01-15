ADD_LIBRARY(common OBJECT
    postgres_connection.c
    e_report.c
    basePath_SSEC.cpp

    matrixRows_input.c
    get_check_data.c
    edges_input.c
    orders_input.c
    vehicles_input.c

    arrays_input.c
    time_msg.c
    pgr_assert.cpp
    identifier.cpp
    pgr_alloc.cpp
    )
