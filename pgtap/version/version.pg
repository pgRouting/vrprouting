
BEGIN;

UPDATE edge_table SET cost = sign(cost), reverse_cost = sign(reverse_cost);
SELECT plan(5);


SELECT has_function('vrp_version');
SELECT has_function('vrp_version', ARRAY[]::text[]);
SELECT function_returns('vrp_version', ARRAY[]::text[], 'text');

SELECT set_eq(
    $$SELECT  proargnames from pg_proc where proname = 'vrp_version'$$,
    $$SELECT  NULL::TEXT[] $$
);

-- parameter types
SELECT set_eq(
    $$SELECT  proallargtypes from pg_proc where proname = 'vrp_version'$$,
    $$SELECT  NULL::OID[] $$
);


SELECT finish();
ROLLBACK;
