#include "Event.h"
#include <algorithm>

Event::Event(const std::string& name, const std::string& gender, bool isTimeBased)
    : name(name), gender(gender), isTimeBased(isTimeBased), isCancelled(false) {}

void Event::addAthlete(const std::string& athleteId) {
    // Prevent duplicate entries
    if (std::find(registeredAthletes.begin(), registeredAthletes.end(), athleteId) == registeredAthletes.end()) {
        registeredAthletes.push_back(athleteId);
    }
}

void Event::removeAthlete(const std::string& athleteId) {
    auto it = std::remove(registeredAthletes.begin(), registeredAthletes.end(), athleteId);
    if (it != registeredAthletes.end()) {
        registeredAthletes.erase(it, registeredAthletes.end());
    }
}