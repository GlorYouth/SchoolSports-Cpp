//
// Created by GlorYouth on 2025/6/2.
//

#include "../include/CompetitionEvent.h"
#include "../include/Athlete.h" // 需要访问 Athlete 的性别
#include <algorithm> // For std::remove, std::find
#include <utility>
#include <iostream> // 用于调试输出

std::atomic<int> CompetitionEvent::nextId(1); // 初始化静态成员, ID从1开始

CompetitionEvent::CompetitionEvent(std::string name, const EventType type, const Gender genderReq)
    : name(std::move(name)), type(type), genderRequirement(genderReq), scoreRuleId(-1), isCancelled(false) {
    id = nextId++; // 分配唯一ID
}

int CompetitionEvent::getId() const {
    return id;
}

std::string CompetitionEvent::getName() const {
    return name;
}

void CompetitionEvent::setName(const std::string& name) {
    this->name = name;
}

EventType CompetitionEvent::getEventType() const {
    return type;
}

void CompetitionEvent::setEventType(EventType type) {
    this->type = type;
}

Gender CompetitionEvent::getGenderRequirement() const {
    return genderRequirement;
}

void CompetitionEvent::setGenderRequirement(Gender genderReq) {
    this->genderRequirement = genderReq;
}

bool CompetitionEvent::addParticipant(int athleteId) {
    if (std::ranges::find(participantAthleteIds, athleteId) == participantAthleteIds.end()) {
        participantAthleteIds.push_back(athleteId);
        // std::cout << "调试: 项目 " << name << " 添加参赛者ID " << athleteId << "。当前人数：" << participantAthleteIds.size() << std::endl;
        return true;
    }
    // std::cerr << "调试: 项目 " << name << " 参赛者ID " << athleteId << " 已存在。" << std::endl;
    return false; // 运动员已在该项目中
}

bool CompetitionEvent::removeParticipant(int athleteId) {
    auto it = std::ranges::find(participantAthleteIds, athleteId);
    if (it != participantAthleteIds.end()) {
        participantAthleteIds.erase(it);
        // std::cout << "调试: 项目 " << name << " 移除参赛者ID " << athleteId << "。当前人数：" << participantAthleteIds.size() << std::endl;
        return true;
    }
    // std::cerr << "调试: 项目 " << name << " 未找到参赛者ID " << athleteId << "，无法移除。" << std::endl;
    return false; // 运动员未在该项目中
}

const std::vector<int>& CompetitionEvent::getParticipantAthleteIds() const {
    return participantAthleteIds;
}

int CompetitionEvent::getParticipantCount() const {
    return static_cast<int>(participantAthleteIds.size());
}

void CompetitionEvent::setScoreRuleId(const int ruleId) {
    this->scoreRuleId = ruleId;
}

int CompetitionEvent::getScoreRuleId() const {
    return scoreRuleId;
}

void CompetitionEvent::setCancelled(const bool cancelled) {
    // std::cout << "调试: 项目 " << name << " (ID: " << id << ") 设置取消状态为: " << (cancelled ? "是" : "否") << std::endl;
    this->isCancelled = cancelled;
}

bool CompetitionEvent::getIsCancelled() const {
    // std::cout << "调试: 项目 " << name << " (ID: " << id << ") 获取取消状态: " << (isCancelled ? "是" : "否") << std::endl;
    return isCancelled;
}

bool CompetitionEvent::canAthleteRegister(Gender athleteGender) const {
    if (this->genderRequirement == Gender::UNSPECIFIED) {
        return true; // 项目不限性别
    }
    return this->genderRequirement == athleteGender;
}

