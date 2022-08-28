BEGIN;
SET search_path TO 'ortools', 'public';
SET client_min_messages TO ERROR;
SELECT CASE WHEN min_version('0.3.0') THEN plan (18) ELSE plan(1) END;

CREATE OR REPLACE FUNCTION no_crash()
RETURNS SETOF TEXT AS
$BODY$
DECLARE
params TEXT[];
subs TEXT[];
error_messages TEXT[];
non_empty_args INTEGER[];
BEGIN
    -- vrp_knapsack
    params = ARRAY[
    '$$SELECT * FROM ortools.knapsack_data$$',
    'capacity => 15',
    'max_rows => 100000'
    ]::TEXT[];
    subs = ARRAY[
    'NULL',
    'capacity => NULL',
    'max_rows => NULL'
    ]::TEXT[];
    error_messages = ARRAY[
    38000,
    38000,
    ''
    ]::TEXT[];
    non_empty_args = ARRAY[0, 1, 2, 3]::INTEGER[];
    
    RETURN QUERY SELECT * FROM no_crash_test('vrp_knapsack', params, subs, error_messages, non_empty_args);

    --vrp_multiple_knapsack
    params = ARRAY[
    '$$SELECT * FROM ortools.multiple_knapsack_data$$',
    'capacities => ARRAY[100,100,100,100,100]',
    'max_rows => 100000'
    ]::TEXT[];
    subs = ARRAY[
    'NULL',
    'capacities => NULL',
    'max_rows => NULL'
    ]::TEXT[];
    error_messages = ARRAY[
    38000,
    38000,
    ''
    ]::TEXT[];
    non_empty_args = ARRAY[0, 1, 2, 3]::INTEGER[];
    
    RETURN QUERY SELECT * FROM no_crash_test('vrp_multiple_knapsack', params, subs, error_messages, non_empty_args);

    -- bin_packing
    params = ARRAY[
    '$$SELECT * FROM ortools.bin_packing_data$$',
    'bin_capacity => 100',
    'max_rows => 100000'
    ]::TEXT[];
    subs = ARRAY[
    'NULL',
    'bin_capacity => NULL',
    'max_rows => NULL'
    ]::TEXT[];
    error_messages = ARRAY[
    38000,
    38000,
    ''
    ]::TEXT[];
    non_empty_args = ARRAY[0, 1, 2, 3]::INTEGER[];
    
    RETURN QUERY SELECT * FROM no_crash_test('vrp_bin_packing', params, subs, error_messages, non_empty_args);
END
$BODY$
LANGUAGE plpgsql VOLATILE;

SELECT * FROM no_crash();

ROLLBACK;
