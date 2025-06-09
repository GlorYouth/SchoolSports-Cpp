#include "Unit.h"
#include "Athlete.h" // 需要 Athlete 的完整定义

Unit::Unit(const std::string& name) : name(name), totalScore(0) {}

std::string Unit::getName() const {
    return name;
}

int Unit::getTotalScore() const {
    return totalScore;
}

const std::vector<std::unique_ptr<Athlete>>& Unit::getAthletes() const {
    return athletes;
}

Athlete* Unit::addAthlete(const std::string& name, const std::string& id, bool isMale) {
    // 创建一个新的 Athlete 并将其所有权移交到 vector 中
    auto newAthlete = std::make_unique<Athlete>(name, id, isMale, this);
    Athlete* ptr = newAthlete.get();
    athletes.push_back(std::move(newAthlete));
    return ptr;
}

void Unit::addScore(int points) {
    totalScore += points;
}

void Unit::setTotalScore(int score) {
    totalScore = score;
}

Athlete* Unit::findAthlete(const std::string& athleteId) {
    for (const auto& athletePtr : athletes) {
        if (athletePtr->getId() == athleteId) {
            return athletePtr.get();
        }
    }
    return nullptr;
} 