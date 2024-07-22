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

vrp_knapsack - Experimental
===============================================================================

.. include:: experimental.rst
   :start-after: begin-warn-expr
   :end-before: end-warn-expr

.. rubric:: Availability

Version 0.4.0

* New **experimental** function

  * vrp_knapsack



Description
-------------------------------------------------------------------------------

The knapsack problem is a problem in combinatorial optimization: 
Given a set of items, each with a weight and a value, 
Determine the number of each item to include in a collection 
so that the total weight is less than or equal to a given limit and the total value is as large as possible

Signatures
-------------------------------------------------------------------------------

.. include:: ../../sql/or_tools/knapsack.sql
   :start-after: signature start
   :end-before: signature end

Parameters
-------------------------------------------------------------------------------

.. include:: ../../sql/or_tools/knapsack.sql
   :start-after: parameters start
   :end-before: parameters end

Optional Parameters
...............................................................................

.. include:: ../../sql/or_tools/knapsack.sql
   :start-after: optional parameters start
   :end-before: optional parameters end

Inner Queries
-------------------------------------------------------------------------------

Weights_Costs SQL
...............................................................................

.. include:: ../../sql/or_tools/knapsack.sql
   :start-after: Weights_Costs start
   :end-before: Weights_Costs end

Result Columns
-------------------------------------------------------------------------------

.. include:: ../../sql/or_tools/knapsack.sql
   :start-after: result start
   :end-before: result end

Example
-------------------------------------------------------------------------------

.. literalinclude:: doc-vrp_knapsack.queries
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