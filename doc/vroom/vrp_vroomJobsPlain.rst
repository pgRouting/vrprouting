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
  `Latest <https://vrp.pgrouting.org/latest/en/vrp_vroomJobsPlain.html>`__
  (`v0 <https://vrp.pgrouting.org/v0/en/vrp_vroomJobsPlain.html>`__)


vrp_vroomJobsPlain - Experimental
===============================================================================

``vrp_vroomJobsPlain`` - Vehicle Routing Problem with VROOM, involving only jobs.

.. include:: experimental.rst
   :start-after: begin-warn-expr
   :end-before: end-warn-expr

.. rubric:: Availability

Version 0.2.0

* New **experimental** function


Description
-------------------------------------------------------------------------------

VROOM is an open-source optimization engine that aims at providing good solutions
to various real-life vehicle routing problems (VRP) within a small computing time.
This function can be used to get the solution to a problem involving only jobs.

.. index::
   single: vrp_vroomJobsPlain -- Experimental on v0.2

Signature
-------------------------------------------------------------------------------

.. include:: ../sql/vroom/vrp_vroomJobs.sql
   :start-after: signature start
   :end-before: signature end

**Example**: This example is based on the VROOM Data of the :doc:`sampledata` network:

.. literalinclude:: doc-vrp_vroomJobsPlain.queries
   :start-after: -- q1
   :end-before: -- q2

Parameters
-------------------------------------------------------------------------------

.. include:: ../sql/vroom/vrp_vroomJobs.sql
   :start-after: parameters start
   :end-before: parameters end

Inner Queries
-------------------------------------------------------------------------------

Jobs SQL
...............................................................................

.. include:: ../../src/common/vroom/jobs_input.c
   :start-after: vrp_vroom start
   :end-before: vrp_vroom end

.. include:: vroom-category.rst
   :start-after: inner_queries_start
   :end-before: inner_queries_end

Result Columns
-------------------------------------------------------------------------------

.. include:: vroom-category.rst
   :start-after: result_columns_start
   :end-before: result_columns_end

Additional Example
-------------------------------------------------------------------------------

Problem involving 2 jobs, using a single vehicle, corresponding to the VROOM Documentation
`Example 2 <https://github.com/VROOM-Project/vroom/blob/master/docs/example_2.json>`__.

.. literalinclude:: doc-vrp_vroomJobsPlain.queries
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
