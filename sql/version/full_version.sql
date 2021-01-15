/*PGR-GNU*****************************************************************

Copyright (c) 2019 pgRouting developers
Mail: project@pgrouting.org

Developer:
Copyright (c) 2019 Celia Virginia Vergara Castillo

------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

 ********************************************************************PGR-GNU*/

/*
signature start

::

    vrp_full_version()
    RETURNS RECORD OF (version, build_type, compile_date, library, system, PostgreSQL, compiler, boost, hash)

signature end

result start

================  =========== ===============================
Column             Type       Description
================  =========== ===============================
**version**       ``TEXT``    vrpRouting version
**build_type**    ``TEXT``    The Build type
**compile_date**  ``TEXT``    Compilation date
**library**       ``TEXT``    Library name and version
**system**        ``TEXT``    Operative system
**postgreSQL**    ``TEXT``    pgsql used
**compiler**      ``TEXT``    Compiler and version
**boost**         ``TEXT``    Boost version
**hash**          ``TEXT``    Git hash of vrpRouting build
================  =========== ===============================

result end
*/


--v3.0
CREATE FUNCTION vrp_full_version(
    OUT version TEXT,
    OUT build_type TEXT,
    OUT compile_date TEXT,
    OUT library TEXT,
    OUT system TEXT,
    OUT PostgreSQL TEXT,
    OUT compiler TEXT,
    OUT boost TEXT,
    OUT hash TEXT
)
RETURNS Record AS
$BODY$
    SELECT vrp_version(),
        _vrp_build_type(),
        _vrp_compilation_date(),
        _vrp_lib_version(),
        _vrp_operating_system(),
        _vrp_pgsql_version(),
        _vrp_compiler_version(),
        _vrp_boost_version(),
        _vrp_git_hash()
$BODY$
LANGUAGE sql IMMUTABLE;

COMMENT ON FUNCTION vrp_full_version() IS
'vrp_full_version
- Documentation
  - ${PROJECT_DOC_LINK}/vrp_full_version.html
';
