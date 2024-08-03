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
  `Latest <https://vrp.pgrouting.org/latest/en/vrp_vroomShipmentsPlain.html>`__
  (`v0 <https://vrp.pgrouting.org/v0/en/vrp_vroomShipmentsPlain.html>`__)


vrp_vroomShipmentsPlain - Experimental
===============================================================================

``vrp_vroomShipmentsPlain`` - Vehicle Routing Problem with VROOM, involving only
shipments, with plain integer values instead of TIMESTAMP or INTERVAL.

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
This function can be used to get the solution to a problem involving only shipments.

.. index::
   single: vrp_vroomShipmentsPlain -- Experimental on v0.2

Signature
-------------------------------------------------------------------------------

.. rubric:: Summary

.. include:: ../sql/vroom/vrp_vroomShipmentsPlain.sql
   :start-after: signature start
   :end-before: signature end

Optional parameters are `named parameters` and have a default value.

.. rubric:: Using defaults

.. include:: ../sql/vroom/vrp_vroomShipmentsPlain.sql
   :start-after: default signature start
   :end-before: default signature end

**Example**: This example is based on the VROOM Data of the :doc:`sampledata` network:

.. literalinclude:: vroomShipmentsPlain.queries
   :start-after: -- q1
   :end-before: -- q2

Parameters
-------------------------------------------------------------------------------

.. include:: ../sql/vroom/vrp_vroomShipments.sql
   :start-after: parameters start
   :end-before: parameters end

Optional Parameters
...............................................................................

.. include:: ../sql/vroom/vrp_vroomPlain.sql
   :start-after: optional parameters start
   :end-before: optional parameters end

Inner Queries
-------------------------------------------------------------------------------

Shipments SQL
...............................................................................

.. include:: concepts.rst
   :start-after: shipments_start
   :end-before: shipments_end

Shipments Time Windows SQL
...............................................................................

.. include:: concepts.rst
   :start-after: shipments_time_windows_start
   :end-before: shipments_time_windows_end

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

Result Columns
-------------------------------------------------------------------------------

.. include:: vroom-category.rst
   :start-after: result_columns_start
   :end-before: result_columns_end

Additional Example
-------------------------------------------------------------------------------

Problem involving 1 shipment, using a single vehicle, similar to the VROOM Documentation
`Example 2 <https://github.com/VROOM-Project/vroom/blob/master/docs/example_2.json>`__ without jobs
and with a shipment.

.. literalinclude:: vroomShipmentsPlain.queries
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
