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
  `Latest <https://vrp.pgrouting.org/latest/en/vrp_oneDepot.html>`__
  (`v0 <https://vrp.pgrouting.org/v0/en/vrp_oneDepot.html>`__)

vrp_bin_packing - Experimental
===============================================================================

.. include:: experimental.rst
   :start-after: begin-warn-expr
   :end-before: end-warn-expr

.. rubric:: Availability

Version 0.4.1

* Support for or-tools v9.10.4067

Version 0.4.0

* New **experimental** function

  * vrp_knapsack



Description
-------------------------------------------------------------------------------

The bin packing problem is an optimization problem, in which
items of different sizes must be packed into a finite number of bins or containers,
each of a fixed given capacity, in a way that minimizes the number of bins used.
The problem has many applications, such as filling up containers, loading trucks with weight capacity constraints,
creating file backups in media and technology mapping in FPGA semiconductor chip design.


Signatures
-------------------------------------------------------------------------------

.. rubric:: Summary

.. admonition:: \ \
   :class: signatures

   | vrp_bin_packing(`Weights SQL`_, bin_capacity, [``max_rows``])
   | Returns set of (bin_number, item_id)
   | OR EMPTY SET

Parameters
-------------------------------------------------------------------------------

.. list-table::
   :width: 81
   :widths: 14 14 44
   :header-rows: 1

   * - Column
     - Type
     - Description
   * - `Weights SQL`_
     - ``TEXT``
     - `Weights SQL`_ as described below
   * - bin_capacity
     - **ANY-INTEGER**
     - Maximum Capacity of the bin.

Optional Parameters
...............................................................................

.. list-table::
   :width: 81
   :widths: auto
   :header-rows: 1

   * - Column
     - Type
     - Default
     - Description
   * - ``max_rows``
     - **ANY-INTEGER**
     - :math:`100000`
     - Maximum items(rows) to fetch from bin_packing_data table.

Where:

:ANY-INTEGER: ``SMALLINT``, ``INTEGER``, ``BIGINT``

Inner Queries
-------------------------------------------------------------------------------

Weights SQL
...............................................................................


.. include:: or_tools-category.rst
   :start-after: weights_start
   :end-before: weights_end

Result Columns
-------------------------------------------------------------------------------

.. list-table::
   :width: 81
   :widths: auto
   :header-rows: 1

   * - Column
     - Type
     - Description
   * - ``bin``
     - **ANY-INTEGER**
     - Integer to uniquely identify a bin.
   * - ``id``
     - **ANY-INTEGER**
     - Indentifier of an item in the ``bin``.

Where:

:ANY-INTEGER: ``SMALLINT``, ``INTEGER``, ``BIGINT``

Example
-------------------------------------------------------------------------------

.. literalinclude:: doc-vrp_bin_packing.queries
   :start-after: -- example_start
   :end-before: -- example_end

See Also
-------------------------------------------------------------------------------

.. include:: or_tools-category.rst
   :start-after: see_also_start
   :end-before: see_also_end

.. rubric:: Indices and tables

* :ref:`genindex`
* :ref:`search`
