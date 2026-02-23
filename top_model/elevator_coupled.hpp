#ifndef ELEVATOR_COUPLED_HPP
#define ELEVATOR_COUPLED_HPP

#include "cadmium/modeling/devs/coupled.hpp"
#include "../atomics/econtrol.hpp"
#include "../atomics/evehicle.hpp"
#include "../data_structures/messages.hpp"

/**
 * Elevator coupled model:
 * EControl <-> EVehicle
 *
 * in : acall
 * out: floor
 */
struct ElevatorCoupled : public Coupled {
    Port<fe::Floor> acall;
    Port<fe::Floor> floor;

    explicit ElevatorCoupled(const std::string& id) : Coupled(id) {
        acall = addInPort<fe::Floor>("acall");
        floor = addOutPort<fe::Floor>("floor");

        auto control = addComponent<EControl>("Econtrol");
        auto vehicle = addComponent<EVehicle>("Evehicle");

        // EIC
        addCoupling(acall, control->acall);

        // IC
        addCoupling(control->timem, vehicle->in);
        addCoupling(vehicle->out, control->fback);

        // EOC
        addCoupling(control->floor, floor);
    }
};

#endif
