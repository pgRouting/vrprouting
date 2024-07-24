BEGIN;

UPDATE edge_table SET cost = sign(cost), reverse_cost = sign(reverse_cost);
SELECT plan(13);

PREPARE orders AS
SELECT * FROM onedepot.orders;
PREPARE vehicles AS
SELECT * FROM onedepot.vehicles;
PREPARE distance AS
SELECT * FROM onedepot.distances;

SELECT isnt_empty('orders', 'Should be not empty to tests be meaningful');
SELECT isnt_empty('vehicles', 'Should be not empty to tests be meaningful');
SELECT isnt_empty('distance', 'Should be not empty to tests be meaningful');


CREATE OR REPLACE FUNCTION test_function()
RETURNS SETOF TEXT AS
$BODY$
DECLARE
params TEXT[];
subs TEXT[];
BEGIN
    -- one to one
    params = ARRAY[
    '$$SELECT * FROM onedepot.orders ORDER BY id$$',
    '$$SELECT * FROM onedepot.vehicles$$',
    '$$SELECT * FROM onedepot.distances$$',
    '1'
    ]::TEXT[];
    subs = ARRAY[
    'NULL',
    'NULL',
    'NULL',
    'NULL'
    ]::TEXT[];
    RETURN query SELECT * FROM no_crash_test('vrp_OneDepot', params, subs);
END
$BODY$
LANGUAGE plpgsql VOLATILE;


SELECT * FROM test_function();

ROLLBACK;
