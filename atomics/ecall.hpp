#ifndef ECALL_HPP
#define ECALL_HPP

#include <cadmium/modeling/devs/atomic.hpp>
#include <limits>
#include <vector>
#include <ostream>

#include "../data_structures/messages.hpp"

/**
 * ECall (Call Generator)
 * - Receives calls from inside/outside of the elevator.
 * - Outputs the requested floor(s) immediately through call_gen.
 *
 * Simplest behavior: pass-through (no additional delay), supporting message bags.
 */
class ECall : public cadmium::Atomic<struct ECallState> {
public:
    // Ports
    cadmium::Port<fe::Floor> inside_call;
    cadmium::Port<fe::Floor> outside_call;
    cadmium::Port<fe::Floor> call_gen;

    explicit ECall(const std::string& id);

    void externalTransition(ECallState& s, double e) const override;
    void internalTransition(ECallState& s) const override;
    void confluentTransition(ECallState& s, double e) const override;
    void output(const ECallState& s) const override;
    [[nodiscard]] double timeAdvance(const ECallState& s) const override;
};

// -------------------- State definition --------------------

enum class ECallPhase { idle, emitting };

struct ECallState {
    double sigma;
    ECallPhase phase;
    std::vector<fe::Floor> pending;

    explicit ECallState(ECallPhase p = ECallPhase::idle)
        : sigma(std::numeric_limits<double>::infinity()), phase(p), pending() {}
};

inline std::ostream& operator<<(std::ostream& os, const ECallState& s) {
    os << "{phase:" << (s.phase == ECallPhase::idle ? "idle" : "emitting")
       << ",pending:" << s.pending.size()
       << ",sigma:" << s.sigma << "}";
    return os;
}

// -------------------- Implementation --------------------

inline ECall::ECall(const std::string& id) : cadmium::Atomic<ECallState>(id, ECallState()) {
    inside_call  = addInPort<fe::Floor>("inside_call");
    outside_call = addInPort<fe::Floor>("outside_call");
    call_gen     = addOutPort<fe::Floor>("call_gen");
}

inline void ECall::externalTransition(ECallState& s, double e) const {
    // account elapsed time
    if (s.sigma != std::numeric_limits<double>::infinity()) {
        s.sigma = std::max(0.0, s.sigma - e);
    }

    // Collect calls (Cadmium v2 ports store a bag/vector of messages)
    if (!inside_call->empty()) {
        const auto& bag = inside_call->getBag();
        s.pending.insert(s.pending.end(), bag.begin(), bag.end());
    }
    if (!outside_call->empty()) {
        const auto& bag = outside_call->getBag();
        s.pending.insert(s.pending.end(), bag.begin(), bag.end());
    }

    // If any call received, schedule an immediate output
    if (!s.pending.empty()) {
        s.phase = ECallPhase::emitting;
        s.sigma = 0.0;
    }
}

inline void ECall::output(const ECallState& s) const {
    if (s.phase == ECallPhase::emitting) {
        for (const auto& floor : s.pending) {
            call_gen->addMessage(floor);
        }
    }
}

inline void ECall::internalTransition(ECallState& s) const {
    if (s.phase == ECallPhase::emitting) {
        s.pending.clear();
        s.phase = ECallPhase::idle;
        s.sigma = std::numeric_limits<double>::infinity();
    }
}

inline void ECall::confluentTransition(ECallState& s, double /*e*/) const {
    // DEVS confluent: internal then external (with e=0) is the typical choice
    internalTransition(s);
    externalTransition(s, 0.0);
}

inline double ECall::timeAdvance(const ECallState& s) const {
    return s.sigma;
}

#endif
