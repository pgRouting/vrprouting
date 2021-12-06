BEGIN;
SET search_path TO 'vroom', 'public';
SET client_min_messages TO ERROR;

SELECT CASE WHEN min_version('0.2.0') THEN plan (22) ELSE plan(1) END;

CREATE or REPLACE FUNCTION vroomShipments_eq_vroom()
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  ids BIGINT[] := ARRAY[1, 2, 3, 4, 5];
  jobs_sql TEXT := '$$SELECT * FROM jobs WHERE id = -1$$, $$SELECT * FROM jobs_time_windows WHERE id = -1$$, ';
  shipments_sql TEXT;
  empty_time_windows TEXT := '$$SELECT * FROM shipments_time_windows WHERE id = -1$$';
  rest_sql TEXT := ', $$SELECT * FROM vehicles$$, $$SELECT * FROM breaks$$' ||
                   ', $$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$, exploration_level => 5, timeout => -1)';
  vroom_sql TEXT;
  vroomShipments_sql TEXT;
data TEXT;
BEGIN
  IF NOT min_version('0.2.0') THEN
    RETURN QUERY
    SELECT skip(1, 'Function is new on 0.2.0');
    RETURN;
  END IF;

  -- Two shipments
  FOR i in 1..array_length(ids, 1) LOOP
    FOR j in (i+1)..array_length(ids, 1) LOOP
      shipments_sql := '$$SELECT * FROM shipments WHERE id in (' || i || ', ' || j || ')$$, ' || empty_time_windows;
      vroom_sql := 'SELECT * FROM vrp_vroomPlain(' || jobs_sql || shipments_sql || rest_sql;
      vroomShipments_sql := 'SELECT * FROM vrp_vroomShipmentsPlain(' || shipments_sql || rest_sql;
      RETURN query SELECT set_eq(vroom_sql, vroomShipments_sql);
    END LOOP;
  END LOOP;

  -- Three shipments
  FOR i in 1..array_length(ids, 1) LOOP
    FOR j in (i+1)..array_length(ids, 1) LOOP
      FOR k in (j+1)..array_length(ids, 1) LOOP
        shipments_sql := '$$SELECT * FROM shipments WHERE id in (' || i || ', ' || j || ', ' || k || ')$$, ' || empty_time_windows;
        vroom_sql := 'SELECT * FROM vrp_vroomPlain(' || jobs_sql || shipments_sql || rest_sql;
        vroomShipments_sql := 'SELECT * FROM vrp_vroomShipmentsPlain(' || shipments_sql || rest_sql;
        RETURN query SELECT set_eq(vroom_sql, vroomShipments_sql);
      END LOOP;
    END LOOP;
  END LOOP;

  -- All the five shipments
  shipments_sql := '$$SELECT * FROM shipments$$, ' || empty_time_windows;
  vroom_sql := 'SELECT * FROM vrp_vroomPlain(' || jobs_sql || shipments_sql || rest_sql;
  vroomShipments_sql := 'SELECT * FROM vrp_vroomShipmentsPlain(' || shipments_sql || rest_sql;
  RETURN query SELECT set_eq(vroom_sql, vroomShipments_sql);

  -- No shipments
  shipments_sql := '$$SELECT * FROM shipments WHERE id = -1$$, ' || empty_time_windows;
  vroom_sql := 'SELECT * FROM vrp_vroomPlain(' || jobs_sql || shipments_sql || rest_sql;
  vroomShipments_sql := 'SELECT * FROM vrp_vroomShipmentsPlain(' || shipments_sql || rest_sql;
  RETURN query SELECT set_eq(vroom_sql, vroomShipments_sql);
END
$BODY$
language plpgsql;

SELECT vroomShipments_eq_vroom();

SELECT * FROM finish();
ROLLBACK;
