
BEGIN;

UPDATE edge_table SET cost = sign(cost), reverse_cost = sign(reverse_cost);
SELECT plan(25);

PREPARE q1 AS
SELECT * FROM _vrp_pgr_pickDeliverEuclidean(
    $$SELECT * FROM orders_1$$,
    $$SELECT * FROM vehicles_1$$,
    max_cycles := 30);


SELECT lives_ok('q1', 'Original query should not fail');

--------------------------------------
-- testing wrong data on max_cycles
--------------------------------------
PREPARE q6 AS
SELECT * FROM _vrp_pgr_pickDeliverEuclidean(
    $$SELECT * FROM orders_1$$,
    $$SELECT * FROM vehicles_1$$,
    max_cycles := -1);

PREPARE q61 AS
SELECT * FROM _vrp_pgr_pickDeliverEuclidean(
    $$SELECT * FROM orders_1$$,
    $$SELECT * FROM vehicles_1$$,
    max_cycles := 0);

SELECT throws_ok('q6',
    'XX000',
    'Illegal value in parameter: max_cycles',
    'Should throw: max_cycles < 0');

SELECT lives_ok('q61',
    'Should live: max_cycles == 0');


--------------------------------------
-- testing wrong data on initial_sol
--------------------------------------
PREPARE initsol1 AS
SELECT * FROM _vrp_pgr_pickDeliverEuclidean(
    $$SELECT * FROM orders_1$$,
    $$SELECT * FROM vehicles_1$$,
    initial_sol := -1);

PREPARE initsol2 AS
SELECT * FROM _vrp_pgr_pickDeliverEuclidean(
    $$SELECT * FROM orders_1$$,
    $$SELECT * FROM vehicles_1$$,
    initial_sol := 7);

PREPARE initsol3 AS
SELECT * FROM _vrp_pgr_pickDeliverEuclidean(
    $$SELECT * FROM orders_1$$,
    $$SELECT * FROM vehicles_1$$,
    initial_sol := 0);



SELECT throws_ok('initsol1',
    'XX000',
    'Illegal value in parameter: initial_sol',
    'Should throw: initial_sol < 0');

SELECT throws_ok('initsol2',
    'XX000',
    'Illegal value in parameter: initial_sol',
    'Should throw: initial_sol > 6');

SELECT throws_ok('initsol3',
    'XX000',
    'Illegal value in parameter: initial_sol',
    'Should throw: initial_sol = 0');

--------------------------------------
-- testing wrong data on factor
--------------------------------------
PREPARE factor1 AS
SELECT * FROM _vrp_pgr_pickDeliverEuclidean(
    $$SELECT * FROM orders_1$$,
    $$SELECT * FROM vehicles_1$$,
    factor := -1);

PREPARE factor2 AS
SELECT * FROM _vrp_pgr_pickDeliverEuclidean(
    $$SELECT * FROM orders_1$$,
    $$SELECT * FROM vehicles_1$$,
    factor := 0);

PREPARE factor3 AS
SELECT * FROM _vrp_pgr_pickDeliverEuclidean(
    $$SELECT * FROM orders_1$$,
    $$SELECT * FROM vehicles_1$$,
    factor := 1);

SELECT throws_ok('factor1',
    'XX000',
    'Illegal value in parameter: factor',
    'Should throw: factor < 0');

SELECT throws_ok('factor2',
    'XX000',
    'Illegal value in parameter: factor',
    'Should throw: factor = 0');

SELECT lives_ok('factor3',
    'Should live: factor >= 1');


PREPARE vehiles0 AS
SELECT * FROM _vrp_pgr_pickDeliverEuclidean(
    $$SELECT * FROM orders_1$$,
    $$SELECT id AS vid FROM vehicles_1$$);

PREPARE vehiles1 AS
SELECT * FROM _vrp_pgr_pickDeliverEuclidean(
    $$SELECT * FROM orders_1$$,
    $$SELECT id FROM vehicles_1$$);

PREPARE vehiles2 AS
SELECT * FROM _vrp_pgr_pickDeliverEuclidean(
    $$SELECT * FROM orders_1$$,
    $$SELECT id, capacity FROM vehicles_1$$);

PREPARE vehiles3 AS
SELECT * FROM _vrp_pgr_pickDeliverEuclidean(
    $$SELECT * FROM orders_1$$,
    $$SELECT id, capacity, s_x FROM vehicles_1$$);

PREPARE vehiles4 AS
SELECT * FROM _vrp_pgr_pickDeliverEuclidean(
    $$SELECT * FROM orders_1$$,
    $$SELECT id, capacity, s_x, s_y FROM vehicles_1$$);

SELECT throws_ok('vehiles0',
    'XX000',
    $$Column 'id' not Found$$,
    'Should throw: id is not included in data');

SELECT throws_ok('vehiles1',
    'XX000',
    $$Column 'capacity' not Found$$,
    'Should throw: capacity is not included in data');

SELECT throws_ok('vehiles2',
    'XX000',
    $$Column 's_x' not Found$$,
    'Should throw: s_x is not included in data');

SELECT throws_ok('vehiles3',
    'XX000',
    $$Column 's_y' not Found$$,
    'Should throw: s_y is not included in data');

SELECT lives_ok('vehiles4',
    'Should live: rest of parameters are optional');

-- e_open and e_close should exist together or not at all

