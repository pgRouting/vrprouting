..
   ****************************************************************************
    vrpRouting Manual
    Copyright(c) vrpRouting Contributors

    This documentation is licensed under a Creative Commons Attribution-Share
    Alike 3.0 License: https://creativecommons.org/licenses/by-sa/3.0/
   ****************************************************************************

|

.. _index:

*******************************************************************************
Table of Contents
*******************************************************************************

vrpRouting extends the `PostGIS <https://postgis.net>`__/`PostgreSQL <https://www.postgresql.org>`__
geospatial database to provide geospatial routing and other network analysis functionality.

This is the manual for vrpRouting |release|.

.. image:: _static/images/ccbysa.png
    :align: left
    :alt: Creative Commons Attribution-Share Alike 3.0 License

The vrpRouting Manual is licensed under a `Creative Commons Attribution-Share Alike 3.0 License
<https://creativecommons.org/licenses/by-sa/3.0/>`_.
Feel free to use this material any way you like, but we ask that you attribute credit to
the vrpRouting Project and wherever possible, a link back to https://pgrouting.org.
For other licenses used in vrpRouting see the :ref:`license` page.

General
===============================================================================

.. toctree::
    :maxdepth: 2

    introduction
    installation
    support


- :doc:`sampledata` that is used in the examples of this manual.

.. toctree::
   :hidden:

   sampledata

:doc:`functions`
===============================================================================

.. include:: functions.rst
   :start-after: functions_start
   :end-before: functions_end

.. toctree::
   :hidden:

   functions

:doc:`release_notes`
===============================================================================

.. include:: release_notes.rst
   :start-after: current

.. toctree::
   :hidden:

   release_notes

.. rubric:: Indices and tables

* :ref:`genindex`
* :ref:`search`
