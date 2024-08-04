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
  `Latest <https://vrp.pgrouting.org/latest/en/vrp_vroomPlain.html>`__
  (`v0 <https://vrp.pgrouting.org/v0/en/vrp_vroomPlain.html>`__)


vrp_vroomPlain - Experimental
===============================================================================

``vrp_vroomPlain`` - Vehicle Routing Problem with VROOM, involving both jobs and
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
This function can be used to get the solution to a problem involving both jobs and
shipments.

.. index::
   single: vrp_vroomPlain -- Experimental on v0.2

Signature
-------------------------------------------------------------------------------

.. admonition:: \ \
   :class: signatures

   | vrp_vroom(
   | `Jobs SQL`_, `Jobs Time Windows SQL`_,
   | `Shipments SQL`_, `Shipments Time Windows SQL`_,
   | `Vehicles SQL`_,
   | `Breaks SQL`_, `Breaks Time Windows SQL`_,
   | `Time Matrix SQL`_
   | [, exploration_level] [, timeout])  -- Experimental on v0.2
   | RETURNS SET OF
   | (seq, vehicle_seq, vehicle_id, vehicle_data, step_seq, step_type, task_id,
   |  task_data, arrival, travel_time, service_time, waiting_time, departure, load)


**Example**: This example is based on the VROOM Data of the :doc:`sampledata` network:

.. literalinclude:: vroomPlain.queries
   :start-after: -- q1
   :end-before: -- q2

Parameters
-------------------------------------------------------------------------------

.. include:: vrp_vroom.rst
   :start-after: vroom_parameters_start
   :end-before: vroom_parameters_end

Optional Parameters
...............................................................................

.. vroom_plain_optionals_start

.. list-table::
   :widths: auto
   :header-rows: 1

   - - Parameter
     - Type
     - Default
     - Description
   - - ``exploration_level``
     - ``INTEGER``
     - :math:`5`
     - Exploration level to use while solving.

       - Ranges from ``[0, 5]``
       - A smaller exploration level gives faster result.
   - - ``timeout``
     - ``INTEGER``
     - :math:`-1`
     - Timeout value to stop the solving process.

       - Gives the best possible solution within a time limit. Note that some
         additional seconds may be required to return back the data.
       - Any negative timeout value is ignored.

.. vroom_plain_optionals_end

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

Problem involving 2 jobs and 1 shipment, using a single vehicle, similar to the VROOM Documentation
`Example 2 <https://github.com/VROOM-Project/vroom/blob/master/docs/example_2.json>`__ with a shipment.

.. literalinclude:: vroomPlain.queries
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
