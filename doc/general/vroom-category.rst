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
  `Latest <https://vrp.pgrouting.org/latest/en/vroom-category.html>`__
  (`v0 <https://vrp.pgrouting.org/v0/en/vroom-category.html>`__)


VROOM - Category (Experimental)
===============================================================================

.. include:: experimental.rst
   :start-after: begin-warn-expr
   :end-before: end-warn-expr


.. contents::

.. rubric:: Functions

.. toctree::
  :maxdepth: 1

  vrp_vroom
  vrp_vroomJobs
  vrp_vroomShipments
  vrp_vroomPlain
  vrp_vroomJobsPlain
  vrp_vroomShipmentsPlain


Synopsis
-------------------------------------------------------------------------------

VROOM is an open-source optimization engine that aims at providing good solutions
to various real-life vehicle routing problems (VRP) within a small computing time.

VROOM can solve several well-known types of vehicle routing problems (VRP).

- TSP (travelling salesman problem)
- CVRP (capacitated VRP)
- VRPTW (VRP with time windows)
- MDHVRPTW (multi-depot heterogeneous vehicle VRPTW)
- PDPTW (pickup-and-delivery problem with TW)

VROOM can also solve any mix of the above problem types.


Characteristics
...............................................................................

VROOM models a Vehicle Routing Problem with ``vehicles``, ``jobs`` and ``shipments``.

The **vehicles** denote the resources that pick and/or deliver the jobs and shipments.
They are characterised by:

- Capacity on arbitrary number of metrics
- Skills
- Working hours
- Driver breaks
- Start and end defined on a per-vehicle basis
- Start and end can be different
- Open trip optimization (only start or only end defined)

The **jobs** denote the single-location pickup and/or delivery tasks, and the **shipments**
denote the pickup-and-delivery tasks that should happen within the same route.
They are characterised by:

- Delivery/pickup amounts on arbitrary number of metrics
- Service time windows
- Service duration
- Skills
- Priority

Terminologies
...............................................................................

- **Tasks**: Either jobs or shipments are referred to as tasks.
- **Skills**: Every task and vehicle may have some set of skills. A task can be
  served by only that vehicle which has all the skills of the task.
- **Priority**: Tasks may have some priority assigned, which is useful when all
  tasks cannot be performed due to constraints, so the tasks with low priority
  are left unassigned.
- **Amount (for shipment), Pickup and delivery (for job)**: They denote the
  multidimensional quantities such as number of items, weights, volume, etc.
- **Capacity (for vehicle)**: Every vehicle may have some capacity, denoting the
  multidimensional quantities. A vehicle can serve only those sets of tasks such
  that the total sum of the quantity does not exceed the vehicle capacity, at
  any point of the route.
- **Time Window**: An interval of time during which some activity can be
  performed, such as working hours of the vehicle, break of the vehicle, or
  service start time for a task.
- **Break**: Array of time windows, denoting valid slots for the break start of
  a vehicle.
- **Setup time**: Setup times serve as a mean to describe the time it takes to
  get started for a task at a given location.
  This models a duration that should not be re-applied for other tasks following
  at the same place.
  So the total "action time" for a task is ``setup + service`` upon arriving at
  a new location or ``service`` only if performing a new task at the previous
  vehicle location.
- **Service time**: The additional time to be spent by a vehicle while serving a
  task.
- **Travel time**: The total time the vehicle travels during its route.
- **Waiting time**: The total time the vehicle is idle, i.e. it is neither
  traveling nor servicing any task. It is generally the time spent by a vehicle
  waiting for a task service to open.

Inner Queries
-------------------------------------------------------------------------------

Jobs SQL
...............................................................................

.. include:: concepts.rst
   :start-after: jobs_start
   :end-before: jobs_end

Shipments SQL
...............................................................................

.. include:: concepts.rst
   :start-after: shipments_start
   :end-before: shipments_end

.. inner_queries_start

Vehicles SQL
...............................................................................

.. include:: ../../src/common/vroom/vehicles_input.c
   :start-after: vrp_vroom start
   :end-before: vrp_vroom end

Breaks SQL
...............................................................................

.. include:: ../../src/common/vroom/breaks_input.c
   :start-after: vrp_vroom start
   :end-before: vrp_vroom end

Time Windows SQL
...............................................................................

.. include:: ../../src/common/vroom/time_windows_input.c
   :start-after: vrp_vroom start
   :end-before: vrp_vroom end

Time Matrix SQL
...............................................................................

.. include:: ../../src/common/vroom/matrix_input.c
   :start-after: vrp_vroom start
   :end-before: vrp_vroom end

.. inner_queries_end

Result Columns
-------------------------------------------------------------------------------

.. result_columns_start

.. include:: ../sql/vroom/vrp_vroom.sql
   :start-after: result start
   :end-before: result end

.. result_columns_end

See Also
-------------------------------------------------------------------------------

.. see_also_start

* `Wikipedia: Vehicle Routing problems <https://en.wikipedia.org/wiki/Vehicle_routing_problem>`__
* `VROOM: Vehicle Routing Open-source Optimization Machine <https://github.com/VROOM-Project/vroom>`__
* `VROOM API Documentation <https://github.com/VROOM-Project/vroom/blob/master/docs/API.md>`__

.. see_also_end

.. rubric:: Indices and tables

* :ref:`genindex`
* :ref:`search`

.. |interval| replace:: :abbr:`ANY-INTERVAL(INTERVAL, SMALLINT, INTEGER, BIGINT)`
.. |interval0| replace:: :abbr:`INTERVAL 0('make_interval(secs => 0), 0)`
.. |intervalmax| replace:: **INTERVAL**: ``make_interval(secs => 4294967295)`` and |br| |ANY-INTEGER|: :math:`4294967295`
.. |timestamp| replace:: :abbr:`ANY-TIMESTAMP(TIMESTAMP, SMALLINT, INTEGER, BIGINT)`
.. |tw_open_default| replace:: :abbr:`TW-OPEN-DEFAULT(to_timestamp(0), 0)`
.. |tw_close_default| replace:: :abbr:`TW-CLOSE-DEFAULT(to_timestamp(4294967295), 4294967295)`
