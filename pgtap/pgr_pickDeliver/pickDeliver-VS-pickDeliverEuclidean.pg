
BEGIN;

UPDATE edge_table SET cost = sign(cost), reverse_cost = sign(reverse_cost);
SELECT plan(1);
SET client_min_messages TO ERROR;

PREPARE pd AS
SELECT seq, vehicle_seq, vehicle_id, stop_seq, stop_type,
    order_id, cargo,
    travel_time, arrival_time, wait_time, service_time, departure_time
FROM _vrp_pgr_pickDeliver(
    'SELECT * FROM orders_1 ORDER BY id',
    'SELECT * FROM vehicles_1',
    -- matrix query
    'WITH
    A AS (
        SELECT p_id AS id, p_x AS x, p_y AS y FROM orders_1
        UNION
        SELECT d_id AS id, d_x, d_y FROM orders_1
        UNION
        SELECT s_id, s_x, s_y FROM vehicles_1
    )
    SELECT A.id AS start_vid, B.id AS end_vid, sqrt( (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y))::INTEGER AS agg_cost
    FROM A, A AS B WHERE A.id != B.id'
    );

PREPARE pd_e AS
SELECT * FROM _vrp_pgr_pickDeliverEuclidean(
    'SELECT * FROM orders_1 ORDER BY id',
    'SELECT * FROM vehicles_1');

SELECT todo_start('Need to check why results are changing');
SELECT set_eq('pd', 'pd_e');
SELECT todo_end();


SELECT finish();
ROLLBACK;
