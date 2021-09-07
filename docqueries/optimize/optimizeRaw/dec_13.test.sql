SET search_path TO 'example2', 'public';

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

SELECT *
FROM vrp_optimizeRaw(
    $$SELECT id, amount, p_id, p_open, p_close, p_service, d_id, d_open, d_close, d_service FROM shipments$$,
    $$SELECT id, capacity, stops, s_id, s_open, s_close, s_service, e_id, e_open, e_close, e_service FROM tmp_vehicles$$,
    $$SELECT start_vid, end_vid, agg_cost FROM timeMatrix$$,
    $$SELECT * FROM tdm_raw('2019-12-13'::TIMESTAMP)$$,
    execution_date => EXTRACT(EPOCH FROM ('2019-12-13 00:00:00'::TIMESTAMP))::BIGINT,
    factor => 1::FLOAT, max_cycles => 3,
    check_triangle_inequality => true,
    subdivision_kind => 0
);

SELECT *
FROM vrp_optimizeRaw(
    $$SELECT id, amount, p_id, p_open, p_close, p_service, d_id, d_open, d_close, d_service FROM shipments$$,
    $$SELECT id, capacity, stops, s_id, s_open, s_close, s_service, e_id, e_open, e_close, e_service FROM tmp_vehicles1$$,
    $$SELECT start_vid, end_vid, agg_cost FROM timeMatrix$$,
    $$SELECT * FROM tdm_raw('2019-12-13'::TIMESTAMP)$$,
    execution_date => EXTRACT(EPOCH FROM ('2019-12-13 00:00:00'::TIMESTAMP))::BIGINT,
    factor => 1::FLOAT, max_cycles => 3,
    check_triangle_inequality => false,
    subdivision_kind => 1
);

SELECT *
FROM vrp_optimizeRaw(
    $$SELECT id, amount, p_id, p_open, p_close, p_service, d_id, d_open, d_close, d_service FROM shipments$$,
    $$SELECT id, capacity, stops, s_id, s_open, s_close, s_service, e_id, e_open, e_close, e_service FROM tmp_vehicles2$$,
    $$SELECT start_vid, end_vid, agg_cost FROM timeMatrix$$,
    $$SELECT * FROM tdm_raw('2019-12-13'::TIMESTAMP)$$,
    execution_date => EXTRACT(EPOCH FROM ('2019-12-13 00:00:00'::TIMESTAMP))::BIGINT,
    factor => 1::FLOAT, max_cycles => 3,
    check_triangle_inequality => false,
    subdivision_kind => 2
);

