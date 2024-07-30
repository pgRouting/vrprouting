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
  `Latest <https://vrp.pgrouting.org/latest/en/concepts.html>`__
  (`v0 <https://vrp.pgrouting.org/v0/en/concepts.html>`__)

Concepts
===============================================================================

.. contents::

General documentation
-------------------------------------------------------------------------------

Vehicle Routing Problems `VRP` are **NP-hard** optimization problem, it
generalises the travelling salesman problem (TSP).

- The objective of the VRP is to minimize the total route cost.
- There are several variants of the VRP problem,

**vrpRouting does not try to implement all variants.**


Characteristics
-------------------------------------------------------------------------------

- Capacitated Vehicle Routing Problem `CVRP` where The vehicles have limited
  carrying capacity of the goods.
- Vehicle Routing Problem with Time Windows `VRPTW` where the locations have
  time windows within which the vehicle's visits must be made.
- Vehicle Routing Problem with Pickup and Delivery `VRPPD` where a number of
  goods need to be moved from certain pickup locations to other delivery
  locations.


.. Rubric:: Limitations

- No multiple time windows for a location.
- Less vehicle used is considered better.
- Less total duration is better.
- Less wait time is better.


Pick & Delivery
...............................................................................

Problem: `CVRPPDTW` Capacitated Pick and Delivery Vehicle Routing problem with
Time Windows

- Times are relative to `0`
- The vehicles

  - have start and ending service duration times.
  - have opening and closing times for the start and ending locations.
  - have a capacity.

- The orders

  - Have pick up and delivery locations.
  - Have opening and closing times for the pickup and delivery locations.
  - Have pickup and delivery duration service times.
  - have a demand request for moving goods from the pickup location to the
    delivery location.

- Time based calculations:

  - Travel time between customers is :math:`distance / speed`
  - Pickup and delivery order pair is done by the same vehicle.
  - A pickup is done before the delivery.

.. note To be defined



.. _Getting_started:

Getting Started
-------------------------------------------------------------------------------

This is a simple guide to walk you through the steps of getting started
with vrpRouting. In this guide we will cover:

.. contents::
    :local:


.. _create_database:

Create a routing Database
...............................................................................

The first thing we need to do is create a database and load pgrouting in
the database. Typically you will create a database for each project. Once
you have a database to work in, your can load your data and build your
application in that database. This makes it easy to move your project
later if you want to to say a production server.

For Postgresql 9.2 and later versions

.. code-block:: bash

	createdb mydatabase
	psql mydatabase -c "create extension postgis"
	psql mydatabase -c "create extension pgrouting"


Handling Parameters
*******************************************************************************

To define a problem, several considerations have to be done, to get consistent results.
This section gives an insight of how parameters are to be considered.

- `Capacity and Demand Units Handling`_
- `Locations`_
- `Time Handling`_
- `Factor Handling`_


Capacity and Demand Units Handling
...............................................................................

The `capacity` of a vehicle, can be measured in:

- Volume units like :math:`m^3`.
- Area units like :math:`m^2` (when no stacking is allowed).
- Weight units like :math:`kg`.
- Number of boxes that fit in the vehicle.
- Number of seats in the vehicle

The `demand` request of the pickup-deliver orders must use the same units as the
units used in the vehicle's `capacity`.

To handle problems like:  10 (equal dimension) boxes of apples and 5 kg of
feathers that are to be transported (not packed in boxes).

