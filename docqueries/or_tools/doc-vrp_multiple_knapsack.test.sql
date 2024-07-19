/* -- example_start */
SELECT *
FROM vrp_multiple_knapsack($$SELECT id, weight, cost FROM ortools.multiple_knapsack$$, ARRAY[100,100,100,100,100]);
/* -- example_end */
