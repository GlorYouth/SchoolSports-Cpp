

#include "../../../include/Components/Core/CompetitionEvent.h"
#include "../../../include/Components/Core/Athlete.h"
#include <algorithm> // For std::remove, std::find
#include <utility>
#include <iostream> // 用于调试输出

std::atomic<int> CompetitionEvent::nextId(1); // 初始化静态成员, ID从1开始

CompetitionEvent::CompetitionEvent(std::string name, const EventType type, const Gender genderReq, const int scoreRuleId, const int durationMinutes)
    : name(std::move(name)), type(type), genderRequirement(genderReq), scoreRuleId(scoreRuleId), isCancelled(false), durationMinutes(durationMinutes) {
    id = nextId++; // 分配唯一ID
}

int CompetitionEvent::getId() const {
    return id;
}

void CompetitionEvent::setId(int newId) {
    id = newId;
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

// 实现静态方法：重置ID计数器
void CompetitionEvent::resetNextId(int startId) {
    nextId.store(startId);
}

// 时间格式验证辅助函数："HH:MM"
bool validateTimeFormat(const std::string& timeStr) {
    // 1. 检查长度是否为5
    if (timeStr.size() != 5) {
        std::cerr << "时间格式错误: 长度应为5个字符 (HH:MM), 收到 " << timeStr.size() << " 个字符。" << std::endl;
        return false;
    }
    
    // 2. 检查分隔符
    if (timeStr[2] != ':') {
        std::cerr << "时间格式错误: 中间字符应为':' (HH:MM)，收到 '" << timeStr[2] << "'。" << std::endl;
        return false;
    }
    
    // 3. 检查小时和分钟是否为数字
    const std::string hourStr = timeStr.substr(0, 2);
    const std::string minStr = timeStr.substr(3, 2);
    
    try {
        // 转换为整数
        const int hour = std::stoi(hourStr);
        const int min = std::stoi(minStr);
        
        // 4. 验证范围
        if (hour < 0 || hour > 23) {
            std::cerr << "时间格式错误: 小时应在0-23之间，收到 " << hour << "。" << std::endl;
            return false;
        }
        
        if (min < 0 || min > 59) {
            std::cerr << "时间格式错误: 分钟应在0-59之间，收到 " << min << "。" << std::endl;
            return false;
        }
        
        return true;
    } catch (const std::invalid_argument&) {
        std::cerr << "时间格式错误: 小时或分钟不是有效数字。" << std::endl;
        return false;
    } catch (const std::out_of_range&) {
        std::cerr << "时间格式错误: 小时或分钟数值超出范围。" << std::endl;
        return false;
    }
}

// ========== 新增：持续时间、场地、时间段相关方法 ==========
void CompetitionEvent::setDurationMinutes(int minutes) {
    durationMinutes = minutes;
}
int CompetitionEvent::getDurationMinutes() const {
    return durationMinutes;
}
void CompetitionEvent::setVenue(const std::string& v) {
    venue = v;
}
std::string CompetitionEvent::getVenue() const {
    return venue;
}
void CompetitionEvent::setStartTime(const std::string& t) {
    // 添加时间格式验证
    if (validateTimeFormat(t)) {
        startTime = t;
    } else {
        std::cerr << "警告: 项目 \"" << name << "\" (ID: " << id 
                 << ") 设置开始时间失败，提供的时间格式 \"" << t << "\" 无效。应为 HH:MM 格式。" << std::endl;
    }
}
void CompetitionEvent::setEndTime(const std::string& t) {
    // 添加时间格式验证
    if (validateTimeFormat(t)) {
        endTime = t;
    } else {
        std::cerr << "警告: 项目 \"" << name << "\" (ID: " << id 
                 << ") 设置结束时间失败，提供的时间格式 \"" << t << "\" 无效。应为 HH:MM 格式。" << std::endl;
    }
}
std::string CompetitionEvent::getStartTime() const {
    return startTime;
}
std::string CompetitionEvent::getEndTime() const {
    return endTime;
}

