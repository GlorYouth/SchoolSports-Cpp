//
// Created by GlorYouth on 2025/6/2.
//

#include "../../include/Components/Athlete.h"
#include <algorithm> // For std::find and std::remove
#include <utility>
#include <iostream> // 用于调试输出

std::atomic<int> Athlete::nextId(1); // 初始化静态成员, ID从1开始

Athlete::Athlete(std::string name, const Gender gender, const int unitId)
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
        // std::cerr << "调试: 运动员 " << name << " 已达到最大报名项目数 (" << maxEventsAllowed << ")。" << std::endl;
        return false; // 已达到最大报名项目数
    }
    if (std::ranges::find(registeredEventIds, eventId) != registeredEventIds.end()) {
        // std::cerr << "调试: 运动员 " << name << " 已报名项目ID " << eventId << "。" << std::endl;
        return false; // 已报名该项目
    }
    registeredEventIds.push_back(eventId);
    // std::cout << "调试: 运动员 " << name << " 成功报名项目ID " << eventId << "。当前报名数：" << registeredEventIds.size() << std::endl;
    return true;
}

bool Athlete::unregisterFromEvent(const int eventId) {
    auto it = std::ranges::find(registeredEventIds, eventId);
    if (it != registeredEventIds.end()) {
        registeredEventIds.erase(it);
        // std::cout << "调试: 运动员 " << name << " 成功取消报名项目ID " << eventId << "。当前报名数：" << registeredEventIds.size() << std::endl;
        return true;
    }
    // std::cerr << "调试: 运动员 " << name << " 未找到已报名的项目ID " << eventId << "，无法取消。" << std::endl;
    return false; // 未报名该项目，或已取消
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

// 实现静态方法：重置ID计数器
void Athlete::resetNextId(int startId) {
    nextId.store(startId);
}

