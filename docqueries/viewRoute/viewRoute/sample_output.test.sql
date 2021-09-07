SET search_path TO 'example2', 'public';

UPDATE vehicles
    SET stops = NULL;

SELECT * FROM shipments
WHERE id IN (73, 93, 203, 219, 302, 425, 505, 507, 508)
ORDER BY id;

/*******
Creating a fix initial solution
*******/
UPDATE vehicles AS v
SET stops = ARRAY[302, 73, 302, 73]
WHERE v.id = 580;

UPDATE vehicles AS v
SET stops = ARRAY[425,425,505,507,508,505,507,508,93,219,219,203,203,93]
WHERE v.id = 585;

SELECT id, stops
FROM vehicles
WHERE id IN (580, 699, 585)
ORDER BY id;

SELECT *, id_to_geom(stop_id) AS location
FROM vrp_viewRoute(
    $$SELECT * FROM shipments$$::TEXT,
    $$SELECT * FROM vehicles$$::TEXT,
    $$SELECT start_vid, end_vid, travel_time FROM timeMatrix$$::TEXT,
    $$SELECT * FROM tdm('2019-12-09'::TIMESTAMP, '2019-12-13'::TIMESTAMP)$$::TEXT,
    580::BIGINT, factor => 1.0::float);


SELECT *, id_to_geom(stop_id) AS location
FROM vrp_viewRoute(
    $$SELECT * FROM shipments$$,
    $$SELECT * FROM vehicles$$,
    $$SELECT start_vid, end_vid, travel_time FROM timeMatrix$$,
    $$SELECT * FROM tdm('2019-12-09'::TIMESTAMP, '2019-12-13'::TIMESTAMP)$$,
    699::BIGINT, factor => 1.0::float);

SELECT *, id_to_geom(stop_id) AS location
FROM vrp_viewRoute(
    $$SELECT * FROM shipments$$,
    $$SELECT * FROM vehicles$$,
    $$SELECT start_vid, end_vid, travel_time FROM timeMatrix$$,
    $$SELECT * FROM tdm('2019-12-09'::TIMESTAMP, '2019-12-13'::TIMESTAMP)$$,
    585::BIGINT, factor => 1.0::float);
