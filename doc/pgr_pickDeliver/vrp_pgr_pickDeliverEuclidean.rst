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
  `Latest <https://vrp.pgrouting.org/latest/en/vrp_pgr_pickDeliverEuclidean.html>`__
  (`v0 <https://vrp.pgrouting.org/v0/en/vrp_pgr_pickDeliverEuclidean.html>`__)

vrp_pgr_pickDeliverEuclidean - Experimental
===============================================================================

``vrp_pgr_pickDeliverEuclidean`` - Pickup and delivery Vehicle Routing Problem

.. include:: experimental.rst
   :start-after: begin-warn-expr
   :end-before: end-warn-expr

.. rubric:: Availability

Version 0.0.0

- New **experimental** function

  - Ported pgr_pickDeliverEuclidean from pgRouting v3.1.3


Synopsis
-------------------------------------------------------------------------------

Problem: Distribute and optimize the pickup-delivery pairs into a fleet of vehicles.

- Optimization problem is NP-hard.
- Pickup and Delivery:

  - capacitated
  - with time windows.

- The vehicles

  - have (x, y) start and ending locations.
  - have a start and ending service times.
  - have opening and closing times for the start and ending locations.

- An order is for doing a pickup and a a deliver.

  - has (x, y) pickup and delivery locations.
  - has opening and closing times for the pickup and delivery locations.
  - has a pickup and deliver service times.

- There is a customer where to deliver a pickup.

  - travel time between customers is distance / speed
  - pickup and delivery pair is done with the same vehicle.
  - A pickup is done before the delivery.



Characteristics
-------------------------------------------------------------------------------

- No multiple time windows for a location.
- Less vehicle used is considered better.
- Less total duration is better.
- Less wait time is better.
- Six different optional different initial solutions

  - the best solution found will be result


.. index::
    single: pgr_pickDeliverEuclidean - Experimental


Signature
-------------------------------------------------------------------------------

.. include:: ../sql/pgr_pickDeliver/pgr_pickDeliverEuclidean.sql
   :start-after: signature start
   :end-before: signature end


Parameters
-------------------------------------------------------------------------------

.. include:: ../sql/pgr_pickDeliver/pgr_pickDeliverEuclidean.sql
   :start-after: parameters start
   :end-before: parameters end


Inner Queries
-------------------------------------------------------------------------------


Orders SQL
................................................................................

.. include:: concepts.rst
    :start-after: pgr_orders_e_start
    :end-before: pgr_orders_e_end

Vehicles SQL
................................................................................

.. include:: concepts.rst
    :start-after: pgr_vehicles_e_start
    :end-before: pgr_vehicles_e_end


Result Columns
-------------------------------------------------------------------------------

.. include:: ../sql/pgr_pickDeliver/pgr_pickDeliverEuclidean.sql
   :start-after: result start
   :end-before: result end


Example
-------------------------------------------------------------------------------

.. TODO put link

This example use the following data:


.. literalinclude:: ./pgr_pickDeliverEuclidean.queries
   :start-after: --q1
   :end-before: --q2


.. rubric:: See Also

* :doc:`pgr-category`
* The queries use the :doc:`sampledata` network.

.. rubric:: Indices and tables

* :ref:`genindex`
* :ref:`search`

