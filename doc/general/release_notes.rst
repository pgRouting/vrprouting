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
  `Latest <https://vrp.pgrouting.org/latest/en/release_notes.html>`__
  (`v0 <https://vrp.pgrouting.org/v0/en/release_notes.html>`__)

Release Notes
-------------------------------------------------------------------------------

To see the full list of changes check the list of `Git commits <https://github.com/pgRouting/vrprouting/commits>`_ on Github.

.. contents:: Contents
     :local:

v0.3.0 Release Notes
*******************************************************************************

To see all issues & pull requests closed by this release see the `Git closed milestone for 0.3.0 <https://github.com/pgRouting/vrprouting/issues?utf8=%E2%9C%93&q=milestone%3A%22Release%200.3.0%22>`_ on Github.

.. rubric:: Modification of experimental functions

- VROOM

  - vrp_vroom
  - vrp_vroomJobs
  - vrp_vroomShipments
  - vrp_vroomPlain
  - vrp_vroomJobsPlain
  - vrp_vroomShipmentsPlain

.. rubric:: Added support for VROOM 1.11.0 (`#24 <https://github.com/pgRouting/vrprouting/issues/24>`_)

- Added setup time in jobs and shipments to refine service time modeling.
- Added support for custom cost matrices, along with the duration matrix.
  - Using start_id, end_id, duration, cost as matrix table columns.
- Added timeout and exploration_level parameters to vroom-category functions.
- Added max_tasks column in vehicles.
- Added tests for empty skills arrays.
- Added custom scaling logic for speed_factor.
- Modified parameter names to make the naming consistent.

.. rubric:: Fixes

- Honor client cancel requests for vroom-category functions.
- Added more information in the inner query and result columns of VROOM category functions
  (`#26 <https://github.com/pgRouting/vrprouting/issues/26>`_, `#27 <https://github.com/pgRouting/vrprouting/issues/27>`_):

  - Summary row in the output, for each vehicle and for the complete problem.
  - Uassigned rows in the output with vehicle_id = -1.
  - Modified travel_time result column to return travel time between current and last step.
  - Added data jsonb field in jobs, shipments, vehicles, breaks as well as in the result columns.
  - Added departure field and location_id field in the result columns.

v0.2.0 Release Notes
*******************************************************************************

.. rubric:: New experimental functions

- VROOM

  - vrp_vroom
  - vrp_vroomJobs
  - vrp_vroomShipments
  - vrp_vroomPlain
  - vrp_vroomJobsPlain
  - vrp_vroomShipmentsPlain

- VRP

  - vrp_compatibleVehicles
  - vrp_optimize
  - vrp_pickDeliverAdd
  - vrp_pickDeliver
  - vrp_simulation
  - vrp_viewRoute

v0.1.0 Release Notes
*******************************************************************************

.. rubric:: Extraction tasks

- Porting pgRouting's VRP functionality

.. rubric:: New official functions

* vrp_version
* vrp_full_version
