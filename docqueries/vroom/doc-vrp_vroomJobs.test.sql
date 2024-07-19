SET TIMEZONE TO 'UTC';
SELECT (to_timestamp(1630573200) at time zone 'UTC')::TIMESTAMP;

/* -- q0 */
ALTER TABLE vroom.jobs ALTER COLUMN service TYPE INTERVAL USING make_interval(secs => service);
ALTER TABLE vroom.shipments ALTER COLUMN p_service TYPE INTERVAL USING make_interval(secs => p_service);
ALTER TABLE vroom.shipments ALTER COLUMN d_service TYPE INTERVAL USING make_interval(secs => d_service);
ALTER TABLE vroom.vehicles ALTER COLUMN tw_open TYPE TIMESTAMP USING (to_timestamp(tw_open + 1630573200) at time zone 'UTC')::TIMESTAMP;
ALTER TABLE vroom.vehicles ALTER COLUMN tw_close TYPE TIMESTAMP USING (to_timestamp(tw_close + 1630573200) at time zone 'UTC')::TIMESTAMP;
ALTER TABLE vroom.breaks ALTER COLUMN service TYPE INTERVAL USING make_interval(secs => service);
ALTER TABLE vroom.jobs_time_windows ALTER COLUMN tw_open TYPE TIMESTAMP USING (to_timestamp(tw_open + 1630573200) at time zone 'UTC')::TIMESTAMP;
ALTER TABLE vroom.jobs_time_windows ALTER COLUMN tw_close TYPE TIMESTAMP USING (to_timestamp(tw_close + 1630573200) at time zone 'UTC')::TIMESTAMP;
ALTER TABLE vroom.shipments_time_windows ALTER COLUMN tw_open TYPE TIMESTAMP USING (to_timestamp(tw_open + 1630573200) at time zone 'UTC')::TIMESTAMP;
ALTER TABLE vroom.shipments_time_windows ALTER COLUMN tw_close TYPE TIMESTAMP USING (to_timestamp(tw_close + 1630573200) at time zone 'UTC')::TIMESTAMP;
ALTER TABLE vroom.breaks_time_windows ALTER COLUMN tw_open TYPE TIMESTAMP USING (to_timestamp(tw_open + 1630573200) at time zone 'UTC')::TIMESTAMP;
ALTER TABLE vroom.breaks_time_windows ALTER COLUMN tw_close TYPE TIMESTAMP USING (to_timestamp(tw_close + 1630573200) at time zone 'UTC')::TIMESTAMP;
ALTER TABLE vroom.matrix ALTER COLUMN duration TYPE INTERVAL USING make_interval(secs => duration);
/* -- q1 */
SELECT *
FROM vrp_vroomJobs(
  'SELECT * FROM vroom.jobs',
  'SELECT * FROM vroom.jobs_time_windows',
  'SELECT * FROM vroom.vehicles',
  'SELECT * FROM vroom.breaks',
  'SELECT * FROM vroom.breaks_time_windows',
  'SELECT * FROM vroom.matrix'
);
/* -- q2 */
SELECT *
FROM vrp_vroomJobs(
  $jobs$
    SELECT * FROM (
      VALUES (1414, 2), (1515, 3)
    ) AS C(id, location_id)
  $jobs$,
  NULL,
  $vehicles$
    SELECT * FROM (
      VALUES (1, 1, 4)
    ) AS C(id, start_id, end_id)
  $vehicles$,
  NULL,
  NULL,
  $matrix$
    SELECT start_id, end_id, make_interval(secs => duration) AS duration FROM (
      VALUES (1, 2, 2104), (1, 3, 197), (1, 4, 1299),
             (2, 1, 2103), (2, 3, 2255), (2, 4, 3152),
             (3, 1, 197), (3, 2, 2256), (3, 4, 1102),
             (4, 1, 1299), (4, 2, 3153), (4, 3, 1102)
    ) AS C(start_id, end_id, duration)
  $matrix$
);
/* -- q3 */
