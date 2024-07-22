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

vrp_multiple_knapsack - Experimental
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

The multiple knapsack problem is a problem in combinatorial optimization: 
it is a more general verison of the classic knapsack problem where instead of a
single knapsack, you will be given multiple knapsacks and your goal is maximise the total
value of packed items in all knapsacks.

Signatures
-------------------------------------------------------------------------------

.. include:: ../../sql/or_tools/multiple_knapsack.sql
   :start-after: signature start
   :end-before: signature end

Parameters
-------------------------------------------------------------------------------

.. include:: ../../sql/or_tools/multiple_knapsack.sql
   :start-after: parameters start
   :end-before: parameters end

Optional Parameters
...............................................................................

.. include:: ../../sql/or_tools/multiple_knapsack.sql
   :start-after: optional parameters start
   :end-before: optional parameters end

Inner Queries
-------------------------------------------------------------------------------

Weights_Costs SQL
...............................................................................

.. include:: ../../sql/or_tools/multiple_knapsack.sql
   :start-after: Weights_Costs start
   :end-before: Weights_Costs end

Result Columns
-------------------------------------------------------------------------------

.. include:: ../../sql/or_tools/multiple_knapsack.sql
   :start-after: result start
   :end-before: result end

Example
-------------------------------------------------------------------------------

.. literalinclude:: doc-vrp_multiple_knapsack.queries
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