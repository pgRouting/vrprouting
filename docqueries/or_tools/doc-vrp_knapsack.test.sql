DROP TABLE IF EXISTS knapsack_query;
CREATE TABLE knapsack_query(
  id INTEGER,
  weight INTEGER,
  cost INTEGER);

INSERT INTO knapsack_query(id, weight, cost)
VALUES
(1, 12, 4),
(2, 2, 2),
(3, 1, 1),
(4, 4, 10),
(5, 1, 2);

/* -- example_start */
SELECT *
FROM knapsack_query;

SELECT * 
FROM vrp_knapsack($$SELECT * FROM knapsack_query$$, 15);
/* -- example_end */
