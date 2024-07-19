
DROP SCHEMA IF EXISTS ortools CASCADE;
CREATE SCHEMA ortools;

CREATE TABLE ortools.bin_packing(
  id INTEGER,
  weight INTEGER);

INSERT INTO ortools.bin_packing(id, weight)
VALUES
(1, 48), (2, 30), (3, 19), (4, 36), (5, 36), (6, 27), (7, 42), (8, 42), (9, 36), (10, 24), (11, 30);


CREATE TABLE ortools.knapsack(
  id INTEGER,
  weight INTEGER,
  cost INTEGER);

INSERT INTO ortools.knapsack(id, weight, cost)
VALUES
(1, 12, 4),
(2, 2, 2),
(3, 1, 1),
(4, 4, 10),
(5, 1, 2);

CREATE TABLE ortools.multiple_knapsack(
  id INTEGER,
  weight INTEGER,
  cost INTEGER);

INSERT INTO ortools.multiple_knapsack(id, weight, cost)
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
