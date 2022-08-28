DROP TABLE IF EXISTS bin_packing_query CASCADE;

CREATE TABLE bin_packing_query(
  id INTEGER,
  weight INTEGER);

INSERT INTO bin_packing_query (id, weight)
VALUES
(1, 48), (2, 30), (3, 19), (4, 36), (5, 36), (6, 27), (7, 42), (8, 42), (9, 36), (10, 24), (11, 30);

/* -- example_start */
SELECT *
FROM bin_packing_query;

SELECT * 
FROM vrp_bin_packing('SELECT id, weight FROM bin_packing_query', 100);
/* -- example_end */