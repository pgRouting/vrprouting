
/*******
Creating an initial solution
*******/
CREATE OR REPLACE PROCEDURE initial_solution()
AS
$BODY$
DECLARE
  day TEXT;
  days TEXT[];
BEGIN
  UPDATE vehicles SET stops = NULL;

  days := ARRAY['2019-12-09', '2019-12-10', '2019-12-11', '2019-12-12','2019-12-13'];

  FOREACH day IN ARRAY days LOOP
    WITH
    new_stops AS (
        SELECT vehicle_id AS id, array_remove(array_agg(order_id ORDER BY seq)::BIGINT[],-1::BIGINT) AS stops
        FROM vrp_pickDeliverRaw(
            format($$SELECT * FROM shipments WHERE date_trunc('day', p_tw_open) = '%1$s 00:00:00' ORDER BY id$$, day),
            format($$SELECT * FROM vehicles  WHERE date_trunc('day', s_tw_open) = '%1$s 00:00:00' ORDER BY id$$, day),
            $$SELECT start_vid, end_vid, agg_cost FROM timeMatrix$$,
            format($$SELECT * FROM tdm_raw('%1$s'::TIMESTAMP)$$, day),
            optimize => true, factor => 1::float, max_cycles => 1, stop_on_all_served => true)
          GROUP BY vehicle_id
        )
    UPDATE vehicles AS v
    SET stops = n.stops
    FROM new_stops AS n WHERE v.id = n.id;
  END LOOP;
END;
$BODY$
LANGUAGE plpgsql;

/**
For testing compatability
**/
DROP FUNCTION IF EXISTS test_all_shipments;
CREATE OR REPLACE FUNCTION test_all_shipments()
RETURNS VOID AS
$BODY$
DECLARE
order_row RECORD;
info RECORD;

BEGIN
  FOR order_row IN SELECT * FROM shipments ORDER BY booking_date LOOP
    SELECT array_agg(vehicle_id ORDER BY vehicle_id) AS v
    FROM vrp_compatibleVehicles(
      $$
        SELECT id, amount, p_id, p_tw_open, p_tw_close, p_t_service, d_id, d_tw_open, d_tw_close, d_t_service
        FROM shipments
      $$,
      $$
        SELECT id, capacity, s_id, s_tw_open, s_tw_close, s_t_service, e_t_service
        FROM vehicles
      $$,
      $$SELECT start_vid, end_vid, travel_time FROM timeMatrix$$,
      $$SELECT * FROM tdm('2019-12-09'::TIMESTAMP, '2019-12-13'::TIMESTAMP)$$,
      order_row.id, factor=>1.0::FLOAT
  ) INTO info;

  RAISE NOTICE 'order % Compatible vehicles %', order_row.id, info.v ;
  END LOOP;
END;
$BODY$
LANGUAGE plpgsql;


DROP FUNCTION IF EXISTS test_all_shipments_raw;
CREATE OR REPLACE FUNCTION test_all_shipments_raw()
RETURNS VOID AS
$BODY$
DECLARE
order_row RECORD;
info RECORD;

BEGIN
  FOR order_row IN SELECT * FROM shipments ORDER BY booking_date LOOP
    SELECT array_agg(vehicle_id ORDER BY vehicle_id) AS v
    FROM vrp_compatibleVehiclesRaw(
      $$SELECT * FROM shipments$$,
      $$SELECT * FROM vehicles$$::TEXT,
      $$SELECT start_vid, end_vid, agg_cost FROM timematrix$$,
      $$SELECT * FROM tdm_raw('2019-12-09'::TIMESTAMP, '2019-12-13'::TIMESTAMP)$$,
      order_row.id, factor=>1.0::FLOAT
  ) INTO info;

  RAISE NOTICE 'order % Compatible vehicles %', order_row.id, info.v ;
  END LOOP;
END;
$BODY$
LANGUAGE plpgsql;

/* testing view route not used currently */
DROP FUNCTION IF EXISTS test_all_vehicles;
CREATE OR REPLACE FUNCTION test_all_vehicles()
RETURNS VOID AS
$BODY$
DECLARE
vehicle_row RECORD;
info RECORD;

BEGIN
    FOR vehicle_row IN SELECT * FROM vehicles ORDER BY id LOOP
        RAISE NOTICE 'vehicle %:', vehicle_row.id;
        FOR info IN
            SELECT *
            FROM vrp_viewRoute(
              $$SELECT * FROM shipments$$::TEXT,
              $$SELECT * FROM vehicles WHERE stops IS NOT NULL$$::TEXT,
              $$SELECT start_vid, end_vid, agg_cost *  3600 AS agg_cost FROM timeMatrix$$::TEXT,
              $$SELECT * FROM tdm('2019-12-09'::TIMESTAMP, '2019-12-13'::TIMESTAMP)$$::TEXT,
              v_id => vehicle_row.id, factor => 1.0::float
            ) ORDER BY stop_seq
        LOOP

            RAISE NOTICE ' vehicle %:  (%)  type=%  stop_id=% o=% c=% tt=%  a=%  wt=%  s=%  st=% d=%',
                info.vehicle_id, info.stop_seq, info.stop_type,
                info.stop_id,
                info.shipment_id,
                info.load,
                info.travel,
                info.arrival,
                info.waiting_time,
                info.schedule,
                info.service,
                info.departure;
        END LOOP;
    END LOOP;
END;
$BODY$
LANGUAGE plpgsql;

DROP FUNCTION IF EXISTS test_all_vehicles_raw;
CREATE OR REPLACE FUNCTION test_all_vehicles_raw()
RETURNS VOID AS
$BODY$
DECLARE
vehicle_row RECORD;
info RECORD;

BEGIN
    FOR vehicle_row IN SELECT * FROM vehicles ORDER BY id LOOP
        RAISE NOTICE 'vehicle %:', vehicle_row.id;
        FOR info IN
            SELECT *
            FROM vrp_viewRouteRaw(
              $$SELECT * FROM shipments$$::TEXT,
              $$SELECT * FROM vehicles WHERE stops IS NOT NULL$$::TEXT,
              $$SELECT start_vid, end_vid, agg_cost *  3600 AS agg_cost FROM timeMatrix$$::TEXT,
              $$SELECT * FROM tdm_raw('2019-12-09'::TIMESTAMP, '2019-12-13'::TIMESTAMP)$$::TEXT,
              v_id => vehicle_row.id, factor => 1.0::float
            ) ORDER BY stop_seq
        LOOP

            RAISE NOTICE ' vehicle %:  (%)  type=%  stop_id=% o=% c=% tt=%  a=%  wt=%  s=%  st=% d=%',
                info.vehicle_id, info.stop_seq, info.stop_type,
                info.stop_id,
                info.shipment_id,
                info.load,
                info.travel_fd,
                info.arrival_ft,
                info.wait_fd,
                info.schedule_ft,
                info.service_fd,
                info.departure_ft;
        END LOOP;
    END LOOP;
END;
$BODY$
LANGUAGE plpgsql;
