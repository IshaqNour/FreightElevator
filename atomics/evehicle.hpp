#ifndef EVehicle_HPP
#define EVehicle_HPP

#include <cadmium/modeling/devs/atomic.hpp>
#include <limits>
#include <ostream>

#include "../data_structures/messages.hpp"

/**
 * EVehicle (Elevator Vehicle)
 * - Receives a travel time (in minutes) and schedules completion after that delay.
 * - When the delay expires, outputs a feedback message (fback).
 *
 * Simplest behavior:
 * - Ignores new commands while already moving.
 */
class EVehicle : public cadmium::Atomic<struct EVehicleState> {
public:
    // Ports
    cadmium::Port<fe::TravelTime> in;   // input: travel time command
    cadmium::Port<fe::TravelTime> out;  // output: completion feedback (echoes travel time)

    explicit EVehicle(const std::string& id);

    void externalTransition(EVehicleState& s, double e) const override;
    void internalTransition(EVehicleState& s) const override;
    void confluentTransition(EVehicleState& s, double e) const override;
    void output(const EVehicleState& s) const override;
    [[nodiscard]] double timeAdvance(const EVehicleState& s) const override;
};

// -------------------- State --------------------

enum class EVehiclePhase { idle, moving };

struct EVehicleState {
    double sigma;
    EVehiclePhase phase;
    fe::TravelTime travel_time;

    explicit EVehicleState(EVehiclePhase p = EVehiclePhase::idle)
        : sigma(std::numeric_limits<double>::infinity()), phase(p), travel_time(0) {}
};

inline std::ostream& operator<<(std::ostream& os, const EVehicleState& s) {
    os << "{phase:" << (s.phase == EVehiclePhase::idle ? "idle" : "moving")
       << ",t:" << s.travel_time
       << ",sigma:" << s.sigma << "}";
    return os;
}

// -------------------- Implementation --------------------

inline EVehicle::EVehicle(const std::string& id) : cadmium::Atomic<EVehicleState>(id, EVehicleState()) {
    in  = addInPort<fe::TravelTime>("in");
    out = addOutPort<fe::TravelTime>("out");
}

inline void EVehicle::externalTransition(EVehicleState& s, double e) const {
    // account elapsed time
    if (s.sigma != std::numeric_limits<double>::infinity()) {
        s.sigma = std::max(0.0, s.sigma - e);
    }

    // Only accept a new command if idle
    if (s.phase == EVehiclePhase::idle && !in->empty()) {
        s.travel_time = in->getBag().back();
        s.phase = EVehiclePhase::moving;
        s.sigma = static_cast<double>(s.travel_time);
    }
}

inline void EVehicle::output(const EVehicleState& s) const {
    if (s.phase == EVehiclePhase::moving) {
        out->addMessage(s.travel_time);
    }
}

inline void EVehicle::internalTransition(EVehicleState& s) const {
    if (s.phase == EVehiclePhase::moving) {
        s.phase = EVehiclePhase::idle;
        s.sigma = std::numeric_limits<double>::infinity();
        s.travel_time = 0;
    }
}

inline void EVehicle::confluentTransition(EVehicleState& s, double /*e*/) const {
    // internal then external with e=0
    internalTransition(s);
    externalTransition(s, 0.0);
}

inline double EVehicle::timeAdvance(const EVehicleState& s) const {
    return s.sigma;
}

#endif
