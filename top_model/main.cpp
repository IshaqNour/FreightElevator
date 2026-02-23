#include <cadmium/core/logger/csv.hpp>
#include <cadmium/core/simulation/root_coordinator.hpp>
#include <limits>
#include <memory>
#include <string>

#include "experiment.hpp"

int main(int argc, char** argv) {
    // You can pass input file paths from the command line to avoid hard-coding:
    //   ./bin/freight_elevator_top input_data/inside.txt input_data/outside.txt
    std::string inside_path = (argc > 1) ? argv[1] : "../input_data/inside_calls.txt";
    std::string outside_path = (argc > 2) ? argv[2] : "../input_data/outside_calls.txt";

    auto model = std::make_shared<FreightElevatorExperiment>("freight_elevator_experiment",
                                                             inside_path,
                                                             outside_path);

    auto rootCoordinator = cadmium::RootCoordinator(model);
    auto logger = std::make_shared<cadmium::CSVLogger>("../simulation_results/freight_elevator_top.csv", ";");
    rootCoordinator.setLogger(logger);

    rootCoordinator.start();
    rootCoordinator.simulate(50.0);  // minutes
    rootCoordinator.stop();

    return 0;
}
