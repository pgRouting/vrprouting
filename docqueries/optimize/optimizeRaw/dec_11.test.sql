SET search_path TO 'example2', 'public';

DROP TABLE IF EXISTS tmp_vehicles;
SELECT *
INTO tmp_vehicles
FROM vehicles;

DROP TABLE IF EXISTS __results;
SELECT vehicle_id AS id, array_agg(order_id ORDER BY seq) AS stops
INTO __results
FROM vrp_optimizeRaw(
    $$SELECT id, amount, p_id, p_open, p_close, p_service, d_id, d_open, d_close, d_service FROM shipments$$,
    $$SELECT id, capacity, stops, s_id, s_open, s_close, s_service, e_id, e_open, e_close, e_service FROM tmp_vehicles$$,
    $$SELECT start_vid, end_vid, agg_cost FROM timeMatrix$$,
    $$SELECT * FROM tdm_raw('2019-12-11'::TIMESTAMP, '2019-12-13'::TIMESTAMP)$$,
    'tmp_vehicles',
    execution_date => EXTRACT(EPOCH FROM ('2019-12-11 00:00:00'::TIMESTAMP))::BIGINT,
    factor => 1::FLOAT, max_cycles => 1,
    check_triangle_inequality => true,
    subdivision_kind => 1
) GROUP BY vehicle_id;

SELECT id, s_tw_open, v.stops, t.stops
FROM vehicles v
LEFT JOIN tmp_vehicles t
USING (s_tw_open, id)
WHERE date_trunc('day', s_tw_open) >= '2019-12-11 00:00:00'
ORDER BY s_tw_open, id;
);
