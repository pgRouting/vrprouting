SET search_path TO 'example2', 'public';

SELECT *
FROM vrp_compatibleVehiclesRaw(
    $$SELECT * FROM shipments$$,
    $$SELECT * FROM vehicles$$::TEXT,
    $$SELECT start_vid, end_vid, agg_cost FROM timeMatrix$$,
    $$SELECT * FROM tdm_raw('2019-12-09'::TIMESTAMP, '2019-12-13'::TIMESTAMP)$$,
    83,
    factor => 1.0::FLOAT,
    dryrun => true
);

SELECT *
FROM vrp_compatibleVehiclesRaw(
    $$SELECT * FROM shipments$$,
    $$SELECT * FROM vehicles$$::TEXT,
    $$SELECT start_vid, end_vid, agg_cost FROM timeMatrix$$,
    $$SELECT * FROM tdm_raw('2019-12-09'::TIMESTAMP)$$,
    83,
    factor => 1.0::FLOAT
) ORDER BY vehicle_id;
