//
// Created by GlorYouth on 2025/6/2.
//

#include "../include/CompetitionEvent.h"
#include "../include/Athlete.h" // 需要访问 Athlete 的性别
#include <algorithm> // For std::remove
#include <utility>

std::atomic<int> CompetitionEvent::nextId(1); // 初始化静态成员, ID从1开始

CompetitionEvent::CompetitionEvent(std::string name, EventType type, Gender genderReq)
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

void CompetitionEvent::addParticipant(int athleteId) {
    // 可以检查是否已存在
    if (std::ranges::find(participantAthleteIds, athleteId) == participantAthleteIds.end()) {
        participantAthleteIds.push_back(athleteId);
    }
}

void CompetitionEvent::removeParticipant(int athleteId) {
    participantAthleteIds.erase(std::remove(participantAthleteIds.begin(), participantAthleteIds.end(), athleteId), participantAthleteIds.end());
}

const std::vector<int>& CompetitionEvent::getParticipantAthleteIds() const {
    return participantAthleteIds;
}

int CompetitionEvent::getParticipantCount() const {
    return static_cast<int>(participantAthleteIds.size());
}

void CompetitionEvent::setScoreRuleId(int ruleId) {
    this->scoreRuleId = ruleId;
}

int CompetitionEvent::getScoreRuleId() const {
    return scoreRuleId;
}

void CompetitionEvent::setCancelled(bool cancelled) {
    this->isCancelled = cancelled;
}

bool CompetitionEvent::getIsCancelled() const {
    return isCancelled;
}

bool CompetitionEvent::canAthleteRegister(Gender athleteGender) const {
    if (this->genderRequirement == Gender::UNSPECIFIED) {
        return true; // 项目不限性别
    }
    return this->genderRequirement == athleteGender;
}
