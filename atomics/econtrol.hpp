#ifndef ECONTROL_HPP
#define ECONTROL_HPP

#include <cadmium/modeling/devs/atomic.hpp>
#include <limits>
#include <deque>
#include <cmath>
#include <ostream>

#include "../data_structures/messages.hpp"

/**
 * EControl (Elevator Controller)
 * - Receives floor requests (acall) and completion feedback (fback).
 * - Computes travel time as |target - current| minutes (1 minute per floor).
 * - Sends travel time to the vehicle via timem.
 * - When the vehicle reports completion, outputs the reached floor via floor.
 *
 * Simplest behavior:
 * - Single elevator, FIFO queue for requests.
 * - While moving, additional requests are queued.
 * - Upon arrival, controller may output both:
 *   (i) the reached floor, and
 *   (ii) the next travel time (if queue is non-empty),
 *   in the same immediate (sigma=0) internal event.
 */
class EControl : public cadmium::Atomic<struct EControlState> {
public:
    // Ports
    cadmium::Port<fe::Floor>      acall;   // input: requested floors
    cadmium::Port<fe::TravelTime> fback;   // input: arrival feedback (value ignored)
    cadmium::Port<fe::TravelTime> timem;   // output: travel time command
    cadmium::Port<fe::Floor>      floor;   // output: reached floor

    explicit EControl(const std::string& id);

    void externalTransition(EControlState& s, double e) const override;
    void internalTransition(EControlState& s) const override;
    void confluentTransition(EControlState& s, double e) const override;
    void output(const EControlState& s) const override;
    [[nodiscard]] double timeAdvance(const EControlState& s) const override;

private:
    static fe::TravelTime compute_travel_time(fe::Floor from, fe::Floor to) {
        return static_cast<fe::TravelTime>(std::abs(to - from));
    }
    static void start_next_if_idle(EControlState& s);
};

// -------------------- State --------------------

struct EControlState {
    fe::Floor current_floor = 1;

    // Movement bookkeeping
    bool moving = false;
    fe::Floor target_floor = 1;

    // Pending outputs for next internal event
    bool send_floor = false;
    fe::Floor floor_to_send = 1;

    bool send_timem = false;
    fe::TravelTime timem_to_send = 0;

    // FIFO request queue
    std::deque<fe::Floor> requests;

    // Time until next internal event (0 or infinity in this model)
    double sigma = std::numeric_limits<double>::infinity();
};

inline std::ostream& operator<<(std::ostream& os, const EControlState& s) {
    os << "{cur:" << s.current_floor
       << ",moving:" << (s.moving ? "T" : "F")
       << ",target:" << s.target_floor
       << ",q:" << s.requests.size()
       << ",send_floor:" << (s.send_floor ? "T" : "F")
       << ",send_timem:" << (s.send_timem ? "T" : "F")
       << ",sigma:" << s.sigma << "}";
    return os;
}

// -------------------- Implementation --------------------

inline EControl::EControl(const std::string& id) : cadmium::Atomic<EControlState>(id, EControlState()) {
    acall = addInPort<fe::Floor>("acall");
    fback = addInPort<fe::TravelTime>("fback");
    timem = addOutPort<fe::TravelTime>("timem");
    floor = addOutPort<fe::Floor>("floor");
}

inline void EControl::start_next_if_idle(EControlState& s) {
    if (!s.moving && !s.requests.empty() && !s.send_timem) {
        s.target_floor = s.requests.front();
        s.requests.pop_front();

        s.timem_to_send = compute_travel_time(s.current_floor, s.target_floor);
        s.send_timem = true;
        s.moving = true;

        s.sigma = 0.0;  // output timem immediately
    }
}

inline void EControl::externalTransition(EControlState& s, double e) const {
    // account elapsed time
    if (s.sigma != std::numeric_limits<double>::infinity()) {
        s.sigma = std::max(0.0, s.sigma - e);
    }

    // 1) If we got feedback: we arrived at the current target
    if (!fback->empty()) {
        // We ignore the feedback value, using it as a completion signal.
        if (s.moving) {
            s.current_floor = s.target_floor;

            s.send_floor = true;
            s.floor_to_send = s.current_floor;

            s.moving = false;  // we can start another move after enqueueing any new calls
            s.sigma = 0.0;     // ensure an immediate output event
        }
    }

    // 2) Enqueue any new floor requests
    if (!acall->empty()) {
        const auto& bag = acall->getBag();
        for (const auto& req : bag) {
            s.requests.push_back(req);
        }
    }

    // 3) If idle, we can command the next move (may happen in same time as fback)
    start_next_if_idle(s);
}

inline void EControl::output(const EControlState& s) const {
    if (s.send_floor) {
        floor->addMessage(s.floor_to_send);
    }
    if (s.send_timem) {
        timem->addMessage(s.timem_to_send);
    }
}

inline void EControl::internalTransition(EControlState& s) const {
    // after output, clear pending output flags
    s.send_floor = false;
    s.send_timem = false;
    s.sigma = std::numeric_limits<double>::infinity();
}

inline void EControl::confluentTransition(EControlState& s, double /*e*/) const {
    // internal then external with e=0 (typical)
    internalTransition(s);
    externalTransition(s, 0.0);
}

inline double EControl::timeAdvance(const EControlState& s) const {
    return s.sigma;
}

#endif
