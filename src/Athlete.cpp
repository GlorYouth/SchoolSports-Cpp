#include "Athlete.h"
#include "Event.h" // Include full header for method implementations
#include <algorithm> // 确保 std::remove 被正确引入

Athlete::Athlete(const std::string& name, const std::string& id, bool isMale, Unit* unit)
    : name(name), id(id), isMale(isMale), unit(unit), individualScore(0) {}

std::string Athlete::getName() const {
    return name;
}

std::string Athlete::getId() const {
    return id;
}

bool Athlete::getIsMale() const {
    return isMale;
}

Unit* Athlete::getUnit() const {
    return unit;
}

int Athlete::getIndividualScore() const {
    return individualScore;
}

bool Athlete::canRegister(int maxEvents) {
    return events.size() < maxEvents;
}

void Athlete::registerForEvent(Event* event) {
    events.push_back(event);
}

const std::vector<Event*>& Athlete::getRegisteredEvents() const {
    return events;
}

void Athlete::addScore(int points) {
    individualScore += points;
}

bool Athlete::isRegisteredFor(const Event* event) const {
    for (const auto& reg_event : events) {
        if (reg_event == event) {
            return true;
        }
    }
    return false;
}

void Athlete::deregisterFromEvent(const Event* event) {
    auto it = std::remove(events.begin(), events.end(), event);
    events.erase(it, events.end());
} 