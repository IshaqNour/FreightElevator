#include <memory>
#include <string>

#include "cadmium/core/logger/csv.hpp"
#include "cadmium/core/simulation/root_coordinator.hpp"
#include "cadmium/lib/iestream.hpp"
#include "cadmium/modeling/devs/coupled.hpp"

#include "../atomics/econtrol.hpp"
#include "../data_structures/messages.hpp"

using namespace std;

// Stand-alone experiment for the EControl atomic model.
struct EControlExperiment : public Coupled {
  Port<freight_elevator::TravelTime> timem_out;
  Port<freight_elevator::Floor> floor_out;

  EControlExperiment(const string& id,
                     const string& calls_path,
                     const string& fback_path)
      : Coupled(id) {
    timem_out = addOutPort<freight_elevator::TravelTime>("timem_out");
    floor_out = addOutPort<freight_elevator::Floor>("floor_out");

    auto calls = addComponent<cadmium::lib::IEStream<freight_elevator::Floor>>(
        "acall_stream", calls_path);
    auto fback = addComponent<cadmium::lib::IEStream<int>>(
        "fback_stream", fback_path);

    auto ctrl = addComponent<EControl>("econtrol");

    // Inputs
    addCoupling(calls->out, ctrl->acall);
    addCoupling(fback->out, ctrl->fback);

    // Outputs
    addCoupling(ctrl->timem, timem_out);
    addCoupling(ctrl->floor, floor_out);
  }
};

int main(int argc, char* argv[]) {
  // Default test inputs (assumes you run from ./bin)
  string calls_path = "../input_data/econtrol_calls_test.txt";
  string fback_path = "../input_data/econtrol_fback_test.txt";

  // Optional CLI: ./econtrol_test <calls_path> <fback_path>
  if (argc >= 3) {
    calls_path = argv[1];
    fback_path = argv[2];
  }

  auto model = make_shared<EControlExperiment>("EControlExperiment", calls_path, fback_path);
  auto logger = make_shared<cadmium::core::logger::CSVLogger>(
      "../simulation_results/econtrol_test.csv", ";");

  cadmium::core::simulation::RootCoordinator root(model, logger);
  root.setTime(50);
  root.start();
  root.simulate();
  root.stop();

  return 0;
}
