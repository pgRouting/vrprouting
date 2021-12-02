DROP SCHEMA IF EXISTS vroom CASCADE;
CREATE SCHEMA vroom;

DROP TABLE IF EXISTS vroom.jobs;
DROP TABLE IF EXISTS vroom.jobs_time_windows;
DROP TABLE IF EXISTS vroom.shipments;
DROP TABLE IF EXISTS vroom.shipments_time_windows;
DROP TABLE IF EXISTS vroom.vehicles;
DROP TABLE IF EXISTS vroom.breaks;
DROP TABLE IF EXISTS vroom.breaks_time_windows;
DROP TABLE IF EXISTS vroom.matrix;


-- JOBS TABLE start
CREATE TABLE vroom.jobs (
  id BIGSERIAL PRIMARY KEY,
  location_index BIGINT,
  service INTEGER,
  delivery BIGINT[],
  pickup BIGINT[],
  skills INTEGER[],
  priority INTEGER
);

INSERT INTO vroom.jobs (
  id, location_index, service, delivery, pickup, skills, priority)
  VALUES
(1, 1, 250, ARRAY[20], ARRAY[20], ARRAY[0], 0),
(2, 2, 250, ARRAY[30], ARRAY[30], ARRAY[0], 0),
(3, 3, 250, ARRAY[10], ARRAY[10], ARRAY[0], 0),
(4, 3, 250, ARRAY[40], ARRAY[40], ARRAY[0], 0),
(5, 4, 250, ARRAY[20], ARRAY[20], ARRAY[0], 0);
-- JOBS TABLE end


-- JOBS TIME WINDOWS TABLE start
CREATE TABLE vroom.jobs_time_windows (
  id BIGINT REFERENCES vroom.jobs(id),
  tw_open INTEGER,
  tw_close INTEGER
);

INSERT INTO vroom.jobs_time_windows (
  id, tw_open, tw_close)
  VALUES
(1, 3625, 4375),
(2, 1250, 2000),
(3, 2725, 3475),
(4, 3525, 4275),
(5, 1025, 1775);
-- JOBS TIME WINDOWS TABLE end


-- SHIPMENTS TABLE start
CREATE TABLE vroom.shipments (
  id BIGSERIAL PRIMARY KEY,
  p_location_index BIGINT,
  p_service INTEGER,
  d_location_index BIGINT,
  d_service INTEGER,
  amount BIGINT[],
  skills INTEGER[],
  priority INTEGER
);

INSERT INTO vroom.shipments (
  id, p_location_index, p_service, d_location_index, d_service,
  amount, skills, priority)
  VALUES
(1, 3, 2250, 5, 2250, ARRAY[10], ARRAY[0], 0),
(2, 5, 2250, 6, 2250, ARRAY[10], ARRAY[0], 0),
(3, 1, 2250, 2, 2250, ARRAY[20], ARRAY[0], 0),
(4, 1, 2250, 4, 2250, ARRAY[20], ARRAY[0], 0),
(5, 2, 2250, 2, 2250, ARRAY[10], ARRAY[0], 0);
-- SHIPMENTS TABLE end


-- SHIPMENTS TIME WINDOWS TABLE start
CREATE TABLE vroom.shipments_time_windows (
  id BIGINT REFERENCES vroom.shipments(id),
  kind CHAR(1),
  tw_open INTEGER,
  tw_close INTEGER
);

INSERT INTO vroom.shipments_time_windows (
  id, kind, tw_open, tw_close)
  VALUES
(1, 'p', 1625, 3650),
(1, 'd', 24925, 26700),
(2, 'p', 375, 1675),
(2, 'd', 4250, 5625),
(3, 'p', 15525, 17550),
(3, 'd', 20625, 21750),
(4, 'p', 6375, 8100),
(4, 'd', 8925, 10250),
(5, 'p', 13350, 15125),
(5, 'd', 18175, 19550);
-- SHIPMENTS TIME WINDOWS TABLE end


-- VEHICLES TABLE start
CREATE TABLE vroom.vehicles (
  id BIGSERIAL PRIMARY KEY,
  start_index BIGINT,
  end_index BIGINT,
  capacity BIGINT[],
  skills INTEGER[],
  tw_open INTEGER,
  tw_close INTEGER,
  speed_factor FLOAT
);

INSERT INTO vroom.vehicles (
  id, start_index, end_index, capacity, skills,
  tw_open, tw_close, speed_factor)
  VALUES
(1, 1, 1, ARRAY[200], ARRAY[0], 0, 30900, 1.0),
(2, 1, 3, ARRAY[200], ARRAY[0], 100, 30900, 1.0),
(3, 1, 1, ARRAY[200], ARRAY[0], 0, 30900, 1.0),
(4, 3, 3, ARRAY[200], ARRAY[0], 0, 30900, 1.0);
-- VEHICLES TABLE end


-- BREAKS TABLE start
CREATE TABLE vroom.breaks (
  id BIGINT PRIMARY KEY,
  vehicle_id BIGINT REFERENCES vroom.vehicles(id),
  service INTEGER
);

INSERT INTO vroom.breaks (
  id, vehicle_id, service)
  VALUES
(1, 1, 0),
(2, 2, 10),
(3, 3, 0),
(4, 4, 0);
-- BREAKS TABLE end


-- BREAKS TIME WINDOWS TABLE start
CREATE TABLE vroom.breaks_time_windows (
  id BIGINT REFERENCES vroom.breaks(id),
  tw_open INTEGER,
  tw_close INTEGER
);

INSERT INTO vroom.breaks_time_windows (
  id, tw_open, tw_close)
  VALUES
(1, 250, 300),
(2, 250, 275),
(3, 0, 0),
(4, 250, 250);
-- BREAKS TIME WINDOWS TABLE end


-- MATRIX TABLE start
CREATE TABLE vroom.matrix (
  start_id BIGINT,
  end_id BIGINT,
  duration INTEGER,
  cost INTEGER
);

INSERT INTO vroom.matrix (
  start_id, end_id, duration)
  VALUES
(1, 1, 0), (1, 2, 50), (1, 3, 90), (1, 4, 75), (1, 5, 106), (1, 6, 127),
(2, 1, 50), (2, 2, 0), (2, 3, 125), (2, 4, 90), (2, 5, 145), (2, 6, 127),
(3, 1, 90), (3, 2, 125), (3, 3, 0), (3, 4, 50), (3, 5, 25), (3, 6, 90),
(4, 1, 75), (4, 2, 90), (4, 3, 50), (4, 4, 0), (4, 5, 75), (4, 6, 55),
(5, 1, 106), (5, 2, 145), (5, 3, 25), (5, 4, 75), (5, 5, 0), (5, 6, 111),
(6, 1, 127), (6, 2, 127), (6, 3, 90), (6, 4, 55), (6, 5, 111), (6, 6, 0);

UPDATE vroom.matrix SET cost = duration;
-- MATRIX TABLE end
