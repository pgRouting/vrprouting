BEGIN;

UPDATE edge_table SET cost = sign(cost), reverse_cost = sign(reverse_cost);
SELECT plan(1);
SET client_min_messages TO ERROR;


PREPARE missing_id_on_matrix AS
SELECT * FROM vrp_pgr_pickDeliver(
    $$ SELECT * FROM orders_1 ORDER BY id $$,
    $$ SELECT * FROM vehicles_1 $$,
    $$ SELECT * FROM edges_matrix WHERE start_vid IN (3, 4, 5, 8, 9, 11) AND end_vid IN (3, 4, 5, 8, 9, 11) $$
);

SELECT throws_ok('missing_id_on_matrix', 'XX000', 'An Infinity value was found on the Matrix. Might be missing information of a node', 'Should throw: matrix is missing node 6');


SELECT finish();
ROLLBACK;
