SET search_path TO 'example2', 'public';

UPDATE vehicles
    SET stops = NULL;

DROP TABLE IF EXISTS tmp_results;
SELECT * INTO tmp_results
FROM vrp_pickDeliverRaw(
    $$SELECT * FROM shipments            WHERE date_trunc('day', p_tw_open) = '2019-12-13 00:00:00'$$,
    $$SELECT * FROM vehicles WHERE date_trunc('day', s_tw_open)     = '2019-12-13 00:00:00'$$,
    $$SELECT start_vid, end_vid, agg_cost FROM timeMatrix$$,
    $$SELECT * FROM tdm_raw('2019-12-13'::TIMESTAMP)$$,
    optimize => true, factor => 1::float, max_cycles => 1, stop_on_all_served => true);


DROP TABLE IF EXISTS tmp_shipments;
SELECT *
INTO tmp_shipments
FROM shipments WHERE date_trunc('day', p_tw_open) = '2019-12-13 00:00:00';

DROP TABLE IF EXISTS check1;
WITH
pickups AS (
    SELECT *
    FROM tmp_results WHERE stop_type = 2
),
dropoffs AS (
    SELECT *
    FROM tmp_results WHERE stop_type = 3
)
SELECT
    order_id, vehicle_id, vehicle_seq,

    p.stop_id AS p_vid, p.seq AS p_seq, p.stop_seq AS p_stop_seq,
    p_open, p.arrival_ft AS p_arrival, p.arrival_ft + p.wait_fd AS p_schedule, p.schedule_ft AS p_schedule_ft, p.departure_ft AS p_departure, p_close,

    d.stop_id AS d_vid, d.seq AS d_seq, d.stop_seq AS d_stop_seq,
    d_open, d.arrival_ft AS d_arrival, d.arrival_ft + d.wait_fd AS d_schedule, d.schedule_ft AS d_schedule_ft, d.departure_ft AS d_departure, d_close
INTO check1
FROM
    pickups AS p
    JOIN dropoffs AS d USING (order_id, vehicle_id, vehicle_seq)
    JOIN tmp_shipments o ON (order_id = id);

SELECT 'calculated schedule is the same as schedule given', count(*)=0
FROM check1
WHERE p_schedule != p_schedule_ft AND  p_schedule + 1 != p_schedule_ft AND  p_schedule - 1 != p_schedule_ft;

SELECT 'number of shipments is correct', count(*) = (SELECT count(*) FROM tmp_shipments) AS OK
FROM shipments
WHERE date_trunc('day', p_tw_open) = '2019-12-13 00:00:00';

SELECT
    'number of shipments on the result is correct',
    count(*) = (SELECT count(*) FROM tmp_shipments) AS OK
FROM check1;

WITH
counts AS (
    SELECT order_id, count(*)
    FROM tmp_results
    WHERE order_id != -1
    GROUP BY order_id
)
SELECT
    'each order appears twice on the results',
    count(*) = (SELECT count(*) FROM tmp_shipments) AS OK
FROM counts
WHERE count = 2;


SELECT
    'pick sequence comes before drop sequence',
    count(*) = (SELECT count(*) FROM tmp_shipments) AS OK
FROM check1
WHERE p_seq < d_seq AND p_stop_seq < d_stop_seq;

SELECT
    'arrival IS before departure',
    count(*) = (SELECT count(*) FROM tmp_shipments) AS OK
FROM check1
WHERE p_arrival <= p_departure AND d_arrival <= d_departure;

SELECT
    'pickup departure before dropoff arrival',
    count(*) = (SELECT count(*) FROM tmp_shipments) AS OK
FROM check1
WHERE p_departure <= d_arrival;

SELECT
    'Correct pickup TW',
    count(*) = (SELECT count(*) FROM tmp_shipments) AS OK
FROM check1
WHERE p_open <= p_schedule_ft AND p_schedule_ft <= p_close;

SELECT
    'Correct dropoff TW',
    count(*) = (SELECT count(*) FROM tmp_shipments) AS OK
FROM check1
WHERE d_open <= d_schedule_ft AND d_schedule_ft <= d_close;


SELECT count(*) AS in_extra, 'In phoney vehicle' AS msg
FROM check1
WHERE vehicle_id = -1;

