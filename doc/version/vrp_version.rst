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
  `Latest <https://vrp.pgrouting.org/latest/en/vrp_version.html>`__
  (`v0 <https://vrp.pgrouting.org/v0/en/vrp_version.html>`__)

vrp_version
===============================================================================

``vrp_version`` — Get the only the version

.. rubric:: Availability

Version 0.0.0

* New **Official** function



Description
-------------------------------------------------------------------------------

Returns vrpRouting version information.

.. index::
    single: version

Signatures
-------------------------------------------------------------------------------

.. admonition:: \ \
   :class: signatures

   | pgr_version()
   | RETURNS ``TEXT``

:Example: vrpRouting Version for this documentatoin

.. literalinclude:: version.queries
   :start-after: -- q1
   :end-before: -- q2

Result Columns
-------------------------------------------------------------------------------

=========== ===============================
 Type       Description
=========== ===============================
``TEXT``    vrpRouting version
=========== ===============================


.. rubric:: See Also

* :doc:`vrp_full_version`

.. rubric:: Indices and tables

* :ref:`genindex`
* :ref:`search`
