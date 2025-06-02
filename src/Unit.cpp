#include "../include/Unit.h"
#include <algorithm> // For std::remove

std::atomic<int> Unit::nextId(1); // 初始化静态成员，ID从1开始

Unit::Unit(const std::string& name) : name(name), totalScore(0.0) {
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

void Unit::addAthleteId(int athleteId) {
    // 可以检查是否已存在，避免重复添加
    if (std::find(athleteIds.begin(), athleteIds.end(), athleteId) == athleteIds.end()) {
        athleteIds.push_back(athleteId);
    }
}

void Unit::removeAthleteId(int athleteId) {
    athleteIds.erase(std::remove(athleteIds.begin(), athleteIds.end(), athleteId), athleteIds.end());
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
