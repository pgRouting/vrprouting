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
