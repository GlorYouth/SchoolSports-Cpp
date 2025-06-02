#include "../include/Unit.h"
#include <algorithm> // For std::remove
#include <utility>

std::atomic<int> Unit::nextId(1); // 初始化静态成员，ID从1开始

Unit::Unit(std::string name) : name(std::move(name)), totalScore(0.0) {
    id = nextId++; // 分配唯一ID
}

int Unit::getId() const {
    return id;
}

std::string Unit::getName() const {
    return name;
}

void Unit::setName(const std::string& name) {
    this->name = name;
}

void Unit::addAthleteId(const int athleteId) {
    // 可以检查是否已存在，避免重复添加
    if (std::ranges::find(athleteIds, athleteId) == athleteIds.end()) {
        athleteIds.push_back(athleteId);
    }
}

void Unit::removeAthleteId(const int athleteId) {
    std::erase(athleteIds, athleteId);
}

const std::vector<int>& Unit::getAthleteIds() const {
    return athleteIds;
}

void Unit::addScore(double score) {
    totalScore += score;
}

double Unit::getTotalScore() const {
    return totalScore;
}

void Unit::resetScore() {
    totalScore = 0.0;
}
