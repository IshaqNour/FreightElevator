#ifndef FE_MESSAGES_HPP
#define FE_MESSAGES_HPP

// For the simplest implementation we just use primitive types (int) as messages.
// This file exists to keep the 'data_structures' folder non-empty and to allow
// future extension (e.g., adding direction, priority, timestamps, etc.).

namespace fe {
    using Floor = int;        // requested/served floor number
    using TravelTime = int;   // travel time in minutes (1 minute per floor)
}

#endif
