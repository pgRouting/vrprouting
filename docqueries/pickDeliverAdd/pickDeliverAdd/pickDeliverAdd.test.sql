SET search_path TO 'example2', 'public';

UPDATE vehicles
    SET stops = NULL;


SELECT id
FROM shipments
WHERE '2019-12-09 14:00:00' <= p_tw_open AND d_tw_close <= '2019-12-09 15:55:00'
ORDER BY id;

-- 20 50 67 93 97 105 205 219 229 406 407 425 429 432 472 476 503 505 506 507 508
-- 425 425 505 507 508 505 507 508 93 219 219 205 205 93

SELECT id, stops
FROM vehicles
WHERE id IN (585);

-- adding order 507 to vehicle 585
SELECT *
FROM vrp_pickDeliverAdd(
  $$
    SELECT id, amount, p_id, p_tw_open, p_tw_close, p_t_service, d_id, d_tw_open, d_tw_close, d_t_service
    FROM shipments
  $$,
  $$
    SELECT id, capacity, s_id, s_tw_open, s_tw_close, s_t_service, e_t_service, stops
    FROM vehicles WHERE id = 585
  $$,
  $$SELECT start_vid, end_vid, travel_time FROM timeMatrix$$,
  $$SELECT * FROM tdm('2019-12-09'::TIMESTAMP, '2019-12-13'::TIMESTAMP)$$::TEXT,
  507, factor => 1.0::float);

UPDATE vehicles SET stops = ARRAY[507, 507]
WHERE id = 585;

-- adding order 205 to vehicle 585
SELECT *
FROM vrp_pickDeliverAdd(
  $$SELECT * FROM shipments$$::TEXT,
  $$SELECT * FROM vehicles WHERE id = 585$$::TEXT,
  $$SELECT start_vid, end_vid, travel_time FROM timeMatrix$$,
  $$SELECT * FROM tdm('2019-12-09'::TIMESTAMP, '2019-12-13'::TIMESTAMP)$$::TEXT,
  205, factor => 1.0::float);

UPDATE vehicles SET stops = ARRAY[507, 507, 205, 205]
WHERE id = 585;

-- adding order 508 to vehicle 585
SELECT *
FROM vrp_pickDeliverAdd(
  $$SELECT * FROM shipments$$::TEXT,
  $$SELECT * FROM vehicles WHERE id = 585$$::TEXT,
  $$SELECT start_vid, end_vid, travel_time FROM timeMatrix$$,
  $$SELECT * FROM tdm('2019-12-09'::TIMESTAMP, '2019-12-13'::TIMESTAMP)$$::TEXT,
  508, factor => 1.0::float);

UPDATE vehicles SET stops = ARRAY[507, 508, 507, 508, 205, 205]
WHERE id = 585;

-- adding order 20 to vehicle 585
SELECT *
FROM vrp_pickDeliverAdd(
  $$SELECT * FROM shipments$$::TEXT,
  $$SELECT * FROM vehicles WHERE id = 585$$::TEXT,
  $$SELECT start_vid, end_vid, travel_time FROM timeMatrix$$,
  $$SELECT * FROM tdm('2019-12-09'::TIMESTAMP, '2019-12-13'::TIMESTAMP)$$::TEXT,
  20, factor => 1.0::float);

-- but order 20 fits in another vehicle
SELECT *
FROM vrp_pickDeliverAdd(
  $$SELECT * FROM shipments$$::TEXT,
  $$SELECT * FROM vehicles$$::TEXT,
  $$SELECT start_vid, end_vid, travel_time FROM timeMatrix$$,
  $$SELECT * FROM tdm('2019-12-09'::TIMESTAMP, '2019-12-13'::TIMESTAMP)$$::TEXT,
  20, factor => 1.0::float);
