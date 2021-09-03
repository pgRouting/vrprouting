SET extra_float_digits=-3;
/* --q1 */
SELECT * FROM vrp_pgr_pickDeliver(
    'SELECT * FROM orders_1 ORDER BY id',
    'SELECT * from vehicles_1',
    -- matrix query
    'WITH
    A AS (
        SELECT p_id AS id, p_x AS x, p_y AS y FROM orders_1
        UNION
        SELECT d_id AS id, d_x, d_y FROM orders_1
        UNION
        SELECT s_id, s_x, s_y FROM vehicles_1
    )
    SELECT A.id AS start_vid, B.id AS end_vid, (sqrt( (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y)))::INTEGER AS agg_cost
    FROM A, A AS B WHERE A.id != B.id'
    );

/* --q2 */

SELECT * FROM vrp_pgr_pickDeliver(
    $$ SELECT * FROM orders_1 ORDER BY id $$,
    $$ SELECT * FROM vehicles_1 ORDER BY id$$,
    $$ SELECT start_vid, end_vid, agg_cost::INTEGER  FROM pgr_dijkstraCostMatrix(
        'SELECT * FROM edge_table ',
        (SELECT array_agg(id) FROM (SELECT p_id AS id FROM orders_1
        UNION
        SELECT d_id FROM orders_1
        UNION
        SELECT s_id FROM vehicles_1) a))
    $$
);

/* --q3 */
