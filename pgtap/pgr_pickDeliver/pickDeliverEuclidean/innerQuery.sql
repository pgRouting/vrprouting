BEGIN;

UPDATE edge_table SET cost = sign(cost), reverse_cost = sign(reverse_cost);
SELECT plan(104);
SET client_min_messages TO ERROR;

/* A call looks like this
TODO select a smaller test, because each passing test takes about 19 seconds
SELECT * INTO pickDeliverResults FROM vrp_pgr_pickdeliverEuclidean(
    $$SELECT * FROM orders_1$$,
    $$SELECT * FROM vehicles_1$$,
    30);
*/

SELECT has_function('vrp_pgr_pickdelivereuclidean',
    ARRAY['text', 'text', 'double precision', 'integer', 'integer']);

SELECT function_returns('vrp_pgr_pickdelivereuclidean',
    ARRAY['text', 'text', 'double precision', 'integer', 'integer'],
    'setof record');

/* testing the pick/deliver orders*/
CREATE OR REPLACE FUNCTION test_anyInteger_orders(fn TEXT, params TEXT[], parameter TEXT)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
start_sql TEXT;
end_sql TEXT;
query TEXT;
p TEXT;
BEGIN
    start_sql = 'select * FROM ' || fn || '($$ SELECT ';
    FOREACH  p IN ARRAY params LOOP
        IF p = parameter THEN CONTINUE;
        END IF;
        start_sql = start_sql || p || ', ';
    END LOOP;
    end_sql = ' FROM orders_1 $$,  $$SELECT * FROM vehicles_1 $$, max_cycles := 30)';

    query := start_sql || parameter || '::SMALLINT ' || end_sql;
    RETURN query SELECT lives_ok(query);

    query := start_sql || parameter || '::INTEGER ' || end_sql;
    RETURN query SELECT lives_ok(query);

    query := start_sql || parameter || '::BIGINT ' || end_sql;
    RETURN query SELECT lives_ok(query);

    query := start_sql || parameter || '::REAL ' || end_sql;
    RETURN query SELECT throws_ok(query);

    query := start_sql || parameter || '::FLOAT8 ' || end_sql;
    RETURN query SELECT throws_ok(query);

    query := start_sql || parameter || '::NUMERIC ' || end_sql;
    RETURN query SELECT throws_ok(query);
END;
$BODY$ LANGUAGE plpgsql;

/* testing the pick/deliver orders*/
CREATE OR REPLACE FUNCTION test_anyNumerical_orders(fn TEXT, params TEXT[], parameter TEXT)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
start_sql TEXT;
end_sql TEXT;
query TEXT;
p TEXT;
BEGIN
    start_sql = 'select * from ' || fn || '($$ SELECT ';
    FOREACH  p IN ARRAY params LOOP
        IF p = parameter THEN CONTINUE;
        END IF;
        start_sql = start_sql || p || ', ';
    END LOOP;
    end_sql = ' FROM orders_1 $$,  $$SELECT * FROM vehicles_1 $$, max_cycles := 30)';

    query := start_sql || parameter || '::SMALLINT ' || end_sql;
    RETURN query SELECT lives_ok(query);

    query := start_sql || parameter || '::INTEGER ' || end_sql;
    RETURN query SELECT lives_ok(query);

    query := start_sql || parameter || '::BIGINT ' || end_sql;
    RETURN query SELECT lives_ok(query);

    query := start_sql || parameter || '::REAL ' || end_sql;
    RETURN query SELECT lives_ok(query);

    query := start_sql || parameter || '::FLOAT8 ' || end_sql;
    RETURN query SELECT lives_ok(query);

    query := start_sql || parameter || '::NUMERIC ' || end_sql;
    RETURN query SELECT lives_ok(query);
END;
$BODY$ LANGUAGE plpgsql;

/*
testing the pick/deliver vehicles
*/
CREATE OR REPLACE FUNCTION test_anyInteger_vehicles(fn TEXT, params TEXT[], parameter TEXT)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
start_sql TEXT;
end_sql TEXT;
query TEXT;
p TEXT;
BEGIN
    start_sql = 'SELECT * FROM ' || fn || '($$ SELECT * FROM orders_1 $$, $$SELECT ';

    FOREACH  p IN ARRAY params LOOP
        IF p = parameter THEN CONTINUE;
        END IF;
        start_sql = start_sql || p || ', ';
    END LOOP;
    end_sql = ' FROM  vehicles_1 $$, max_cycles := 30)';

    query := start_sql || parameter || '::SMALLINT ' || end_sql;
    RETURN query SELECT lives_ok(query);

    query := start_sql || parameter || '::INTEGER ' || end_sql;
    RETURN query SELECT lives_ok(query);

    query := start_sql || parameter || '::BIGINT ' || end_sql;
    RETURN query SELECT lives_ok(query);

    query := start_sql || parameter || '::REAL ' || end_sql;
    RETURN query SELECT throws_ok(query);

    query := start_sql || parameter || '::FLOAT8 ' || end_sql;
    RETURN query SELECT throws_ok(query);
END;
$BODY$ LANGUAGE plpgsql;

/*
testing the pick/deliver vehicles
 */
