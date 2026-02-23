# Cadmium Freight Elevator (ABP-style repository structure)
#
# Notes:
#  - This makefile assumes the standard Cadmium-Simulation-Environment layout:
#       Cadmium-Simulation-Environment/
#         cadmium/
#         DESTimes/
#         DEVS-Models/
#           Cadmium-FreightElevator/   <-- this repository
#
#  - If your cadmium/DESTimes folders are elsewhere, update INCLUDECADMIUM and
#    INCLUDEDESTIMES.

CC=g++
CFLAGS=-std=c++17

# Cadmium includes (adjust if needed)
INCLUDECADMIUM=-I ../../cadmium/include
INCLUDEDESTIMES=-I ../../DESTimes/include

# Local includes
INCLUDELOCAL=-I ./atomics -I ./data_structures -I ./top_model

# Ensure expected folders exist (ABP-style)
$(shell mkdir -p bin)
$(shell mkdir -p build)
$(shell mkdir -p simulation_results)

# Objects (compiled once, linked into all executables)
DATA_OBJ=build/messages.o

# --- Default target ---
all: simulator tests

# --- Simulator (top model) ---
simulator: bin/freight_elevator_top

bin/freight_elevator_top: $(DATA_OBJ) build/main_top.o
	$(CC) $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) $(INCLUDELOCAL) -o $@ $^

build/main_top.o: top_model/main.cpp \
	data_structures/messages.hpp \
	top_model/freight_elevator_top.hpp top_model/elevator_coupled.hpp \
	atomics/ecall.hpp atomics/econtrol.hpp atomics/evehicle.hpp
	$(CC) $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) $(INCLUDELOCAL) -c $< -o $@

# --- Tests ---
tests: bin/ecall_test bin/econtrol_test bin/evehicle_test bin/elevator_test

bin/ecall_test: $(DATA_OBJ) build/main_ecall_test.o
	$(CC) $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) $(INCLUDELOCAL) -o $@ $^

build/main_ecall_test.o: test/main_ecall_test.cpp \
	data_structures/messages.hpp atomics/ecall.hpp
	$(CC) $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) $(INCLUDELOCAL) -c $< -o $@

bin/econtrol_test: $(DATA_OBJ) build/main_econtrol_test.o
	$(CC) $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) $(INCLUDELOCAL) -o $@ $^

build/main_econtrol_test.o: test/main_econtrol_test.cpp \
	data_structures/messages.hpp atomics/econtrol.hpp
	$(CC) $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) $(INCLUDELOCAL) -c $< -o $@

bin/evehicle_test: $(DATA_OBJ) build/main_evehicle_test.o
	$(CC) $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) $(INCLUDELOCAL) -o $@ $^

build/main_evehicle_test.o: test/main_evehicle_test.cpp \
	data_structures/messages.hpp atomics/evehicle.hpp
	$(CC) $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) $(INCLUDELOCAL) -c $< -o $@

bin/elevator_test: $(DATA_OBJ) build/main_elevator_test.o
	$(CC) $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) $(INCLUDELOCAL) -o $@ $^

build/main_elevator_test.o: test/main_elevator_test.cpp \
	data_structures/messages.hpp top_model/elevator_coupled.hpp \
	atomics/econtrol.hpp atomics/evehicle.hpp
	$(CC) $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) $(INCLUDELOCAL) -c $< -o $@

# --- Shared data structures object ---
build/messages.o: data_structures/messages.cpp data_structures/messages.hpp
	$(CC) $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) $(INCLUDELOCAL) -c $< -o $@

# --- Cleanup ---
clean:
	rm -rf bin build simulation_results
