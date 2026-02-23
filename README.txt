Freight Elevator - Cadmium v2 (ABP-style repository structure)

This repository contains the Freight Elevator model implemented in Cadmium v2.

--------------------------------------------------------------------------------
1) Repository structure (mirrors the ABP example)
--------------------------------------------------------------------------------
- atomics/          Atomic DEVS models (ECall, EControl, EVehicle)
- data_structures/  Shared message/type definitions
- input_data/       Input files for experiments/tests
- test/             Stand-alone experiments for atomic models + coupled integration
- top_model/        Coupled models + the top-level simulator (main.cpp)
- vendor/           Optional (kept for consistency with ABP). Not used by default.

The build will generate:
- bin/              Compiled executables
- build/            Object files
- simulation_results/  CSV logs produced by the executables

--------------------------------------------------------------------------------
2) Build instructions
--------------------------------------------------------------------------------
This makefile assumes the standard course layout:

  Cadmium-Simulation-Environment/
    cadmium/
    DESTimes/
    DEVS-Models/
      Cadmium-FreightElevator/   <-- this repository

If your cadmium/DESTimes folders are elsewhere, update INCLUDECADMIUM and
INCLUDEDESTIMES in the makefile.

From the repository root:
  make clean
  make all

Targets:
  make simulator   -> builds ./bin/freight_elevator_top
  make tests       -> builds the test executables under ./bin/

--------------------------------------------------------------------------------
3) Run instructions (recommended: run from the bin/ folder)
--------------------------------------------------------------------------------
cd bin

Top model (Freight Elevator):
  ./freight_elevator_top  [inside_calls_file] [outside_calls_file]

Default input files (if no args are provided):
  ../input_data/inside_calls.txt
  ../input_data/outside_calls.txt

Atomic model experiments:
  ./ecall_test     [inside_calls_file] [outside_calls_file]
  ./econtrol_test  [calls_file] [fback_file]
  ./evehicle_test  [in_file]

Coupled integration experiment:
  ./elevator_test  [calls_file]

Each executable writes a CSV log into:
  ../simulation_results/

--------------------------------------------------------------------------------
4) Documentation files included at the repo root
--------------------------------------------------------------------------------
- FreightElevator.docx
- FreightElevator_ConceptualModel_OnePage.pdf
- form_FreightElevator.doc
- ECall_DEVS_Graph_clean.json, EControl_DEVS_Graph_clean.json, EVehicle_DEVS_Graph_clean.json
