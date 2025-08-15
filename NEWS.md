# vrpRouting 0


## vrpRouting 0.5


### vrpRouting 0.5.0 Release Notes

To see all issues & pull requests closed by this release see the
[Git closed milestone for 0.5.0](https://github.com/pgRouting/vrprouting/issues?utf8=%E2%9C%93&q=milestone%3A%22Release%200.5.0%22)
on Github.

**Removing vroom**

- vrp_vroomjobsplain(text,text,text,text,text,text,integer,integer)
- vrp_vroomjobs(text,text,text,text,text,text,integer,interval)
- vrp_vroomplain(text,text,text,text,text,text,text,text,integer,integer)
- vrp_vroomshipmentsplain(text,text,text,text,text,text,integer,integer)
- vrp_vroomshipments(text,text,text,text,text,text,integer,interval)
- _vrp_vroom(text,text,text,text,text,text,text,text,integer,integer,smallint,boolean)
- vrp_vroom(text,text,text,text,text,text,text,text,integer,interval)

**Removing or-tools**

- vrp_bin_packing(text,integer,integer)
- vrp_knapsack(text,integer,integer)
- vrp_multiple_knapsack(text,integer[],integer)

## vrpRouting 0.4


### vrpRouting 0.4.2 Release Notes

To see all issues & pull requests closed by this release see the
[Git closed milestone for 0.4.2](https://github.com/pgRouting/vrprouting/issues?utf8=%E2%9C%93&q=milestone%3A%22Release%200.4.2%22)
on Github.

**Code reorganization**

* Renamed files to be compiled as C++ with .hpp & .cpp extensions
* Moved sphinx doc from .c files .rst files
* Removing prefix `pgr_` & addding `namespace vrprouting`
* Separating implementation from header vroom.hpp
* Queries are read on the cpp code.

**Documentation queries**

* Renamed to extension `.pg`
* Removed `doc-` and `doc-vrp_` suffixes on file names

### vrpRouting 0.4.1 Release Notes

To see all issues & pull requests closed by this release see the
[Git closed milestone for 0.4.1](https://github.com/pgRouting/vrprouting/issues?utf8=%E2%9C%93&q=milestone%3A%22Release%200.4.1%22)
on Github.


**Modification on experimental functions**

* OR Tools

  * vrp_bin_packing
  * vrp_knapsack
  * vrp_multiple_knapsack

* vrp_oneDepot

**Removal of Boost on the build**

* vrp_full_version
* vrp_version

  
  * Boost removed from the result columns
  
  

### vrpRouting 0.4.0 Release Notes

To see all issues & pull requests closed by this release see the
[Git closed milestone for 0.4.0](https://github.com/pgRouting/vrprouting/issues?utf8=%E2%9C%93&q=milestone%3A%22Release%200.4.0%22)
on Github.

**Added support for VROOM 1.12.0 ([#34](https://github.com/pgRouting/vrprouting/issues/34))**

- No visible changes on user side with respect to signatures.
- Adjusted to VROOM v1.12.0 due to internal breaking changes:

  - Made changes according to new vroom::Input signature.
  - CI changes to compile without routing support, with Position Independent Code.
  - Removed support for VROOM 1.11.0

## vrpRouting 0.3


### vrpRouting 0.3.0 Release Notes

To see all issues & pull requests closed by this release see the
[Git closed milestone for 0.3.0](https://github.com/pgRouting/vrprouting/issues?utf8=%E2%9C%93&q=milestone%3A%22Release%200.3.0%22)
on Github.

**Modification of experimental functions**

- VROOM

  - vrp_vroom
  - vrp_vroomJobs
  - vrp_vroomShipments
  - vrp_vroomPlain
  - vrp_vroomJobsPlain
  - vrp_vroomShipmentsPlain

**Added support for VROOM 1.11.0 ([#24](https://github.com/pgRouting/vrprouting/issues/24))**

- Added setup time in jobs and shipments to refine service time modeling.
- Added support for custom cost matrices, along with the duration matrix.

  - Using start_id, end_id, duration, cost as matrix table columns.
- Added timeout and exploration_level parameters to vroom-category functions.
- Added max_tasks column in vehicles.
- Added tests for empty skills arrays.
- Added custom scaling logic for speed_factor.
- Modified parameter names to make the naming consistent.

**Fixes**

- Honor client cancel requests for vroom-category functions.
- Added more information in the inner query and result columns of VROOM category functions
  ([#26](https://github.com/pgRouting/vrprouting/issues/26), [#27](https://github.com/pgRouting/vrprouting/issues/27)):

  - Summary row in the output, for each vehicle and for the complete problem.
  - Uassigned rows in the output with vehicle_id = -1.
  - Modified travel_time result column to return travel time between current and last step.
  - Added data jsonb field in jobs, shipments, vehicles, breaks as well as in the result columns.
  - Added departure field and location_id field in the result columns.

## vrpRouting 0.2


### vrpRouting 0.2.0 Release Notes

**New experimental functions**

- VROOM

  - vrp_vroom
  - vrp_vroomJobs
  - vrp_vroomShipments
  - vrp_vroomPlain
  - vrp_vroomJobsPlain
  - vrp_vroomShipmentsPlain

- VRP

  - vrp_compatibleVehicles
  - vrp_optimize
  - vrp_pickDeliverAdd
  - vrp_pickDeliver
  - vrp_simulation
  - vrp_viewRoute

## vrpRouting 0.1


### vrpRouting 0.1.0 Release Notes

**Extraction tasks**

- Porting pgRouting's VRP functionality

**New official functions**

* vrp_version
* vrp_full_version
