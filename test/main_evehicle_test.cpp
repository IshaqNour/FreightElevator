#include <memory>
#include <string>

#include "cadmium/core/logger/csv.hpp"
#include "cadmium/core/simulation/root_coordinator.hpp"
#include "cadmium/lib/iestream.hpp"
#include "cadmium/modeling/devs/coupled.hpp"

#include "../atomics/evehicle.hpp"
#include "../data_structures/messages.hpp"

using namespace std;

// Stand-alone experiment for the EVehicle atomic model.
struct EVehicleExperiment : public Coupled {
  Port<int> out;

  EVehicleExperiment(const string& id, const string& in_path) : Coupled(id) {
    out = addOutPort<int>("out");

    auto in_stream = addComponent<cadmium::lib::IEStream<int>>(
        "in_stream", in_path);

    auto vehicle = addComponent<EVehicle>("evehicle");

    // Input and output couplings
    addCoupling(in_stream->out, vehicle->in);
    addCoupling(vehicle->out, out);
  }
};

int main(int argc, char* argv[]) {
  // Default test input (assumes you run from ./bin)
  string in_path = "../input_data/evehicle_in_test.txt";

  // Optional CLI: ./evehicle_test <in_path>
  if (argc >= 2) {
    in_path = argv[1];
  }

  auto model = make_shared<EVehicleExperiment>("EVehicleExperiment", in_path);
  auto logger = make_shared<cadmium::core::logger::CSVLogger>(
      "../simulation_results/evehicle_test.csv", ";");

  cadmium::core::simulation::RootCoordinator root(model, logger);
  root.setTime(50);
  root.start();
  root.simulate();
  root.stop();

  return 0;
}
