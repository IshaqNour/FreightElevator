#ifndef FREIGHT_ELEVATOR_EXPERIMENT_HPP
#define FREIGHT_ELEVATOR_EXPERIMENT_HPP

#include "cadmium/modeling/devs/coupled.hpp"
#include "cadmium/lib/iestream.hpp"

#include "freight_elevator_top.hpp"

/**
 * Experiment coupled model:
 * - Two IEStreams feed inside/outside call ports
 * - Exposes the system output port so main() can log it
 */
struct FreightElevatorExperiment : public Coupled {
    Port<fe::Floor> floor_out;

    FreightElevatorExperiment(const std::string& id,
                              const std::string& inside_calls_file,
                              const std::string& outside_calls_file)
        : Coupled(id) {

        floor_out = addOutPort<fe::Floor>("floor_out");

        auto inside_calls = addComponent<cadmium::lib::IEStream<fe::Floor>>("inside_calls", inside_calls_file);
        auto outside_calls = addComponent<cadmium::lib::IEStream<fe::Floor>>("outside_calls", outside_calls_file);
        auto system = addComponent<FreightElevatorTop>("freight_elevator");

        addCoupling(inside_calls->out, system->inside_call);
        addCoupling(outside_calls->out, system->outside_call);
        addCoupling(system->floor, floor_out);
    }
};

#endif
