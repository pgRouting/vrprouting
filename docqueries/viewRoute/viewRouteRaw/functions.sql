SET search_path TO 'data', 'status','triggers','public';


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

