DROP SCHEMA IF EXISTS ortools CASCADE;
CREATE SCHEMA ortools;

DROP TABLE IF EXISTS ortools.knapsack_data CASCADE;
DROP TABLE IF EXISTS ortools.multiple_knapsack_data CASCADE;
DROP TABLE IF EXISTS ortools.bin_packing_data CASCADE;

--Bin Packing Start
CREATE TABLE ortools.bin_packing_data(
  id INTEGER,
  weight INTEGER);

INSERT INTO ortools.bin_packing_data (id, weight)
VALUES
(1, 48), (2, 30), (3, 19), (4, 36), (5, 36), (6, 27), (7, 42), (8, 42), (9, 36), (10, 24), (11, 30);
--Bin Packing End

-- Multiple Knapsack Start
CREATE TABLE ortools.multiple_knapsack_data(
  id INTEGER,
  weight INTEGER,
  cost INTEGER);

INSERT INTO ortools.multiple_knapsack_data (id, weight,  cost)
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
--Multiple Knapsack End

--Knapsack Start
CREATE TABLE ortools.knapsack_data(
  id INTEGER,
  weight INTEGER,
  cost INTEGER);

INSERT INTO ortools.knapsack_data (id, weight,  cost)
VALUES
(1, 12, 4),
(2, 2, 2),
(3, 1, 1),
(4, 4, 10),
(5, 1, 2);
--Knapsack End
