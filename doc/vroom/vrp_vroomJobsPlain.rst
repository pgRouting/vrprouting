..
   ****************************************************************************
    vrpRouting Manual
    Copyright(c) vrpRouting Contributors

    This documentation is licensed under a Creative Commons Attribution-Share
    Alike 3.0 License: https://creativecommons.org/licenses/by-sa/3.0/
   ****************************************************************************

|

* `Documentation <https://vrp.pgrouting.org/>`__ → `vrpRouting v0 <https://vrp.pgrouting.org/v0>`__
* Supported Versions
  `Latest <https://vrp.pgrouting.org/latest/en/vrp_vroomJobsPlain.html>`__
  (`v0 <https://vrp.pgrouting.org/v0/en/vrp_vroomJobsPlain.html>`__)


vrp_vroomJobsPlain - Experimental
===============================================================================

``vrp_vroomJobsPlain`` - Vehicle Routing Problem with VROOM, involving only
jobs, with plain integer values instead of TIMESTAMP or INTERVAL.

.. include:: experimental.rst
   :start-after: begin-warn-expr
   :end-before: end-warn-expr

.. rubric:: Availability

Version 0.3.0

* Function **modified** for VROOM 1.11.0

Version 0.2.0

* New **experimental** function


Description
-------------------------------------------------------------------------------

VROOM is an open-source optimization engine that aims at providing good solutions
to various real-life vehicle routing problems (VRP) within a small computing time.
This function can be used to get the solution to a problem involving only jobs.

.. index::
   single: vrp_vroomJobsPlain -- Experimental on v0.2

Signature
-------------------------------------------------------------------------------

.. admonition:: \ \
   :class: signatures

   | vrp_vroom(
   | `Jobs SQL`_, `Jobs Time Windows SQL`_,
   | `Vehicles SQL`_,
   | `Breaks SQL`_, `Breaks Time Windows SQL`_,
   | `Time Matrix SQL`_
   | [, exploration_level] [, timeout])  -- Experimental on v0.2
   | RETURNS SET OF
   | (seq, vehicle_seq, vehicle_id, vehicle_data, step_seq, step_type, task_id,
   |  task_data, arrival, travel_time, service_time, waiting_time, departure, load)

**Example**: This example is based on the VROOM Data of the :doc:`sampledata` network:

.. literalinclude:: vroomJobsPlain.queries
   :start-after: -- q1
   :end-before: -- q2

Parameters
-------------------------------------------------------------------------------

.. include:: vrp_vroomJobs.rst
   :start-after: vjobs_parameter_start
   :end-before: vjobs_parameter_end

Optional Parameters
...............................................................................

.. include:: vrp_vroomPlain.rst
   :start-after: vroom_plain_optionals_start
   :end-before: vroom_plain_optionals_end

Inner Queries
-------------------------------------------------------------------------------

Jobs SQL
...............................................................................

.. include:: concepts.rst
   :start-after: jobs_start
   :end-before: jobs_end

Jobs Time Windows SQL
...............................................................................

.. include:: concepts.rst
   :start-after: general_time_windows_start
   :end-before: general_time_windows_end

Vehicles SQL
...............................................................................

.. include:: concepts.rst
   :start-after: vroom_vehicles_start
   :end-before: vroom_vehicles_end

Breaks SQL
...............................................................................

.. include:: concepts.rst
   :start-after: breaks_start
   :end-before: breaks_end

Breaks Time Windows SQL
...............................................................................

.. include:: concepts.rst
   :start-after: general_time_windows_start
   :end-before: general_time_windows_end

Time Matrix SQL
...............................................................................

.. include:: concepts.rst
   :start-after: vroom_matrix_start
   :end-before: vroom_matrix_end

Result Columns
-------------------------------------------------------------------------------

.. include:: concepts.rst
   :start-after: vroom_result_start
   :end-before: vroom_result_end

Additional Example
-------------------------------------------------------------------------------

Problem involving 2 jobs, using a single vehicle, corresponding to the VROOM Documentation
`Example 2 <https://github.com/VROOM-Project/vroom/blob/master/docs/example_2.json>`__.

.. literalinclude:: vroomJobsPlain.queries
   :start-after: -- q2
   :end-before: -- q3

See Also
-------------------------------------------------------------------------------

* :doc:`vroom-category`
* The queries use the :doc:`sampledata` network.

.. include:: vroom-category.rst
   :start-after: see_also_start
   :end-before: see_also_end

.. rubric:: Indices and tables

* :ref:`genindex`
* :ref:`search`

.. |interval| replace:: |ANY-INTEGER|
.. |interval0| replace:: :math:`0`
.. |intervalmax| replace:: :math:`4294967295`
.. |timestamp| replace:: |ANY-INTEGER|
.. |tw_open_default| replace:: :math:`0`
.. |tw_close_default| replace:: :math:`4294967295`

