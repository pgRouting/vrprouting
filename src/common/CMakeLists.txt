ADD_LIBRARY(common OBJECT
    postgres_connection.c
    get_check_data.c

    matrixRows_input.c
    arrays_input.c
    orders_input.c
    vehicles_input.c
    time_multipliers_input.c

    time_msg.c
    e_report.c
    pgr_alloc.cpp

    vroom/time_windows_input.c
    vroom/breaks_input.c
    vroom/jobs_input.c
    vroom/shipments_input.c
    vroom/vehicles_input.c
    )
