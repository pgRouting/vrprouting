SET search_path TO 'example2', 'public';

UPDATE vehicles SET stops = ARRAY[20,20];

SELECT *
FROM vrp_compatibleVehicles(
  $$
    SELECT id, amount, p_id, p_tw_open, p_tw_close, p_t_service, d_id, d_tw_open, d_tw_close, d_t_service
    FROM shipments
  $$,
  $$
    SELECT id, capacity, s_id, s_tw_open, s_tw_close, s_t_service, e_t_service
    FROM vehicles
  $$,
  $$SELECT start_vid, end_vid, travel_time FROM timeMatrix$$,
  $$SELECT * FROM tdm('2019-12-09'::TIMESTAMP, '2019-12-13'::TIMESTAMP)$$,
  20, factor => 1);
