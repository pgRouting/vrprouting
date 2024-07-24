SET search_path TO 'example2', 'public';

SELECT * FROM _vrp_vehiclesAtTime(
    $$SELECT * FROM vehicles$$,
    '2019-12-09 15:15:00'::TIMESTAMP,
    dryrun => true
);


SELECT * FROM _vrp_vehiclesAtTime(
    $$SELECT * FROM vehicles$$,
    '2019-12-09 15:15:00'::TIMESTAMP,
    dryrun => false
);
