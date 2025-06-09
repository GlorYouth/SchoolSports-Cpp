#include "Unit.h"
#include "Athlete.h"
#include <iostream>

Unit::Unit(const std::string& name) : name(name), score(0) {}

void Unit::addAthlete(const std::string& athleteId, const std::string& athleteName, const std::string& gender) {
    athletes.emplace_back(std::make_unique<Athlete>(athleteName, athleteId, gender, this));
}

void Unit::addScore(int points) {
    this->score += points;
}

Athlete* Unit::findAthlete(const std::string& athleteId) {
    for (const auto& athlete : athletes) {
        if (athlete->id == athleteId) {
            return athlete.get();
        }
    }
    return nullptr;
} 