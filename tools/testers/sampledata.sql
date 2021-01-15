\set QUIET 1

SET client_min_messages = WARNING;


------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
--              SAMPLE DATA
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------

DROP TABLE IF EXISTS edge_table;
DROP TABLE IF EXISTS edge_table_vertices_pgr;
DROP TABLE IF EXISTS vehicles;
DROP TABLE IF EXISTS orders;

--EDGE TABLE CREATE start
CREATE TABLE edge_table (
    id BIGSERIAL,
    source BIGINT,
    target BIGINT,
    cost FLOAT,
    reverse_cost FLOAT,
    x1 FLOAT,
    y1 FLOAT,
    x2 FLOAT,
    y2 FLOAT,
    geom geometry
);
--EDGE TABLE CREATE end
--EDGE TABLE ADD DATA start
INSERT INTO edge_table (
    cost, reverse_cost,
    x1, y1,
    x2, y2) VALUES
( 1,  1,    2,   0,    2,   1),
(-1,  1,    2,   1,    3,   1),
(-1,  1,    3,   1,    4,   1),
( 1,  1,    2,   1,    2,   2),
( 1, -1,    3,   1,    3,   2),
( 1,  1,    0,   2,    1,   2),
( 1,  1,    1,   2,    2,   2),
( 1,  1,    2,   2,    3,   2),
( 1,  1,    3,   2,    4,   2),
( 1,  1,    2,   2,    2,   3),
( 1, -1,    3,   2,    3,   3),
( 1, -1,    2,   3,    3,   3),
( 1, -1,    3,   3,    4,   3),
( 1,  1,    2,   3,    2,   4),
( 1,  1,    4,   2,    4,   3),
( 1,  1,    4,   1,    4,   2),
( 1,  1,    0.5, 3.5,  1.999999999999, 3.5),
( 1,  1,    3.5, 2.3,  3.5, 4);
--EDGE TABLE ADD DATA end

--EDGE TABLE update geometry start

UPDATE edge_table SET geom = st_makeline(st_point(x1,y1),st_point(x2,y2));

--EDGE TABLE update geometry end

--EDGE TABLE TOPOLOGY start
SELECT pgr_createTopology('edge_table',0.001, the_geom =>'geom');
--EDGE TABLE TOPOLOGY end


--VEHICLES TABLE start

CREATE TABLE vehicles (
      id BIGSERIAL PRIMARY KEY,
      start_node_id BIGINT,
      start_x FLOAT,
      start_y FLOAT,
      start_open FLOAT,
      start_close FLOAT,
      capacity FLOAT
);

INSERT INTO vehicles
(start_node_id, start_x,  start_y,  start_open,  start_close,  capacity) VALUES
(            6,       3,        2,           0,           50,        50),
(            6,       3,        2,           0,           50,        50);

--VEHICLES TABLE end



--ORDERS TABLE start
CREATE TABLE orders (
    id BIGSERIAL PRIMARY KEY,
    demand FLOAT,
    -- the pickups
    p_node_id BIGINT,
    p_x FLOAT,
    p_y FLOAT,
    p_open FLOAT,
    p_close FLOAT,
    p_service FLOAT,
    -- the deliveries
    d_node_id BIGINT,
    d_x FLOAT,
    d_y FLOAT,
    d_open FLOAT,
    d_close FLOAT,
    d_service FLOAT
);


INSERT INTO orders
(demand,
    p_node_id,  p_x, p_y,  p_open,  p_close,  p_service,
    d_node_id,  d_x, d_y,  d_open,  d_close,  d_service) VALUES
(10,
            3,    3,   1,      2,         10,          3,
            8,    1,   2,      6,         15,          3),
(20,
            9,    4,   2,      4,         15,          2,
            4,    4,   1,      6,         20,          3),
(30,
            5,    2,   2,      2,         10,          3,
           11,    3,   3,      3,         20,          3);


--ORDERS TABLE end
