
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
