BEGIN;
SET search_path TO 'vroom', 'public';
SET client_min_messages TO ERROR;

SELECT CASE WHEN min_version('0.3.0') THEN plan (22) ELSE plan(1) END;

CREATE or REPLACE FUNCTION vroom_plain_eq_timestamp()
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  ids BIGINT[] := ARRAY[1, 2, 3, 4, 5];

  jobsPlain TEXT := '$$SELECT * FROM jobs';
  jobsPlain_sql TEXT;
  jobs TEXT := '$$SELECT id, location_index, make_interval(secs => service) AS service, delivery, pickup, skills, priority FROM jobs';
  jobs_sql TEXT;

  shipmentsPlain TEXT := '$$SELECT * FROM shipments';
  shipmentsPlain_sql TEXT;
  shipments TEXT := '$$SELECT id, p_location_index, make_interval(secs => p_service) AS p_service, ' ||
                    'd_location_index, make_interval(secs => d_service) AS d_service, amount, skills, priority FROM shipments';
  shipments_sql TEXT;

  jobsPlain_time_windows TEXT := '$$SELECT * FROM jobs_time_windows$$';
  jobs_time_windows TEXT := '$$SELECT id, (to_timestamp(tw_open) at time zone ''UTC'')::TIMESTAMP AS tw_open, ' ||
                            '(to_timestamp(tw_close) at time zone ''UTC'')::TIMESTAMP AS tw_close FROM jobs_time_windows$$';

  shipmentsPlain_time_windows TEXT := '$$SELECT * FROM shipments_time_windows$$';
  shipments_time_windows TEXT := '$$SELECT id, kind, (to_timestamp(tw_open) at time zone ''UTC'')::TIMESTAMP AS tw_open, ' ||
                                 '(to_timestamp(tw_close) at time zone ''UTC'')::TIMESTAMP AS tw_close  FROM shipments_time_windows$$';

  restPlain_sql TEXT := ', $$SELECT * FROM vehicles$$, $$SELECT * FROM breaks$$' ||
                         ', $$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$, exploration_level => 5, timeout => -1)';

  rest_sql TEXT := ', $$SELECT id, start_index, end_index, capacity, skills, (to_timestamp(tw_open) at time zone ''UTC'')::TIMESTAMP AS tw_open' ||
                   ', (to_timestamp(tw_close) at time zone ''UTC'')::TIMESTAMP AS tw_close, speed_factor, max_tasks FROM vehicles$$' ||
                   ', $$SELECT id, vehicle_id, make_interval(secs => service) AS service FROM breaks$$' ||
                   ', $$SELECT id, (to_timestamp(tw_open) at time zone ''UTC'')::TIMESTAMP AS tw_open' ||
                   ', (to_timestamp(tw_close) at time zone ''UTC'')::TIMESTAMP AS tw_close FROM breaks_time_windows$$, ' ||
                   '$$SELECT start_id, end_id, make_interval(secs => duration) AS duration, cost FROM matrix$$, ' ||
                   'exploration_level => 5, timeout => $$-00:00:01$$::INTERVAL)';

  returnPlain_sql TEXT := 'SELECT * FROM vrp_vroomPlain(';
  return_sql TEXT := 'SELECT seq, vehicle_seq, vehicle_id, step_seq, step_type, task_id, ' ||
                     'EXTRACT (EPOCH FROM arrival) AS arrival, EXTRACT (EPOCH FROM travel_time) AS travel_time, ' ||
                     'EXTRACT (EPOCH FROM service_time) AS service_time, EXTRACT (EPOCH FROM waiting_time) AS waiting_time, ' ||
                     'load FROM vrp_vroom(';

  vroom_sql TEXT;
  vroomPlain_sql TEXT;
  id_filter TEXT;
data TEXT;
BEGIN
  IF NOT min_version('0.3.0') THEN
    RETURN QUERY
    SELECT skip(1, 'Function is modified on 0.3.0');
    RETURN;
  END IF;

  -- Two jobs and shipments
  FOR i in 1..array_length(ids, 1) LOOP
    FOR j in (i+1)..array_length(ids, 1) LOOP
      id_filter := ' WHERE id in (' || i || ', ' || j || ')$$, ';
      jobs_sql := jobs || id_filter || jobs_time_windows;
      shipments_sql := shipments || id_filter || shipments_time_windows;
      vroom_sql := return_sql || jobs_sql || ', ' || shipments_sql || rest_sql;
      jobsPlain_sql := jobsPlain || id_filter || jobsPlain_time_windows;
      shipmentsPlain_sql := shipmentsPlain || id_filter || shipmentsPlain_time_windows;
      vroomPlain_sql := returnPlain_sql || jobsPlain_sql || ', ' || shipmentsPlain_sql || restPlain_sql;
      RAISE WARNING '%', vroom_sql;
      RAISE WARNING '%', vroomPlain_sql;
      RETURN query SELECT set_eq(vroom_sql, vroomPlain_sql);
    END LOOP;
  END LOOP;

  -- Three jobs and shipments
  FOR i in 1..array_length(ids, 1) LOOP
    FOR j in (i+1)..array_length(ids, 1) LOOP
      FOR k in (j+1)..array_length(ids, 1) LOOP
        id_filter := ' WHERE id in (' || i || ', ' || j || ', ' || k || ')$$, ';
        jobs_sql := jobs || id_filter || jobs_time_windows;
        shipments_sql := shipments || id_filter || shipments_time_windows;
        vroom_sql := return_sql || jobs_sql || ', ' || shipments_sql || rest_sql;
        jobsPlain_sql := jobsPlain || id_filter || jobsPlain_time_windows;
        shipmentsPlain_sql := shipmentsPlain || id_filter || shipmentsPlain_time_windows;
        vroomPlain_sql := returnPlain_sql || jobsPlain_sql || ', ' || shipmentsPlain_sql || restPlain_sql;
        RETURN query SELECT set_eq(vroom_sql, vroomPlain_sql);
      END LOOP;
    END LOOP;
  END LOOP;

  -- All the five jobs and shipments
  id_filter := '$$, ';
  jobs_sql := jobs || id_filter || jobs_time_windows;
  shipments_sql := shipments || id_filter || shipments_time_windows;
  vroom_sql := return_sql || jobs_sql || ', ' || shipments_sql || rest_sql;
  jobsPlain_sql := jobsPlain || id_filter || jobsPlain_time_windows;
  shipmentsPlain_sql := shipmentsPlain || id_filter || shipmentsPlain_time_windows;
  vroomPlain_sql := returnPlain_sql || jobsPlain_sql || ', ' || shipmentsPlain_sql || restPlain_sql;
  RETURN query SELECT set_eq(vroom_sql, vroomPlain_sql);

  -- No jobs or shipments
  id_filter := ' WHERE id in (-1)$$, ';
  jobs_sql := jobs || id_filter || jobs_time_windows;
  shipments_sql := shipments || id_filter || shipments_time_windows;
  vroom_sql := return_sql || jobs_sql || ', ' || shipments_sql || rest_sql;
  jobsPlain_sql := jobsPlain || id_filter || jobsPlain_time_windows;
  shipmentsPlain_sql := shipmentsPlain || id_filter || shipmentsPlain_time_windows;
  vroomPlain_sql := returnPlain_sql || jobsPlain_sql || ', ' || shipmentsPlain_sql || restPlain_sql;
  RETURN query SELECT set_eq(vroom_sql, vroomPlain_sql);
END
$BODY$
language plpgsql;

SELECT vroom_plain_eq_timestamp();

SELECT * FROM finish();
ROLLBACK;
