DROP TABLE IF EXISTS multiple_knapsack_query CASCADE;

CREATE TABLE multiple_knapsack_query(
  id INTEGER,
  weight INTEGER,
  cost INTEGER);

INSERT INTO multiple_knapsack_query (id, weight,  cost)
VALUES
(1, 48, 10),
(2, 30, 30),
(3, 42, 25),
(4, 36, 50),
(5, 36, 35),
(6, 48, 30), 
(7, 42, 15), 
(8, 42, 40),
(9, 36, 30),
(10, 24, 35), 
(11, 30, 45), 
(12, 30, 10), 
(13, 42, 20), 
(14, 36, 30), 
(15, 36, 25);

/* -- example_start */
SELECT * 
FROM multiple_knapsack_query;

SELECT * 
FROM vrp_multiple_knapsack('SELECT id, weight, cost FROM multiple_knapsack_query', ARRAY[100,100,100,100,100]);
/* -- example_end */