#include <memory>
#include <string>

#include "cadmium/core/logger/csv.hpp"
#include "cadmium/core/simulation/root_coordinator.hpp"
#include "cadmium/lib/iestream.hpp"
#include "cadmium/modeling/devs/coupled.hpp"

#include "../top_model/elevator_coupled.hpp"
#include "../data_structures/messages.hpp"

using namespace std;

// Integration experiment for the ElevatorCoupled (EControl + EVehicle).
struct ElevatorExperiment : public Coupled {
  Port<freight_elevator::Floor> floor_out;

  ElevatorExperiment(const string& id, const string& calls_path) : Coupled(id) {
    floor_out = addOutPort<freight_elevator::Floor>("floor_out");

    auto calls = addComponent<cadmium::lib::IEStream<freight_elevator::Floor>>(
        "calls_stream", calls_path);

    auto elevator = addComponent<ElevatorCoupled>("elevator");

    // Input and output couplings
    addCoupling(calls->out, elevator->call_in);
    addCoupling(elevator->floor_out, floor_out);
  }
};

int main(int argc, char* argv[]) {
  // Default test input (assumes you run from ./bin)
  string calls_path = "../input_data/elevator_calls_test.txt";

  // Optional CLI: ./elevator_test <calls_path>
  if (argc >= 2) {
    calls_path = argv[1];
  }

  auto model = make_shared<ElevatorExperiment>("ElevatorExperiment", calls_path);
  auto logger = make_shared<cadmium::core::logger::CSVLogger>(
      "../simulation_results/elevator_test.csv", ";");

  cadmium::core::simulation::RootCoordinator root(model, logger);
  root.setTime(50);
  root.start();
  root.simulate();
  root.stop();

  return 0;
}
