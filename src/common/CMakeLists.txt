ADD_LIBRARY(common OBJECT
    postgres_connection.c
    e_report.c

    matrixRows_input.c
    get_check_data.c
    orders_input.c
    vehicles_input.c

    arrays_input.c
    time_msg.c
    pgr_alloc.cpp
    )
