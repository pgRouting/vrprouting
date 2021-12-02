# vrpRouting - Vehicle Routing problems on PostgreSQL

[![Join the chat at https://gitter.im/pgRouting/pgrouting](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/pgRouting/pgrouting?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

## STATUS

### Branches

* The *master* branch has the development of the next micro release
* The *develop* branch has the development of the next minor/major release

For the complete list of releases go to:
https://github.com/pgRouting/pgrouting/releases


## LINKS

* https://pgrouting.org/
* https://vrp.pgrouting.org/
* https://github.com/pgRouting/vrprouting

## STATUS

Status of the project can be found [here](https://github.com/pgRouting/vrprouting/wiki#status)


## INTRODUCTION

vrpRouting extends the pgRouting/PostGIS/PostgreSQL geospatial database to provide algorithms for Vehicle Routing Problems.
It requires VROOM as a dependency for building and computing the VRP solution. This release is compatible with VROOM version 1.11.0

This library is under development and currently contains the following functions:

* `vrp_onedepot`
* `vrp_pgr_pickdelivereuclidean`
* `vrp_pgr_pickdeliver`
* `vrp_vroom`
* `vrp_vroomJobs`
* `vrp_vroomShipments`

## REQUIREMENTS

Building requirements
--------------------
* Perl
* C and C++ compilers with C++17 standard support
* Postgresql >= TBD
* The Boost Graph Library (BGL) >= 1.65
* CMake >= 3.12
* Sphinx > 4.0.0
* VROOM >= 1.11.0

User's requirements
--------------------

* PostGIS
* pgRouting

## COMPILATION

For Linux

	mkdir build
	cd build
	cmake  ..
	make
	sudo make install

Build with documentation (requires [Sphinx](http://sphinx-doc.org/))

	cmake -DWITH_DOC=ON ..
    make doc

Postgresql

	createdb mydatabase
	psql mydatabase -c "CREATE EXTENSION vrprouting CASCADE"

## USAGE

See online documentation: https://vrp.pgrouting.org/latest/en/index.html

## LICENSE

* GPLv2-or-later
* Boost under Boost license (see LICENSE_1_0.txt)


