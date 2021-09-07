BEGIN;

SELECT plan(15);


PREPARE No_problem_query AS
SELECT * FROM vrp_pgr_pickDeliver(
  $$ SELECT * FROM orders_1 ORDER BY id $$,
  $$ SELECT * FROM vehicles_1 ORDER BY id$$,
  $$ SELECT * FROM edges_matrix$$);

SELECT lives_ok('No_problem_query', 'Should live: '|| 1);
SELECT lives_ok('No_problem_query', 'Should live: '|| 2);
SELECT lives_ok('No_problem_query', 'Should live: '|| 3);
SELECT lives_ok('No_problem_query', 'Should live: '|| 4);
SELECT lives_ok('No_problem_query', 'Should live: '|| 5);
SELECT lives_ok('No_problem_query', 'Should live: '|| 6);
SELECT lives_ok('No_problem_query', 'Should live: '|| 7);
SELECT lives_ok('No_problem_query', 'Should live: '|| 8);
SELECT lives_ok('No_problem_query', 'Should live: '|| 9);
SELECT lives_ok('No_problem_query', 'Should live: '|| 10);
SELECT lives_ok('No_problem_query', 'Should live: '|| 11);
SELECT lives_ok('No_problem_query', 'Should live: '|| 12);
SELECT lives_ok('No_problem_query', 'Should live: '|| 13);
SELECT lives_ok('No_problem_query', 'Should live: '|| 14);
SELECT lives_ok('No_problem_query', 'Should live: '|| 15);


SELECT finish();
ROLLBACK;
