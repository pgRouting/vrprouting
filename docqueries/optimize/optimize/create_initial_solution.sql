SET search_path TO 'example2', 'public';

UPDATE vehicles SET stops = NULL;

/*******
Creating an initial solution
*******/
WITH
new_stops AS (
    SELECT vehicle_id AS id, array_remove(array_agg(order_id ORDER BY seq)::BIGINT[],-1::BIGINT) AS stops
    FROM vrp_pickDeliverRaw(
        $$SELECT * FROM shipments            WHERE date_trunc('day', p_tw_open) = '2019-12-09 00:00:00' ORDER BY id$$,
        $$SELECT * FROM vehicles WHERE date_trunc('day', s_tw_open)     = '2019-12-09 00:00:00' ORDER BY id$$,
        $$SELECT start_vid, end_vid, agg_cost FROM timeMatrix$$,
        $$SELECT * FROM tdm_raw('2019-12-09'::TIMESTAMP)$$,
        optimize => true, factor => 1::float, max_cycles => 1, stop_on_all_served => true)
      GROUP BY vehicle_id
    )
UPDATE vehicles AS v
SET stops = n.stops
FROM new_stops AS n WHERE v.id = n.id;

WITH
new_stops AS (
    SELECT vehicle_id AS id, array_remove(array_agg(order_id ORDER BY seq)::BIGINT[],-1::BIGINT) AS stops
    FROM vrp_pickDeliverRaw(
        $$SELECT * FROM shipments            WHERE date_trunc('day', p_tw_open) = '2019-12-10 00:00:00' ORDER BY id$$,
        $$SELECT * FROM vehicles WHERE date_trunc('day', s_tw_open)     = '2019-12-10 00:00:00' ORDER BY id$$,
        $$SELECT start_vid, end_vid, agg_cost FROM timeMatrix$$,
        $$SELECT * FROM tdm_raw('2019-12-10'::TIMESTAMP)$$,
        optimize => true, factor => 1::float, max_cycles => 1, stop_on_all_served => true)
    GROUP BY vehicle_id
)
UPDATE vehicles AS v
SET stops = n.stops
FROM new_stops AS n WHERE v.id = n.id;

WITH
new_stops AS (
    SELECT vehicle_id AS id, array_remove(array_agg(order_id ORDER BY seq)::BIGINT[],-1::BIGINT) AS stops
    FROM vrp_pickDeliverRaw(
        $$SELECT * FROM shipments            WHERE date_trunc('day', p_tw_open) = '2019-12-11 00:00:00' ORDER BY id$$,
        $$SELECT * FROM vehicles WHERE date_trunc('day', s_tw_open)     = '2019-12-11 00:00:00' ORDER BY id$$,
        $$SELECT start_vid, end_vid, agg_cost FROM timeMatrix$$,
        $$SELECT * FROM tdm_raw('2019-12-11'::TIMESTAMP)$$,
        optimize => true, factor => 1::float, max_cycles => 1, stop_on_all_served => true)
    GROUP BY vehicle_id
)
UPDATE vehicles AS v
SET stops = n.stops
FROM new_stops AS n WHERE v.id = n.id;

WITH
new_stops AS (
    SELECT vehicle_id AS id, array_remove(array_agg(order_id ORDER BY seq)::BIGINT[],-1::BIGINT) AS stops
    FROM vrp_pickDeliverRaw(
        $$SELECT * FROM shipments            WHERE date_trunc('day', p_tw_open) = '2019-12-12 00:00:00' ORDER BY id$$,
        $$SELECT * FROM vehicles WHERE date_trunc('day', s_tw_open)     = '2019-12-12 00:00:00' ORDER BY id$$,
        $$SELECT start_vid, end_vid, agg_cost FROM timeMatrix$$,
        $$SELECT * FROM tdm_raw('2019-12-12'::TIMESTAMP)$$,
        optimize => true, factor => 1::float, max_cycles => 1, stop_on_all_served => true)
    GROUP BY vehicle_id
)
UPDATE vehicles AS v
SET stops = n.stops
FROM new_stops AS n WHERE v.id = n.id;

WITH
new_stops AS (
    SELECT vehicle_id AS id, array_remove(array_agg(order_id ORDER BY seq)::BIGINT[],-1::BIGINT) AS stops
    FROM vrp_pickDeliverRaw(
        $$SELECT * FROM shipments            WHERE date_trunc('day', p_tw_open) = '2019-12-13 00:00:00' ORDER BY id$$,
        $$SELECT * FROM vehicles WHERE date_trunc('day', s_tw_open)     = '2019-12-13 00:00:00' ORDER BY id$$,
        $$SELECT start_vid, end_vid, agg_cost FROM timeMatrix$$,
        $$SELECT * FROM tdm_raw('2019-12-13'::TIMESTAMP)$$,
        optimize => true, factor => 1::float, max_cycles => 1, stop_on_all_served => true)
    GROUP BY vehicle_id
)
UPDATE vehicles AS v
SET stops = n.stops
FROM new_stops AS n WHERE v.id = n.id;

