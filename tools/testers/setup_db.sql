\set QUIET 1

-- Tests for pgroutng.

-- Format the output for nice TAP.
\pset format unaligned
\pset tuples_only true
\pset pager

-- Revert all changes on failure.
\set ON_ERROR_ROLLBACK 1
\set ON_ERROR_STOP true

SET client_min_messages = WARNING;

CREATE EXTENSION IF NOT EXISTS pgtap;
CREATE EXTENSION IF NOT EXISTS vrprouting CASCADE;

BEGIN;

    \i sampledata.sql
    \i matrix_new_values.sql
    \i vrppdtw_data.sql
    \i solomon_100_rc101.data.sql
    \i no_crash_test.sql
    \i general_pgtap_tests.sql
    \i vroomdata.sql

END;
