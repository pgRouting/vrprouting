..
   ****************************************************************************
    vrpRouting Manual
    Copyright(c) vrpRouting Contributors

    This documentation is licensed under a Creative Commons Attribution-Share
    Alike 3.0 License: https://creativecommons.org/licenses/by-sa/3.0/
   ****************************************************************************

|

* `Documentation <https://vrp.pgrouting.org/>`__ → `vrpRouting v0 <https://vrp.pgrouting.org/v0>`__
* Supported Versions:
  `Latest <https://vrp.pgrouting.org/latest/en/vrp_full_version.html>`__
  (`v0 <https://vrp.pgrouting.org/v0/en/vrp_full_version.html>`__)

vrp_full_version
===============================================================================

``vrp_full_version`` — Get the details of vrpRouting version information.

.. rubric:: Availability

Version 0.0.0

* New **official** function


Description
-------------------------------------------------------------------------------

Get the details of vrpRouting version information

.. index::
    single: full_version

Signatures
-------------------------------------------------------------------------------

.. include:: ../sql/version/full_version.sql
   :start-after: signature start
   :end-before: signature end

:Example: Get the version installed for this documentation.

.. literalinclude:: doc-full_version.queries
   :start-after: -- q1
   :end-before: -- q2

Result Columns
-------------------------------------------------------------------------------

.. include:: ../sql/version/version.sql
   :start-after: result start
   :end-before: result end


.. rubric:: See also

* :doc:`vrp_version`

.. rubric:: Indices and tables

* :ref:`genindex`
* :ref:`search`
