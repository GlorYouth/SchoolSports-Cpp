//
// Created by GlorYouth on 2025/6/2.
//

#include "../include/Athlete.h"
#include <algorithm> // For std::find and std::remove
#include <utility>

std::atomic<int> Athlete::nextId(1); // 初始化静态成员, ID从1开始

Athlete::Athlete(std::string name, Gender gender, int unitId)
    : name(std::move(name)), gender(gender), unitId(unitId) {
    id = nextId++; // 分配唯一ID
}

int Athlete::getId() const {
    return id;
}

std::string Athlete::getName() const {
    return name;
}

void Athlete::setName(const std::string& name) {
    this->name = name;
}

Gender Athlete::getGender() const {
    return gender;
}

void Athlete::setGender(Gender gender) {
    this->gender = gender;
}

int Athlete::getUnitId() const {
    return unitId;
}

void Athlete::setUnitId(int uId) {
    this->unitId = uId;
}

bool Athlete::registerForEvent(const int eventId, const int maxEventsAllowed) {
    if (registeredEventIds.size() >= static_cast<size_t>(maxEventsAllowed)) {
        // 已达到最大报名项目数
        return false;
    }
    if (std::ranges::find(registeredEventIds, eventId) == registeredEventIds.end()) {
        registeredEventIds.push_back(eventId);
        return true;
    }
    // 已报名该项目
    return false;
}

void Athlete::unregisterFromEvent(const int eventId) {
    registeredEventIds.erase(std::ranges::remove(registeredEventIds, eventId).begin(), registeredEventIds.end());
}

const std::vector<int>& Athlete::getRegisteredEventIds() const {
    return registeredEventIds;
}

bool Athlete::isRegisteredForEvent(int eventId) const {
    return std::ranges::find(registeredEventIds, eventId) != registeredEventIds.end();
}

int Athlete::getRegisteredEventsCount() const {
    return static_cast<int>(registeredEventIds.size());
}