If the vehicle's `capacity` is measured by `boxes`, a conversion of `kg of
feathers` to `equivalent number of boxes` is needed.
If the vehicle's `capacity` is measured by `kg`, a conversion of `box of apples`
to `equivalent number of kg` is needed.

Showing how the 2 possible conversions can be done

Let:
- :math:`f_boxes`: number of boxes that would be used for `1` kg of feathers.
- :math:`a_weight`: weight of `1` box of apples.

=============== ====================== ==================
Capacity Units  apples                  feathers
=============== ====================== ==================
boxes            10                     :math:`5 * f\_boxes`
kg              :math:`10 * a\_weight`       5
=============== ====================== ==================



Locations
*******************************************************************************

- When using the Euclidean signatures:

  - The vehicles have :math:`(x, y)` pairs for start and ending locations.
  - The orders Have :math:`(x, y)` pairs for pickup and delivery locations.

- When using a matrix:

  - The vehicles have identifiers for the start and ending locations.
  - The orders have identifiers for the pickup and delivery locations.
  - All the identifiers are indices to the given matrix.


Time Handling
...............................................................................

The times are relative to 0

Suppose that a vehicle's driver starts the shift at 9:00 am and ends the shift at 4:30 pm
and the service time duration is 10 minutes with 30 seconds.

All time units have to be converted

============ ================= ==================== ===================== =========
Meaning of 0   time units       9:00 am              4:30 pm               10 min 30 secs
============ ================= ==================== ===================== =========
0:00 am         hours            9                  16.5                   :math:`10.5 / 60  = 0.175`
9:00 am         hours            0                  7.5                    :math:`10.5 / 60  = 0.175`
0:00 am         minutes          :math:`9*60 = 54`  :math:`16.5*60 = 990`  10.5
9:00 am         minutes          0                  :math:`7.5*60 = 540`   10.5
============ ================= ==================== ===================== =========


.. _pd_factor:

Factor Handling
*******************************************************************************

.. TODO

TBD


Group of Functions
-------------------------------------------------------------------------------

TBD



Inner Queries
-------------------------------------------------------------------------------

Vroom Inner Queries
...............................................................................

Jobs SQL
*******************************************************************************

.. jobs_start

A ``SELECT`` statement that returns the following columns:

| ``id, location_id``
| ``[setup, service, delivery, pickup, skills, priority, data]``

Maximum values apply from vroom

``setup`` and ``service``

- |intervalmax|

``skills``

- :math:`2147483647`

``priority``

- :math:`100`

.. list-table::
   :width: 81
   :widths: auto
   :header-rows: 1

   - - Column
     - Type
     - Default
     - Description
   - - ``id``
     - |ANY-INTEGER|
     -
     - Positive unique identifier of the job.
   - - ``location_id``
     - |ANY-INTEGER|
     -
     - Positive unique identifier of the location of the job.
   - - ``setup``
     - |interval|
     - |interval0|
     - The Job setup duration.

   - - ``service``
     - |interval|
     - |interval0|
     - The Job service duration. Max value:
   - - ``pickup``
     - ``ARRAY[ANY-INTEGER]``
     - ``[]``
     - Array of non-negative integers describing multidimensional quantities for
       pickup such as number of items, weight, volume etc.

       - All jobs must have the same value of :code:`array_length(pickup, 1)`
   - - ``delivery``
     - ``ARRAY[ANY-INTEGER]``
     - ``[]``
     - Array of non-negative integers describing multidimensional quantities for
       delivery such as number of items, weight, volume etc.

       - All jobs must have the same value of :code:`array_length(delivery, 1)`
   - - ``skills``
     - ``ARRAY[ANY-INTEGER]``
     - ``[]``
     - Array of non-negative integers defining mandatory skills.
   - - ``priority``
     - ``INTEGER``
     - :math:`0`
     - Value range: :math:`[0, 100]`
   - - ``data``
     - ``JSONB``
     - ``'{}'::JSONB``
     - Any metadata information of the job.

.. jobs_end


Return columns & values
--------------------------------------------------------------------------------

TBD


Performance
-------------------------------------------------------------------------------

TBD

How to contribute
-------------------------------------------------------------------------------

.. rubric:: Wiki

* Edit an existing  `vrpRouting Wiki <https://github.com/pgRouting/vrprouting/wiki>`_ page.


.. rubric:: Adding Functionaity to vrpRouting

Consult the `developer's documentation <https://vrp.pgrouting.org/doxy/main/index.html>`_


.. rubric:: Indices and tables

* :ref:`genindex`
* :ref:`search`

.. |interval| replace:: :abbr:`ANY-INTERVAL(INTERVAL, SMALLINT, INTEGER, BIGINT)`
.. |interval0| replace:: :abbr:`INTERVAL 0('make_interval(secs => 0), 0)`
.. |intervalmax| replace:: **INTERVAL**: ``make_interval(secs => 4294967295)`` and |br| |ANY-INTEGER|: :math:`4294967295`
.. |timestamp| replace:: :abbr:`ANY-TIMESTAMP(TIMESTAMP, SMALLINT, INTEGER, BIGINT)`
.. |tw_open_default| replace:: :abbr:`TW-OPEN-DEFAULT(to_timestamp(0), 0)`
.. |tw_close_default| replace:: :abbr:`TW-CLOSE-DEFAULT(to_timestamp(4294967295), 4294967295)`
