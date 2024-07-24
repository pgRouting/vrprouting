SET search_path TO 'example2', 'public';

UPDATE vehicles SET stops = NULL;


DROP TABLE IF EXISTS withfloats;
SELECT *
INTO withfloats
FROM vrp_pickDeliverRaw(
    $$SELECT * FROM shipments WHERE date_trunc('day', p_tw_open) = '2019-12-13 00:00:00'$$,
    $$SELECT * FROM vehicles  WHERE date_trunc('day', s_tw_open) = '2019-12-13 00:00:00'$$,
    $$SELECT start_vid, end_vid, agg_cost FROM timeMatrix$$,
    $$SELECT * FROM tdm_raw('2019-12-13'::TIMESTAMP)$$,
    execution_date => EXTRACT(EPOCH FROM('2019-12-13 00:00:00'::TIMESTAMP))::BIGINT,
    optimize => true, factor => 1::float, stop_on_all_served => true, max_cycles => 1);

DROP TABLE IF EXISTS withtimes;
SELECT *
INTO withtimes
FROM vrp_pickDeliver(
    $$SELECT * FROM shipments WHERE date_trunc('day', p_tw_open) = '2019-12-13 00:00:00'$$,
    $$SELECT * FROM vehicles  WHERE date_trunc('day', s_tw_open) = '2019-12-13 00:00:00'$$,
    $$SELECT start_vid, end_vid, travel_time FROM timeMatrix$$,
    $$SELECT * FROM tdm('2019-12-13'::TIMESTAMP)$$,
    execution_date => '2019-12-13 00:00:00'::TIMESTAMP,
    optimize => true, factor => 1::float, stop_on_all_served => true, max_cycles => 1);

\dS withfloats
\dS withtimes

SELECT 'same result number of rows', count(*) = (SELECT count(*) FROM withtimes)
FROM withfloats;

SELECT 'rows match', count(*) = (SELECT count(*) FROM withtimes)
FROM withtimes t JOIN withfloats f USING (seq, stop_seq, vehicle_seq, vehicle_id)
WHERE (f.order_id = t.shipment_id);

/* create stops for testing no optimize */
WITH
new_stops AS (
  SELECT vehicle_id AS id, array_remove(array_agg(shipment_id ORDER BY seq)::BIGINT[],-1::BIGINT) AS stops
  FROM vrp_pickDeliver(
    $$SELECT * FROM shipments            WHERE date_trunc('day', p_tw_open) = '2019-12-13 00:00:00'$$,
    $$SELECT * FROM vehicles WHERE date_trunc('day', s_tw_open)     = '2019-12-13 00:00:00'$$,
    $$SELECT start_vid, end_vid, travel_time FROM timeMatrix$$,
    $$SELECT * FROM tdm('2019-12-13'::TIMESTAMP)$$,
    execution_date => '2019-12-13 00:00:00'::TIMESTAMP,
    optimize => true, factor => 1::float, max_cycles => 1, stop_on_all_served => true)
  GROUP BY vehicle_id
)
UPDATE vehicles AS v
SET stops = n.stops
FROM new_stops AS n WHERE v.id = n.id;


/* dont no optimize using vrp_pickDeliver */
DROP TABLE IF EXISTS no_changes;
SELECT *
INTO no_changes
FROM vrp_pickDeliver(
    $$SELECT * FROM shipments            WHERE date_trunc('day', p_tw_open) = '2019-12-13 00:00:00'$$,
    $$SELECT * FROM vehicles WHERE date_trunc('day', s_tw_open)     = '2019-12-13 00:00:00'$$,
    $$SELECT start_vid, end_vid, travel_time FROM timeMatrix$$,
    $$SELECT * FROM tdm('2019-12-13'::TIMESTAMP)$$,
    execution_date => '2019-12-13 00:00:00'::TIMESTAMP,
    optimize => false, factor => 1::float, max_cycles => 1, stop_on_all_served => true);



WITH
new_stops AS (
    SELECT vehicle_id AS id, array_remove(array_agg(shipment_id ORDER BY seq)::BIGINT[],-1::BIGINT) AS new_stops
    FROM no_changes
    GROUP BY id
),
original_stops AS (
    SELECT id, stops::BIGINT[]
    FROM vehicles WHERE date_trunc('day', s_tw_open) = '2019-12-13 00:00:00'
)
SELECT count(*)
FROM original_stops
LEFT JOIN new_stops USING(id)
WHERE stops = new_stops;

/* optimize using vrp_pickDeliver */
SELECT *
INTO the_stops
FROM vrp_pickDeliver(
    $$SELECT * FROM shipments            WHERE date_trunc('day', p_tw_open) = '2019-12-13 00:00:00'$$,
    $$SELECT * FROM vehicles WHERE date_trunc('day', s_tw_open)     = '2019-12-13 00:00:00'$$,
    $$SELECT start_vid, end_vid, travel_time FROM timeMatrix$$,
    $$SELECT * FROM tdm('2019-12-13'::TIMESTAMP)$$,
    execution_date => '2019-12-13 00:00:00'::TIMESTAMP,
    optimize => true, factor => 1::float, max_cycles => 1,  stop_on_all_served => false);

/* save stops after optimize */
WITH
b AS (
    SELECT vehicle_id AS id, array_remove(array_agg(shipment_id ORDER BY seq)::BIGINT[],-1::BIGINT) AS stops
    FROM the_stops
    GROUP BY vehicle_id
)
UPDATE vehicles AS v
SET stops = b.stops
FROM b WHERE v.id = b.id;
