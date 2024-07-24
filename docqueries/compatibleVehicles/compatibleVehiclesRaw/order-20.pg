SET search_path TO 'example2', 'public';

UPDATE vehicles SET stops = ARRAY[20,20];

SELECT *
FROM vrp_compatibleVehiclesRaw(
    $$SELECT * FROM shipments$$,
    $$SELECT * FROM vehicles$$,
    $$SELECT start_vid, end_vid, agg_cost FROM timeMatrix$$,
    $$SELECT * FROM tdm_raw('2019-12-09'::TIMESTAMP, '2019-12-13'::TIMESTAMP)$$,
    20, factor => 1);
