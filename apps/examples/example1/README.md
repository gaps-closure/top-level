# CLOSURE Example: Example 1

## Directory Structure

* plain:       contains original unannotated application
* annotated:   contains annotations applied to plain to reflect security intent below
* refactored:  contains annotated code refactored for making security-compliant partitioning feasible
* divvied:     contains code divvied up into separate directories for each level/enclave as input for GEDL generation, followed by code autogeneration
* partitioned: contains fully paritioned sources with cross-domain RPCs, marshalling, serialization, tags, HAL API, etc.

## Cross Domain Topology

* Enclaves: 2 (Orange, Purple)

## Security Intent

* Variable `a` in `get_a()` is in ORANGE and can be shared with PURPLE
* Variable `b` in `get_b()` is in PURPLE and cannot be shared
* Calculated EWMA must be available on PURPLE side (for printing there)

## Dependencies

* CLE Version:
* Refactoring Methodology:
* HAL API:
* DFDL/DAGR:

## Other Tools

None