PREPARE vehiles5 AS
SELECT * FROM _vrp_pgr_pickDeliverEuclidean(
    $$SELECT * FROM orders_1$$,
    $$SELECT *, 10 AS e_close FROM vehicles_1$$);

PREPARE vehiles6 AS
SELECT * FROM _vrp_pgr_pickDeliverEuclidean(
    $$SELECT * FROM orders_1$$,
    $$SELECT *, 10 AS e_open FROM vehicles_1$$);

-- e_x and e_y should exist together or not at all

PREPARE vehiles7 AS
SELECT * FROM _vrp_pgr_pickDeliverEuclidean(
    $$SELECT * FROM orders_1$$,
    $$SELECT *, 10 AS e_x FROM vehicles_1$$);

PREPARE vehiles8 AS
SELECT * FROM _vrp_pgr_pickDeliverEuclidean(
    $$SELECT * FROM orders_1$$,
    $$SELECT *, 10 AS e_open FROM vehicles_1$$);

SELECT throws_ok('vehiles5',
    'XX000',
    $$Column 'e_open' not Found$$,
    'vehiles5, Should throw: e_close found, but not e_open');

SELECT throws_ok('vehiles6',
    'XX000',
    $$Column 'e_close' not Found$$,
    'vehiles6, Should throw: e_open found, but not e_close');

SELECT throws_ok('vehiles7',
    'XX000',
    $$Column 'e_y' not Found$$,
    'vehiles7, Should throw: e_x found, but not e_y');

SELECT throws_ok('vehiles8',
    'XX000',
    $$Column 'e_close' not Found$$,
    'vehiles8, Should throw: e_y found, but not e_x');

-- s_open and s_close should exist together or not at all

PREPARE no_s_open AS
SELECT * FROM _vrp_pgr_pickDeliverEuclidean(
    $$SELECT * FROM orders_1$$,
    $$SELECT id, s_x, s_y, s_close, capacity FROM vehicles_1$$);

PREPARE no_s_close AS
SELECT * FROM _vrp_pgr_pickDeliverEuclidean(
    $$SELECT * FROM orders_1$$,
    $$SELECT id, s_x, s_y, s_open, capacity FROM vehicles_1$$);

SELECT throws_ok('no_s_open',
    'XX000',
    $$Column 's_open' not Found$$,
    'no_s_open, Should throw: s_close found, but not s_open');

SELECT throws_ok('no_s_close',
    'XX000',
    $$Column 's_close' not Found$$,
    'no_s_close, Should throw: s_open found, but not s_close');
---------------------
--  s_open > s_close
---------------------
UPDATE vehicles_1 SET s_open = 5, s_close = 4 WHERE id = 1;

PREPARE vehicles9 AS
SELECT * FROM _vrp_pgr_pickDeliverEuclidean(
    $$SELECT * FROM orders_1$$,
    $$SELECT * FROM vehicles_1$$);

SELECT throws_ok('vehicles9',
    'XX000',
    'Illegal values found on vehicle',
    'vehicles9, Should throw: s_open > s_close');

UPDATE vehicles_1 SET s_open = 0, s_close = 50 WHERE id = 1;

---------------------
--  e_open > e_close
---------------------
PREPARE vehicles10 AS
SELECT * FROM _vrp_pgr_pickDeliverEuclidean(
    $$SELECT * FROM orders_1$$,
    $$SELECT *, 5 AS e_open, 4 AS e_close FROM vehicles_1$$);

SELECT throws_ok('vehicles10',
    'XX000',
    'Illegal values found on vehicle',
    'vehicles10, Should throw: e_open > e_close');

--------------------------------------
-- testing wrong data on orders
--------------------------------------

---------------------
--  d_open > d_close
---------------------
UPDATE orders_1 SET d_close = 5 WHERE id = 1;

PREPARE orders1 AS
SELECT * FROM _vrp_pgr_pickDeliverEuclidean(
    $$SELECT * FROM orders_1$$,
    $$SELECT * FROM vehicles_1$$);

SELECT todo_start('thorws text changed');

SELECT CASE WHEN (pgr_full_version()).system LIKE 'Darwin%'
THEN skip('test crashing server', 1 )
ELSE collect_tap(
  throws_ok('orders1',
    'XX000',
    'Order not feasible on any truck was found',
    'orders1, Should throw: d_open > d_close')
) END;

UPDATE orders_1 SET d_close = 15 WHERE id = 1;

---------------------
--  p_open > p_close
---------------------
UPDATE orders_1 SET p_close = 1 WHERE id = 1;

SELECT CASE WHEN (pgr_full_version()).system LIKE 'Darwin%'
THEN skip('test crashing server', 1 )
ELSE collect_tap(
   throws_ok('orders1',
    'XX000',
    'Order not feasible on any truck was found',
    'orders1, Should throw: p_open > p_close')
) END;

UPDATE orders_1 SET p_close = 10 WHERE id = 1;

SELECT todo_end();
---------------------
--  amount <= 0
---------------------

UPDATE orders_1 SET amount= -20 WHERE id =1;

SELECT CASE WHEN (pgr_full_version()).system LIKE 'Darwin%'
THEN skip('test crashing server', 1 )
ELSE collect_tap(
  throws_ok('orders1',
    'XX000',
    'Unexpected Negative value in column amount',
    'Should throw: amount(PICKUP) < 0')
) END;

UPDATE orders_1 SET amount= 10 WHERE id =11;

SELECT finish();
ROLLBACK;
