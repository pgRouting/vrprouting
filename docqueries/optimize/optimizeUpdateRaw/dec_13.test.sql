SET search_path TO 'example2', 'public';

CALL initial_solution();

DROP TABLE IF EXISTS tmp_vehicles;
SELECT *
INTO tmp_vehicles
FROM vehicles;

DROP TABLE IF EXISTS tmp_vehicles1;
SELECT *
INTO tmp_vehicles1
FROM vehicles;

DROP TABLE IF EXISTS tmp_vehicles2;
SELECT *
INTO tmp_vehicles2
FROM vehicles;

COMMIT;

SET client_min_messages TO NOTICE;

CALL vrp_optimizeUpdateRaw(
    $$SELECT id, amount, p_id, p_open, p_close, p_service, d_id, d_open, d_close, d_service FROM shipments$$,
    $$SELECT id, capacity, stops, s_id, s_open, s_close, s_service, e_id, e_open, e_close, e_service FROM tmp_vehicles$$,
    $$SELECT start_vid, end_vid, agg_cost FROM timeMatrix$$,
    $$SELECT * FROM tdm_raw('2019-12-13'::TIMESTAMP)$$,
    'tmp_vehicles',
    execution_date => EXTRACT(EPOCH FROM ('2019-12-13 00:00:00'::TIMESTAMP))::BIGINT,
    factor => 1::FLOAT, max_cycles => 3,
    check_triangle_inequality => true,
    subdivision_kind => 0
);

CALL vrp_optimizeUpdateRaw(
    $$SELECT id, amount, p_id, p_open, p_close, p_service, d_id, d_open, d_close, d_service FROM shipments$$,
    $$SELECT id, capacity, stops, s_id, s_open, s_close, s_service, e_id, e_open, e_close, e_service FROM tmp_vehicles1$$,
    $$SELECT start_vid, end_vid, agg_cost FROM timeMatrix$$,
    $$SELECT * FROM tdm_raw('2019-12-13'::TIMESTAMP)$$,
    'tmp_vehicles1',
    execution_date => EXTRACT(EPOCH FROM ('2019-12-13 00:00:00'::TIMESTAMP))::BIGINT,
    factor => 1::FLOAT, max_cycles => 3,
    check_triangle_inequality => false,
    subdivision_kind => 1
);

CALL vrp_optimizeUpdateRaw(
    $$SELECT id, amount, p_id, p_open, p_close, p_service, d_id, d_open, d_close, d_service FROM shipments$$,
    $$SELECT id, capacity, stops, s_id, s_open, s_close, s_service, e_id, e_open, e_close, e_service FROM tmp_vehicles2$$,
    $$SELECT start_vid, end_vid, agg_cost FROM timeMatrix$$,
    $$SELECT * FROM tdm_raw('2019-12-13'::TIMESTAMP)$$,
    'tmp_vehicles2',
    execution_date => EXTRACT(EPOCH FROM ('2019-12-13 00:00:00'::TIMESTAMP))::BIGINT,
    factor => 1::FLOAT, max_cycles => 3,
    check_triangle_inequality => false,
    subdivision_kind => 2
);

BEGIN;

SELECT id, s_tw_open, v.stops, t.stops
FROM vehicles v
LEFT JOIN tmp_vehicles t
USING (s_tw_open, id)
WHERE date_trunc('day', s_tw_open) >= '2019-12-13 00:00:00'
ORDER BY s_tw_open, id;

SELECT id, s_tw_open, v.stops, t.stops
FROM vehicles v
LEFT JOIN tmp_vehicles1 t
USING (s_tw_open, id)
WHERE date_trunc('day', s_tw_open) >= '2019-12-13 00:00:00'
ORDER BY s_tw_open, id;

SELECT id, s_tw_open, v.stops, t.stops
FROM vehicles v
LEFT JOIN tmp_vehicles2 t
USING (s_tw_open, id)
WHERE date_trunc('day', s_tw_open) >= '2019-12-13 00:00:00'
ORDER BY s_tw_open, id;
