BEGIN;
SET search_path TO 'vroom', 'public';

SELECT CASE WHEN min_version('0.2.0') THEN plan (15) ELSE plan(1) END;

CREATE OR REPLACE FUNCTION types_check()
RETURNS SETOF TEXT AS
$BODY$
BEGIN

  IF NOT min_version('0.2.0') THEN
    RETURN QUERY
    SELECT skip(1, 'Function is new on 0.2.0');
    RETURN;
  END IF;

  -- vrp_vroomPlain
  RETURN QUERY
  SELECT has_function('vrp_vroomplain');
  RETURN QUERY
  SELECT has_function('vrp_vroomplain', ARRAY['text', 'text', 'text', 'text', 'text', 'text', 'text', 'text']);
  RETURN QUERY
  SELECT function_returns('vrp_vroomplain', ARRAY['text', 'text', 'text', 'text', 'text', 'text', 'text', 'text'], 'setof record');

  -- parameter names
  RETURN QUERY
  SELECT bag_has(
    $$SELECT proargnames from pg_proc where proname = 'vrp_vroomplain'$$,
    $$SELECT '{"","","","","","","","","seq","vehicle_seq","vehicle_id","step_seq","step_type",'
              '"task_id","arrival","travel_time","service_time","waiting_time","load"}'::TEXT[]$$
  );

  -- parameter types
  RETURN QUERY
  SELECT set_eq(
    $$SELECT  proallargtypes from pg_proc where proname = 'vrp_vroomplain'$$,
    $$VALUES
      ('{25,25,25,25,25,25,25,25,20,20,20,20,23,20,23,23,23,23,1016}'::OID[])
    $$
  );


  -- vrp_vroomJobsPlain
  RETURN QUERY
  SELECT has_function('vrp_vroomjobsplain');
  RETURN QUERY
  SELECT has_function('vrp_vroomjobsplain', ARRAY['text', 'text', 'text', 'text', 'text', 'text']);
  RETURN QUERY
  SELECT function_returns('vrp_vroomjobsplain', ARRAY['text', 'text', 'text', 'text', 'text', 'text'], 'setof record');

  -- parameter names
  RETURN QUERY
  SELECT bag_has(
    $$SELECT proargnames from pg_proc where proname = 'vrp_vroomjobsplain'$$,
    $$SELECT '{"","","","","","","seq","vehicle_seq","vehicle_id","step_seq","step_type",'
              '"task_id","arrival","travel_time","service_time","waiting_time","load"}'::TEXT[]$$
  );

  -- parameter types
  RETURN QUERY
  SELECT set_eq(
    $$SELECT  proallargtypes from pg_proc where proname = 'vrp_vroomjobsplain'$$,
    $$VALUES
      ('{25,25,25,25,25,25,20,20,20,20,23,20,23,23,23,23,1016}'::OID[])
    $$
  );


  -- vrp_vroomShipmentsPlain
  RETURN QUERY
  SELECT has_function('vrp_vroomshipmentsplain');
  RETURN QUERY
  SELECT has_function('vrp_vroomshipmentsplain', ARRAY['text', 'text', 'text', 'text', 'text', 'text']);
  RETURN QUERY
  SELECT function_returns('vrp_vroomshipmentsplain', ARRAY['text', 'text', 'text', 'text', 'text', 'text'], 'setof record');

  -- parameter names
  RETURN QUERY
  SELECT bag_has(
    $$SELECT proargnames from pg_proc where proname = 'vrp_vroomshipmentsplain'$$,
    $$SELECT '{"","","","","","","seq","vehicle_seq","vehicle_id","step_seq","step_type",'
              '"task_id","arrival","travel_time","service_time","waiting_time","load"}'::TEXT[]$$
  );

  -- parameter types
  RETURN QUERY
  SELECT set_eq(
    $$SELECT  proallargtypes from pg_proc where proname = 'vrp_vroomshipmentsplain'$$,
    $$VALUES
      ('{25,25,25,25,25,25,20,20,20,20,23,20,23,23,23,23,1016}'::OID[])
    $$
  );
END;
$BODY$
LANGUAGE plpgsql;

SELECT types_check();

SELECT * FROM finish();
ROLLBACK;
