SET search_path TO 'example2', 'public';

CALL initial_solution();

DROP TABLE IF EXISTS tmp_vehicles;
SELECT *
INTO tmp_vehicles
FROM vehicles;

COMMIT;

SET client_min_messages TO NOTICE;

CALL vrp_optimize(
    $$SELECT * FROM shipments$$,
    $$SELECT * FROM tmp_vehicles$$,
    $$SELECT start_vid, end_vid, travel_time FROM timeMatrix$$,
    $$SELECT * FROM tdm('2019-12-13'::TIMESTAMP)$$,
    'tmp_vehicles',
    '2019-12-13 00:00:00',  factor => 1::FLOAT, max_cycles => 1
);

BEGIN;

/** no changes */
SELECT count(*) = 144
FROM vehicles v
LEFT JOIN tmp_vehicles t
USING (s_tw_open, id)
WHERE date_trunc('day', s_tw_open) < '2019-12-13 00:00:00';

/** changes */
SELECT id, s_tw_open, v.stops, t.stops
FROM vehicles v
LEFT JOIN tmp_vehicles t
USING (s_tw_open, id)
WHERE date_trunc('day', s_tw_open) >= '2019-12-13 00:00:00'
ORDER BY s_tw_open, id;
