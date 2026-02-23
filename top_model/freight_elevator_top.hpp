#ifndef FREIGHT_ELEVATOR_TOP_HPP
#define FREIGHT_ELEVATOR_TOP_HPP

#include "cadmium/modeling/devs/coupled.hpp"
#include "../atomics/ecall.hpp"
#include "elevator_coupled.hpp"
#include "../data_structures/messages.hpp"

/**
 * Freight Elevator Top coupled model:
 * ECall -> ElevatorCoupled
 *
 * in : inside_call, outside_call
 * out: floor
 */
struct FreightElevatorTop : public Coupled {
    Port<fe::Floor> inside_call;
    Port<fe::Floor> outside_call;
    Port<fe::Floor> floor;

    explicit FreightElevatorTop(const std::string& id) : Coupled(id) {
        inside_call = addInPort<fe::Floor>("inside_call");
        outside_call = addInPort<fe::Floor>("outside_call");
        floor = addOutPort<fe::Floor>("floor");

        auto call = addComponent<ECall>("Ecall");
        auto elevator = addComponent<ElevatorCoupled>("Elevator");

        // EIC
        addCoupling(inside_call, call->inside_call);
        addCoupling(outside_call, call->outside_call);

        // IC
        addCoupling(call->call_gen, elevator->acall);

        // EOC
        addCoupling(elevator->floor, floor);
    }
};

#endif