CREATE OR REPLACE FUNCTION test_anyNumerical_vehicles(fn TEXT, params TEXT[], parameter TEXT)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
start_sql TEXT;
end_sql TEXT;
query TEXT;
p TEXT;
BEGIN
    start_sql = 'SELECT * FROM ' || fn || '($$ SELECT * FROM orders_1 $$, $$ SELECT ';
    FOREACH  p IN ARRAY params LOOP
        IF p = parameter THEN CONTINUE;
        END IF;
        start_sql = start_sql || p || ', ';
    END LOOP;
    end_sql = ' FROM vehicles_1 $$, max_cycles := 30)';

    query := start_sql || parameter || '::SMALLINT ' || end_sql;
    RETURN query SELECT lives_ok(query);

    query := start_sql || parameter || '::INTEGER ' || end_sql;
    RETURN query SELECT lives_ok(query);

    query := start_sql || parameter || '::BIGINT ' || end_sql;
    RETURN query SELECT lives_ok(query);

    query := start_sql || parameter || '::REAL ' || end_sql;
    RETURN query SELECT lives_ok(query);

    query := start_sql || parameter || '::FLOAT8 ' || end_sql;
    RETURN query SELECT lives_ok(query);
END;
$BODY$ LANGUAGE plpgsql;

SELECT test_anyInteger_orders('vrp_pgr_pickdelivereuclidean',
    ARRAY['id', 'amount',
    'p_x', 'p_y', 'p_open', 'p_close', 'p_service',
    'd_x', 'd_y', 'd_open', 'd_close', 'd_service'],
    'id');

SELECT test_anyInteger_orders('vrp_pgr_pickdelivereuclidean',
    ARRAY['id', 'amount',
    'p_x', 'p_y', 'p_open', 'p_close', 'p_service',
    'd_x', 'd_y', 'd_open', 'd_close', 'd_service'],
    'amount');

SELECT test_anynumerical_orders('vrp_pgr_pickdelivereuclidean',
    ARRAY['id', 'amount',
    'p_x', 'p_y', 'p_open', 'p_close', 'p_service',
    'd_x', 'd_y', 'd_open', 'd_close', 'd_service'],
    'p_x');
SELECT test_anynumerical_orders('vrp_pgr_pickdelivereuclidean',
    ARRAY['id', 'amount',
    'p_x', 'p_y', 'p_open', 'p_close', 'p_service',
    'd_x', 'd_y', 'd_open', 'd_close', 'd_service'],
    'p_y');
SELECT test_anyInteger_orders('vrp_pgr_pickdelivereuclidean',
    ARRAY['id', 'amount',
    'p_x', 'p_y', 'p_open', 'p_close', 'p_service',
    'd_x', 'd_y', 'd_open', 'd_close', 'd_service'],
    'p_open');
SELECT test_anyInteger_orders('vrp_pgr_pickdelivereuclidean',
    ARRAY['id', 'amount',
    'p_x', 'p_y', 'p_open', 'p_close', 'p_service',
    'd_x', 'd_y', 'd_open', 'd_close', 'd_service'],
    'p_close');
SELECT test_anyInteger_orders('vrp_pgr_pickdelivereuclidean',
    ARRAY['id', 'amount',
    'p_x', 'p_y', 'p_open', 'p_close', 'p_service',
    'd_x', 'd_y', 'd_open', 'd_close', 'd_service'],
    'p_service');

SELECT test_anynumerical_orders('vrp_pgr_pickdelivereuclidean',
    ARRAY['id', 'amount',
    'p_x', 'p_y', 'p_open', 'p_close', 'p_service',
    'd_x', 'd_y', 'd_open', 'd_close', 'd_service'],
    'd_x');
SELECT test_anynumerical_orders('vrp_pgr_pickdelivereuclidean',
    ARRAY['id', 'amount',
    'p_x', 'p_y', 'p_open', 'p_close', 'p_service',
    'd_x', 'd_y', 'd_open', 'd_close', 'd_service'],
    'd_y');
SELECT test_anyInteger_orders('vrp_pgr_pickdelivereuclidean',
    ARRAY['id', 'amount',
    'p_x', 'p_y', 'p_open', 'p_close', 'p_service',
    'd_x', 'd_y', 'd_open', 'd_close', 'd_service'],
    'd_open');
SELECT test_anyInteger_orders('vrp_pgr_pickdelivereuclidean',
    ARRAY['id', 'amount',
    'p_x', 'p_y', 'p_open', 'p_close', 'p_service',
    'd_x', 'd_y', 'd_open', 'd_close', 'd_service'],
    'd_close');
SELECT test_anyInteger_orders('vrp_pgr_pickdelivereuclidean',
    ARRAY['id', 'amount',
    'p_x', 'p_y', 'p_open', 'p_close', 'p_service',
    'd_x', 'd_y', 'd_open', 'd_close', 'd_service'],
    'd_service');

/* Currently this are not used TODO add when they are used
    'end_x', 'end_y', 'end_open', 'end_close', 'end_service'],
    'speed' is optional defaults to 1
    's_service' is optional defaults to 0
*/
SELECT test_anyInteger_vehicles('vrp_pgr_pickdelivereuclidean',
    ARRAY['id', 'capacity',
    's_x', 's_y', 's_open', 's_close'],
    'id');
SELECT test_anyInteger_vehicles('vrp_pgr_pickdelivereuclidean',
    ARRAY['id', 'capacity',
    's_x', 's_y', 's_open', 's_close'],
    'capacity');
SELECT test_anyNumerical_vehicles('vrp_pgr_pickdelivereuclidean',
    ARRAY['id', 'capacity',
    's_x', 's_y', 's_open', 's_close'],
    's_x');
SELECT test_anyNumerical_vehicles('vrp_pgr_pickdelivereuclidean',
    ARRAY['id', 'capacity',
    's_x', 's_y', 's_open', 's_close'],
    's_y');
SELECT test_anyInteger_vehicles('vrp_pgr_pickdelivereuclidean',
    ARRAY['id', 'capacity',
    's_x', 's_y', 's_open', 's_close'],
    's_open');
SELECT test_anyInteger_vehicles('vrp_pgr_pickdelivereuclidean',
    ARRAY['id', 'capacity',
    's_x', 's_y', 's_open', 's_close'],
    's_close');

SELECT finish();
ROLLBACK;
