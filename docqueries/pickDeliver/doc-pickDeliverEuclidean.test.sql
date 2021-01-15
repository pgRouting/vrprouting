SET extra_float_digits=-3;
\echo --q1
SELECT * FROM vrp_pickDeliverEuclidean(
    'SELECT * FROM orders ORDER BY id',
    'SELECT * from vehicles'
);
\echo --q2
