SET extra_float_digits=-3;
/* --q1 */
SELECT * FROM vrp_pgr_pickDeliverEuclidean(
    'SELECT * FROM orders_1 ORDER BY id',
    'SELECT * from vehicles_1'
);
/* --q2 */
