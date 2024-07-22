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
   
.. include:: ../../sql/or_tools/bin_packing.sql
   :start-after: signature start
   :end-before: signature end

Parameters
-------------------------------------------------------------------------------

.. include:: ../../sql/or_tools/bin_packing.sql
   :start-after: parameters start
   :end-before: parameters end

Optional Parameters
...............................................................................

.. include:: ../../sql/or_tools/bin_packing.sql
   :start-after: optional parameters start
   :end-before: optional parameters end

Inner Queries
-------------------------------------------------------------------------------

Weights SQL
...............................................................................

.. include:: ../../sql/or_tools/bin_packing.sql
   :start-after: Weights start
   :end-before: Weights end

Result Columns
-------------------------------------------------------------------------------

.. include:: ../../sql/or_tools/bin_packing.sql
   :start-after: result start
   :end-before: result end

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