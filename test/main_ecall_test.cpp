#include <memory>
#include <string>

#include "cadmium/core/logger/csv.hpp"
#include "cadmium/core/simulation/root_coordinator.hpp"
#include "cadmium/lib/iestream.hpp"
#include "cadmium/modeling/devs/coupled.hpp"

#include "../atomics/ecall.hpp"
#include "../data_structures/messages.hpp"

using namespace std;

// Stand-alone experiment for the ECall atomic model.
struct ECallExperiment : public Coupled {
  Port<freight_elevator::Floor> out;

  ECallExperiment(const string& id,
                  const string& inside_calls_path,
                  const string& outside_calls_path)
      : Coupled(id) {
    out = addOutPort<freight_elevator::Floor>("out");

    auto inside_calls = addComponent<cadmium::lib::IEStream<freight_elevator::Floor>>(
        "inside_calls", inside_calls_path);
    auto outside_calls = addComponent<cadmium::lib::IEStream<freight_elevator::Floor>>(
        "outside_calls", outside_calls_path);

    auto ecall = addComponent<ECall>("ecall");

    // Inputs
    addCoupling(inside_calls->out, ecall->inside_call);
    addCoupling(outside_calls->out, ecall->outside_call);

    // Output
    addCoupling(ecall->call_gen, out);
  }
};

int main(int argc, char* argv[]) {
  // Default test inputs (assumes you run from ./bin)
  string inside_path = "../input_data/ecall_inside_test.txt";
  string outside_path = "../input_data/ecall_outside_test.txt";

  // Optional CLI: ./ecall_test <inside_calls_path> <outside_calls_path>
  if (argc >= 3) {
    inside_path = argv[1];
    outside_path = argv[2];
  }

  auto model = make_shared<ECallExperiment>("ECallExperiment", inside_path, outside_path);
  auto logger = make_shared<cadmium::core::logger::CSVLogger>(
      "../simulation_results/ecall_test.csv", ";");

  cadmium::core::simulation::RootCoordinator root(model, logger);
  root.setTime(50);
  root.start();
  root.simulate();
  root.stop();

  return 0;
}
