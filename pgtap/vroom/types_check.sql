BEGIN;
SET search_path TO 'vroom', 'public';

SELECT CASE WHEN min_version('0.3.0') THEN plan (30) ELSE plan(1) END;

CREATE OR REPLACE FUNCTION types_check()
RETURNS SETOF TEXT AS
$BODY$
BEGIN

  IF NOT min_version('0.3.0') THEN
    RETURN QUERY
    SELECT skip(1, 'Function is modified on 0.3.0');
    RETURN;
  END IF;

  -- vrp_vroomPlain
  RETURN QUERY
  SELECT has_function('vrp_vroomplain');
  RETURN QUERY
  SELECT has_function('vrp_vroomplain', ARRAY['text', 'text', 'text', 'text', 'text', 'text', 'text', 'text', 'integer', 'integer']);
  RETURN QUERY
  SELECT function_returns('vrp_vroomplain', ARRAY['text', 'text', 'text', 'text', 'text', 'text', 'text', 'text', 'integer', 'integer'], 'setof record');

  -- parameter names
  RETURN QUERY
  SELECT bag_has(
    $$SELECT proargnames from pg_proc where proname = 'vrp_vroomplain'$$,
    $$SELECT '{"","","","","","","","","exploration_level","timeout","seq","vehicle_seq","vehicle_id","step_seq","step_type",'
              '"task_id","arrival","travel_time","service_time","waiting_time","load"}'::TEXT[]$$
  );

  -- parameter types
  RETURN QUERY
  SELECT set_eq(
    $$SELECT  proallargtypes from pg_proc where proname = 'vrp_vroomplain'$$,
    $$VALUES
      ('{25,25,25,25,25,25,25,25,23,23,20,20,20,20,23,20,23,23,23,23,1016}'::OID[])
    $$
  );


  -- vrp_vroom
  RETURN QUERY
  SELECT has_function('vrp_vroom');
  RETURN QUERY
  SELECT has_function('vrp_vroom', ARRAY['text', 'text', 'text', 'text', 'text', 'text', 'text', 'text', 'integer', 'interval']);
  RETURN QUERY
  SELECT function_returns('vrp_vroom', ARRAY['text', 'text', 'text', 'text', 'text', 'text', 'text', 'text', 'integer', 'interval'], 'setof record');

  -- parameter names
  RETURN QUERY
  SELECT bag_has(
    $$SELECT proargnames from pg_proc where proname = 'vrp_vroom'$$,
    $$SELECT '{"","","","","","","","","exploration_level","timeout","seq","vehicle_seq","vehicle_id","step_seq","step_type",'
              '"task_id","arrival","travel_time","service_time","waiting_time","load"}'::TEXT[]$$
  );

  -- parameter types
  RETURN QUERY
  SELECT set_eq(
    $$SELECT  proallargtypes from pg_proc where proname = 'vrp_vroom'$$,
    $$VALUES
      ('{25,25,25,25,25,25,25,25,23,1186,20,20,20,20,23,20,1114,1186,1186,1186,1016}'::OID[])
    $$
  );


  -- vrp_vroomJobsPlain
  RETURN QUERY
  SELECT has_function('vrp_vroomjobsplain');
  RETURN QUERY
  SELECT has_function('vrp_vroomjobsplain', ARRAY['text', 'text', 'text', 'text', 'text', 'text', 'integer', 'integer']);
  RETURN QUERY
  SELECT function_returns('vrp_vroomjobsplain', ARRAY['text', 'text', 'text', 'text', 'text', 'text', 'integer', 'integer'], 'setof record');

  -- parameter names
  RETURN QUERY
  SELECT bag_has(
    $$SELECT proargnames from pg_proc where proname = 'vrp_vroomjobsplain'$$,
    $$SELECT '{"","","","","","","exploration_level","timeout","seq","vehicle_seq","vehicle_id","step_seq","step_type",'
              '"task_id","arrival","travel_time","service_time","waiting_time","load"}'::TEXT[]$$
  );

  -- parameter types
  RETURN QUERY
  SELECT set_eq(
    $$SELECT  proallargtypes from pg_proc where proname = 'vrp_vroomjobsplain'$$,
    $$VALUES
      ('{25,25,25,25,25,25,23,23,20,20,20,20,23,20,23,23,23,23,1016}'::OID[])
    $$
  );


  -- vrp_vroomJobs
  RETURN QUERY
  SELECT has_function('vrp_vroomjobs');
  RETURN QUERY
  SELECT has_function('vrp_vroomjobs', ARRAY['text', 'text', 'text', 'text', 'text', 'text', 'integer', 'interval']);
  RETURN QUERY
  SELECT function_returns('vrp_vroomjobs', ARRAY['text', 'text', 'text', 'text', 'text', 'text', 'integer', 'interval'], 'setof record');

  -- parameter names
  RETURN QUERY
  SELECT bag_has(
    $$SELECT proargnames from pg_proc where proname = 'vrp_vroomjobs'$$,
    $$SELECT '{"","","","","","","exploration_level","timeout","seq","vehicle_seq","vehicle_id","step_seq","step_type",'
              '"task_id","arrival","travel_time","service_time","waiting_time","load"}'::TEXT[]$$
  );

  -- parameter types
  RETURN QUERY
  SELECT set_eq(
    $$SELECT  proallargtypes from pg_proc where proname = 'vrp_vroomjobs'$$,
    $$VALUES
      ('{25,25,25,25,25,25,23,1186,20,20,20,20,23,20,1114,1186,1186,1186,1016}'::OID[])
    $$
  );


  -- vrp_vroomShipmentsPlain
  RETURN QUERY
  SELECT has_function('vrp_vroomshipmentsplain');
  RETURN QUERY
  SELECT has_function('vrp_vroomshipmentsplain', ARRAY['text', 'text', 'text', 'text', 'text', 'text', 'integer', 'integer']);
  RETURN QUERY
  SELECT function_returns('vrp_vroomshipmentsplain', ARRAY['text', 'text', 'text', 'text', 'text', 'text', 'integer', 'integer'], 'setof record');

  -- parameter names
  RETURN QUERY
  SELECT bag_has(
    $$SELECT proargnames from pg_proc where proname = 'vrp_vroomshipmentsplain'$$,
    $$SELECT '{"","","","","","","exploration_level","timeout","seq","vehicle_seq","vehicle_id","step_seq","step_type",'
              '"task_id","arrival","travel_time","service_time","waiting_time","load"}'::TEXT[]$$
  );

  -- parameter types
  RETURN QUERY
  SELECT set_eq(
    $$SELECT  proallargtypes from pg_proc where proname = 'vrp_vroomshipmentsplain'$$,
    $$VALUES
      ('{25,25,25,25,25,25,23,23,20,20,20,20,23,20,23,23,23,23,1016}'::OID[])
    $$
  );


  -- vrp_vroomShipments
  RETURN QUERY
  SELECT has_function('vrp_vroomshipments');
  RETURN QUERY
  SELECT has_function('vrp_vroomshipments', ARRAY['text', 'text', 'text', 'text', 'text', 'text', 'integer', 'interval']);
  RETURN QUERY
  SELECT function_returns('vrp_vroomshipments', ARRAY['text', 'text', 'text', 'text', 'text', 'text', 'integer', 'interval'], 'setof record');

  -- parameter names
  RETURN QUERY
  SELECT bag_has(
    $$SELECT proargnames from pg_proc where proname = 'vrp_vroomshipments'$$,
    $$SELECT '{"","","","","","","exploration_level","timeout","seq","vehicle_seq","vehicle_id","step_seq","step_type",'
              '"task_id","arrival","travel_time","service_time","waiting_time","load"}'::TEXT[]$$
  );

  -- parameter types
  RETURN QUERY
  SELECT set_eq(
    $$SELECT  proallargtypes from pg_proc where proname = 'vrp_vroomshipments'$$,
    $$VALUES
      ('{25,25,25,25,25,25,23,1186,20,20,20,20,23,20,1114,1186,1186,1186,1016}'::OID[])
    $$
  );
END;
$BODY$
LANGUAGE plpgsql;

SELECT types_check();

SELECT * FROM finish();
ROLLBACK;
