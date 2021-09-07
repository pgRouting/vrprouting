
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
