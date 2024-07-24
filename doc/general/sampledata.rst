..
   ****************************************************************************
    vrpRouting Manual
    Copyright(c) vrpRouting Contributors

    This documentation is licensed under a Creative Commons Attribution-Share
    Alike 3.0 License: https://creativecommons.org/licenses/by-sa/3.0/
   ****************************************************************************

Sample Data
===============================================================================

The documentation provides very simple example queries based on a small sample network.
To be able to execute the sample queries, run the following SQL commands to create a table with a small network data set.


City Network
-------------------------------------------------------------------------------

.. rubric:: Create table

.. literalinclude:: ../../tools/testers/sampledata.sql
   :start-after: --EDGE TABLE CREATE start
   :end-before: --EDGE TABLE CREATE end


.. rubric:: Populate

.. literalinclude:: ../../tools/testers/sampledata.sql
   :start-after: --EDGE TABLE ADD DATA start
   :end-before: --EDGE TABLE ADD DATA end


.. rubric:: Update geometry

.. literalinclude:: ../../tools/testers/sampledata.sql
   :start-after: --EDGE TABLE update geometry start
   :end-before: --EDGE TABLE update geometry end


.. rubric:: Add Topology

.. literalinclude:: ../../tools/testers/sampledata.sql
   :start-after: --EDGE TABLE TOPOLOGY start
   :end-before: --EDGE TABLE TOPOLOGY end

`pgr_PickDeliver` data
-------------------------------------------------------------------------------

Vehicles table
...............................................................................

.. literalinclude:: ../../tools/testers/sampledata.sql
   :start-after: --VEHICLES TABLE start
   :end-before: --VEHICLES TABLE end

Orders table
...............................................................................


.. literalinclude:: ../../tools/testers/sampledata.sql
   :start-after: --ORDERS TABLE start
   :end-before: --ORDERS TABLE end

`vrp_oneDepot` data
-------------------------------------------------------------------------------

.. literalinclude:: ../../tools/testers/solomon_100_rc101.data.sql

VROOM Data
-------------------------------------------------------------------------------

Jobs
...............................................................................

.. literalinclude:: ../../tools/testers/vroomdata.sql
   :start-after: -- JOBS TABLE start
   :end-before: -- JOBS TABLE end

Jobs Time Windows
...............................................................................

.. literalinclude:: ../../tools/testers/vroomdata.sql
   :start-after: -- JOBS TIME WINDOWS TABLE start
   :end-before: -- JOBS TIME WINDOWS TABLE end

Shipments
...............................................................................

.. literalinclude:: ../../tools/testers/vroomdata.sql
   :start-after: -- SHIPMENTS TABLE start
   :end-before: -- SHIPMENTS TABLE end

Shipments Time Windows
...............................................................................

.. literalinclude:: ../../tools/testers/vroomdata.sql
   :start-after: -- SHIPMENTS TIME WINDOWS TABLE start
   :end-before: -- SHIPMENTS TIME WINDOWS TABLE end

Vehicles
...............................................................................

.. literalinclude:: ../../tools/testers/vroomdata.sql
   :start-after: -- VEHICLES TABLE start
   :end-before: -- VEHICLES TABLE end

Breaks
...............................................................................

.. literalinclude:: ../../tools/testers/vroomdata.sql
   :start-after: -- BREAKS TABLE start
   :end-before: -- BREAKS TABLE end

Breaks Time Windows
...............................................................................

.. literalinclude:: ../../tools/testers/vroomdata.sql
   :start-after: -- BREAKS TIME WINDOWS TABLE start
   :end-before: -- BREAKS TIME WINDOWS TABLE end

Matrix
...............................................................................

.. literalinclude:: ../../tools/testers/vroomdata.sql
   :start-after: -- MATRIX TABLE start
   :end-before: -- MATRIX TABLE end

Modified VROOM Data
-------------------------------------------------------------------------------

The tables created using the above VROOM Data are modified for the VROOM functions
with timestamps/interval, as:

.. literalinclude:: ../../docqueries/vroom/vroom.pg
   :start-after: -- q0
   :end-before: -- q1

Images
-------------------------------------------------------------------------------

* Red arrows correspond when ``cost`` > 0 in the edge table.
* Blue arrows correspond when ``reverse_cost`` > 0 in the edge table.
* Points are outside the graph.
* Click on the graph to enlarge.

.. _fig1:

Currently VRP functions work on an undirected graph

.. figure:: /images/Fig6-undirected.png

