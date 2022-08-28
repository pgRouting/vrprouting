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


OR-Tools - Category (Experimental)
===============================================================================

.. include:: experimental.rst
   :start-after: begin-warn-expr
   :end-before: end-warn-expr


.. contents::

.. rubric:: Functions

.. toctree::
  :maxdepth: 1

  knapsack
  multiple_knapsack
  bin_packing


Synopsis
-------------------------------------------------------------------------------

OR-Tools is an open source software suite for optimization, tuned for tackling
the world's toughest problems in vehicle routing, flows, integer and linear 
programming, and constraint programming.

OR-Tools can solve

- VR (Vehicle Routing)
- Scheduling
- Bin Packing

Currently only Bin Packing problems can be solved with OR-Tools in vrpRouting.

Terminologies
...............................................................................

- **weight**: For each item, a positive integer weight is associated with it.
- **cost**: For each item, a positive integer cost(value) is associated with it.
- **capacity**: Maximum storage capacity of a knapsack or a bin.

Inner Queries
-------------------------------------------------------------------------------

Weights_Costs SQL
...............................................................................

.. include:: ../../sql/or_tools/multiple_knapsack.sql
   :start-after: Weights_Costs start
   :end-before: Weights_Costs end

Weights SQL
...............................................................................

.. include:: ../../sql/or_tools/bin_packing.sql
   :start-after: Weights start
   :end-before: Weights end

See Also
-------------------------------------------------------------------------------

.. see_also_start

* `Wikipedia: Bin Packing problems <https://en.wikipedia.org/wiki/Bin_packing_problem>`__
* `OR-Tools: Google OR-Tools <https://developers.google.com/optimization>`__

.. see_also_end

.. rubric:: Indices and tables

* :ref:`genindex`
* :ref:`search`
