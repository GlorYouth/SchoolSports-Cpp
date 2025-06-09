#include "Athlete.h"
#include "Event.h"
#include <algorithm>

Athlete::Athlete(const std::string& name, const std::string& id, const std::string& gender, Unit* unit)
    : name(name), id(id), gender(gender), unit(unit) {}

bool Athlete::canRegister(int maxEvents) const {
    return registeredEvents.size() < maxEvents;
}

void Athlete::registerForEvent(const std::string& eventName) {
    if (!isRegisteredFor(eventName)) {
        registeredEvents.push_back(eventName);
    }
}

void Athlete::unregisterFromEvent(const std::string& eventName) {
    auto it = std::remove(registeredEvents.begin(), registeredEvents.end(), eventName);
    registeredEvents.erase(it, registeredEvents.end());
}

bool Athlete::isRegisteredFor(const std::string& eventName) const {
    return std::find(registeredEvents.begin(), registeredEvents.end(), eventName) != registeredEvents.end();
} 