#include "Event.h"

Event::Event(const std::string& name, EventType type, GenderCategory gender, int duration, int min, int max)
    : name(name), type(type), gender(gender), duration(duration), minParticipants(min), maxParticipants(max) {}

std::string Event::getName() const {
    return name;
}

EventType Event::getType() const {
    return type;
}

GenderCategory Event::getGender() const {
    return gender;
}

int Event::getDuration() const {
    return duration;
}

int Event::getMinParticipants() const {
    return minParticipants;
}

int Event::getMaxParticipants() const {
    return maxParticipants;
}

void Event::setDuration(int new_duration) {
    this->duration = new_duration;
}

void Event::setMinParticipants(int min) {
    this->minParticipants = min;
}

void Event::setMaxParticipants(int max) {
    this->maxParticipants = max;
}

void Event::addParticipant(Athlete* athlete) {
    participants.push_back(athlete);
}

int Event::getParticipantCount() const {
    return participants.size();
}

const std::vector<Athlete*>& Event::getParticipants() const {
    return participants;
}

void Event::setResults(const std::vector<Result>& res) {
    this->results = res;
}

const std::vector<Result>& Event::getResults() const {
    return results;
}

bool Event::hasResults() const {
    return !results.empty();
} 