SET search_path TO 'example2', 'public';

UPDATE vehicles SET stops = NULL;

DROP TABLE IF EXISTS withtimes;

SELECT *, id_to_geom(stop_id) AS location
INTO withtimes
FROM vrp_pickDeliver(
  $$
    SELECT id, amount, p_id, p_tw_open, p_tw_close, p_t_service, d_id, d_tw_open, d_tw_close, d_t_service
    FROM shipments
    WHERE date_trunc('day', p_tw_open) = '2019-12-09 00:00:00'
  $$,
  $$
    SELECT id, capacity, s_id, s_tw_open, s_tw_close, s_t_service, e_t_service
    FROM vehicles
    WHERE date_trunc('day', s_tw_open)     = '2019-12-09 00:00:00'
  $$,
  $$SELECT start_vid, end_vid, travel_time FROM timeMatrix$$,
  $$SELECT * FROM tdm('2019-12-09'::TIMESTAMP)$$,
  '2019-12-09 00:00:00'::TIMESTAMP,
  optimize => true, factor => 1::float, stop_on_all_served => true, max_cycles => 1);

SELECT *
FROM withtimes;

UPDATE shipments
SET p_t_service = '00:01:00',
    d_t_service = '00:02:00';

SELECT *, id_to_geom(stop_id) AS location
FROM vrp_pickDeliver(
  $$
    SELECT id, amount, p_id, p_tw_open, p_tw_close, p_t_service, d_id, d_tw_open, d_tw_close, d_t_service
    FROM shipments
    WHERE date_trunc('day', p_tw_open) = '2019-12-09 00:00:00'
  $$,
  $$
    SELECT id, capacity, s_id, s_tw_open, s_tw_close, s_t_service, e_t_service
    FROM vehicles
    WHERE date_trunc('day', s_tw_open)     = '2019-12-09 00:00:00'
  $$,
  $$SELECT start_vid, end_vid, travel_time FROM timeMatrix$$,
  $$SELECT * FROM tdm('2019-12-09'::TIMESTAMP)$$,
  '2019-12-09 00:00:00'::TIMESTAMP,
  optimize => true, factor => 1::float, stop_on_all_served => true, max_cycles => 1);
