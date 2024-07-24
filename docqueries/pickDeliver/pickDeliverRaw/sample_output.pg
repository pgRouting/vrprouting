SET search_path TO 'example2', 'public';

UPDATE vehicles
    SET stops = NULL;

SELECT *
FROM vrp_pickDeliverRaw(
    $$
      SELECT id, amount, p_id, p_open, p_close, p_service, d_id, d_open, d_close, d_service
      FROM shipments
      WHERE date_trunc('day', p_tw_open) = '2019-12-09 00:00:00'
    $$,
    $$
      SELECT id, capacity, stops, s_id, s_open, s_close, s_service, e_id, e_open, e_close, e_service
      FROM vehicles
      WHERE date_trunc('day', s_tw_open) = '2019-12-09 00:00:00'
    $$,
    $$SELECT start_vid, end_vid, agg_cost FROM timeMatrix$$,
    $$SELECT * FROM tdm_raw('2019-12-09'::TIMESTAMP, '2019-12-13'::TIMESTAMP)$$,
    optimize => true, factor => 1::float, max_cycles => 1, stop_on_all_served => true);

UPDATE shipments
SET p_t_service = '00:01:00'::INTERVAL
WHERE id < 50;

UPDATE shipments
SET d_t_service = '00:02:00'::INTERVAL
WHERE id > 50;

SELECT *
FROM vrp_pickDeliverRaw(
    $$
      SELECT id, amount, p_id, p_open, p_close, p_service, d_id, d_open, d_close, d_service
      FROM shipments
      WHERE date_trunc('day', p_tw_open) = '2019-12-09 00:00:00'
    $$,
    $$
      SELECT id, capacity, stops, s_id, s_open, s_close, s_service, e_id, e_open, e_close, e_service
      FROM vehicles
      WHERE date_trunc('day', s_tw_open) = '2019-12-09 00:00:00'
    $$,
    $$SELECT start_vid, end_vid, agg_cost FROM timeMatrix$$,
    $$SELECT * FROM tdm_raw('2019-12-09'::TIMESTAMP, '2019-12-13'::TIMESTAMP)$$,
    optimize => true, factor => 1::float, max_cycles => 1, stop_on_all_served => true);
