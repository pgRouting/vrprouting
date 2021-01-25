..
   ****************************************************************************
    vrpRouting Manual
    Copyright(c) vrpRouting Contributors

    This documentation is licensed under a Creative Commons Attribution-Share
    Alike 3.0 License: https://creativecommons.org/licenses/by-sa/3.0/
   ****************************************************************************

Installation
===============================================================================

.. contents::

Instructions for downloading and installing binaries for different Operative systems
instructions and additional notes and corrections not included in this documentation
can be found in
`Installation wiki <https://github.com/pgRouting/vrprouting/wiki/Notes-on-Download%2C-Installation-and-building-vrpRouting>`__

To use vrpRouting: PostgreSQL server, pgRouting and postGIS needs to be installed.

There is no package for vrpRouting

Get the vrpRouting sources
-------------------------------------------------------------------------------


The vrpRouting latest release can be found in https://github.com/pgRouting/vrprouting/releases/latest

To get this release, download the repository or download the

.. rubric:: Download the repository

.. code-block:: bash

    git clone git://github.com/pgRouting/vrprouting.git
    cd pgrouting
    git checkout v${PROJECT_VERSION}


.. rubric:: Download this release

.. code-block:: bash

    wget -O pgrouting-${PROJECT_VERSION}.tar.gz https://github.com/pgRouting/vrprouting/archive/v${PROJECT_VERSION}.tar.gz


Short Version
-------------------------------------------------------------------------------


Extracting the tar ball

.. code-block:: bash

    tar xvfz pgrouting-${PROJECT_VERSION}.tar.gz
    cd pgrouting-${PROJECT_VERSION}

To compile assuming you have all the dependencies in your search path:

.. code-block:: bash

    mkdir build
    cd build
    cmake  ..
    make
    sudo make install

Once vrpRouting is installed, it needs to be enabled in each individual database you want to use it in.


Enabling and upgrading in the database
...............................................................................

.. rubric:: Enabling the database

vrpRouting is an extension and depends on postGIS. Enabling postGIS before enabling vrpRouting in the database

.. code-block:: sql

  CREATE EXTENSION vrprouting CASCADE;


.. rubric:: Upgrading the database

When the database has an old version of vrpRouting then an upgrade is needed.


.. code-block:: sql

   ALTER EXTENSION vrprouting UPDATE TO "${PROJECT_VERSION}";


.. rubric:: See also

* https://www.postgresql.org/docs/current/sql-createextension.html
* https://www.postgresql.org/docs/current/sql-alterextension.html


Dependencies
-------------------------------------------------------------------------------

.. rubric:: Compilation Dependencies

To be able to compile vrpRouting, make sure that the following dependencies are met:

* C and C++0x compilers
  * g++ version >= 4.8
* Postgresql version >= 12
* The Boost Graph Library (BGL). Version >= 1.71
* CMake >= 3.2


.. rubric:: optional dependencies

For user's documentation

* Sphinx >= 1.1
* Latex

For developer's documentation

* Doxygen >= 1.7

For testing

* pgtap
* pg_prove

For using:

* PostGIS version >= 2.2

.. rubric:: Example: Installing dependencies on linux

Installing the compilation dependencies

.. rubric:: Database dependencies

.. code-block:: none

    sudo apt-get install
        postgresql-10 \
        postgresql-server-dev-10 \
        postgresql-10-postgis


.. rubric:: Build dependencies

.. code-block:: none

    sudo apt-get install
        cmake \
        g++ \
        libboost-graph-dev

.. rubric:: Optional dependencies

For documentation and testing

.. code-block:: none

    sudo apt-get install -y python-sphinx \
        texlive \
        doxygen \
        libtap-parser-sourcehandler-pgtap-perl \
        postgresql-10-pgtap


.. _install_configuring:

Configuring
-------------------------

vrpRouting uses the `cmake` system to do the configuration.

The build directory is different from the source directory

Create the build directory

.. code-block:: bash

    $ mkdir build

Configurable variables
.......................

.. rubric:: To see the variables that can be configured


.. code-block:: bash

    $ cd build
    $ cmake -L ..


.. rubric:: Configuring The Documentation

Most of the effort of the documentation has being on the HTML files.
Some variables for the documentation:

================== ========= ============================
Variable            Default     Comment
================== ========= ============================
WITH_DOC           BOOL=OFF  Turn on/off building the documentation
BUILD_HTML         BOOL=ON   If ON, turn on/off building HTML for user's documentation
BUILD_DOXY         BOOL=ON   If ON, turn on/off building HTML for developer's documentation
BUILD_LATEX        BOOL=OFF  If ON, turn on/off building PDF
BUILD_MAN          BOOL=OFF  If ON, turn on/off building MAN pages
DOC_USE_BOOTSTRAP  BOOL=OFF  If ON, use sphinx-bootstrap for HTML pages of the users documentation
================== ========= ============================

Configuring with documentation

.. code-block:: bash

    $ cmake -DWITH_DOC=ON ..

.. note:: Most of the effort of the documentation has being on the html files.


.. _install_build:

Building
----------------------------------------------------------------

Using ``make`` to build the code and the documentation

The following instructions start from *path/to/pgrouting/build*

.. code-block:: bash

    $ make          # build the code but not the documentation
    $ make doc      # build only the documentation
    $ make all doc  # build both the code and the documentation


We have tested on several platforms, For installing or reinstalling all the steps are needed.

.. warning:: The sql signatures are configured and build in the ``cmake`` command.

.. rubric:: MinGW on Windows

.. code-block:: bash

    $ mkdir build
    $ cd build
    $ cmake -G"MSYS Makefiles" ..
    $ make
    $ make install


.. rubric:: Linux

The following instructions start from *path/to/pgrouting*

.. code-block:: bash

    mkdir build
    cd build
    cmake  ..
    make
    sudo make install

When the configuration changes:

.. code-block:: bash

    rm -rf build

and start the build process as mentioned above.

.. _install_testing:

Testing
-------------------------

Currently there is no :code:`make test` and testing is done as follows

The following instructions start from *path/to/pgrouting/*

.. code-block:: none

    tools/testers/doc_queries_generator.pl
    createdb  -U <user> ___pgr___test___
    sh ./tools/testers/pg_prove_tests.sh <user>
    dropdb  -U <user> ___pgr___test___

See Also
-------------------------------------------------------------------------------

.. rubric:: Indices and tables

* :ref:`genindex`
* :ref:`search`


