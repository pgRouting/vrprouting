SET search_path TO 'example2', 'public';

COMMIT;

CALL vrp_simulation(
    $$SELECT * FROM shipments$$,
    $$SELECT * FROM vehicles$$,
    $$SELECT start_vid, end_vid, travel_time FROM timeMatrix$$,
    $$SELECT * FROM tdm('2019-12-09'::TIMESTAMP, '2019-12-13'::TIMESTAMP)$$,

    1.0, -- factor
    1, -- max_cycles when adding an order
    10, -- max_cycles when response time is big or day changes

    '2019-12-02 00:00:00'::TIMESTAMP, -- start simulation from this "booking date"

    print => 1000, -- print vehicle stops every 1000 bookings
    optimize => true -- turn optimization on
);

BEGIN;
SELECT id, stops FROM sim_vehicles_tbl ORDER BY id;
