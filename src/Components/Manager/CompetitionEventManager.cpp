#include "../../../include/Components/Manager/CompetitionEventManager.h"
#include "../../../include/Components/Core/SystemSettings.h"
#include "../../../include/utils.h"

CompetitionEventManager::CompetitionEventManager(SystemSettings& settings) : settings(settings) {}

// 内部访问方法
std::map<int, CompetitionEvent>& CompetitionEventManager::getEventsMap() {
    return settings.getData().eventsMap;
}

const std::map<int, CompetitionEvent>& CompetitionEventManager::getEventsMapConst() const {
    return settings.getDataConst().eventsMap;
}

// 基本操作
int CompetitionEventManager::add(const std::string& eventName, EventType type, Gender genderReq, int scoreRuleId) {
    // 检查规则ID是否存在
    if (scoreRuleId != -1 && !settings.rules.contains(scoreRuleId)) {
        std::cerr << "错误：添加比赛项目失败，计分规则ID " << scoreRuleId << " 不存在。" << std::endl;
        return -1; // 规则不存在
    }
    
    // 检查项目名称是否已存在
    for (const auto& [id, event] : getEventsMapConst()) {
        if (event.getName() == eventName) {
            std::cerr << "错误：项目名称 '" << eventName << "' 已存在。" << std::endl;
            return -1; // 项目名已存在
        }
    }
    
    CompetitionEvent newEvent(eventName, type, genderReq, scoreRuleId);
    int eventId = newEvent.getId();
    getEventsMap().insert({eventId, newEvent});
    return eventId;
}

bool CompetitionEventManager::remove(int eventId) {
    auto& eventsMap = getEventsMap();
    auto eventToRemove = eventsMap.find(eventId);
    if (eventToRemove == eventsMap.end()) {
        return false; // 项目不存在
    }
    
    // 检查并处理运动员报名
    const auto& participantIds = eventToRemove->second.getParticipantAthleteIds();
    std::vector<int> athletesToUpdate(participantIds.begin(), participantIds.end());
    
    // 从所有已报名的运动员的报名列表中移除此项目
    for (int athleteId : athletesToUpdate) {
        if (auto athlete = settings.athletes.get(athleteId)) {
            athlete.value().get().unregisterFromEvent(eventId);
        }
    }

    // 移除项目
    eventsMap.erase(eventId);
    return true;
}

void CompetitionEventManager::clear() {
    auto& eventsMap = getEventsMap();
    std::vector<int> eventIds;
    for (const auto& pair : eventsMap) {
        eventIds.push_back(pair.first);
    }
    for (int id : eventIds) {
        remove(id);
    }
    eventsMap.clear();
}

void CompetitionEventManager::resetIdCounter() {
    CompetitionEvent::resetNextId(1);
}

// 获取方法
std::optional<utils::Ref<CompetitionEvent>> CompetitionEventManager::get(int eventId) {
    auto& eventsMap = getEventsMap();
    if (const auto it = eventsMap.find(eventId); it != eventsMap.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<utils::RefConst<CompetitionEvent>> CompetitionEventManager::getConst(int eventId) const {
    const auto& eventsMap = getEventsMapConst();
    if (const auto it = eventsMap.find(eventId); it != eventsMap.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::map<int, utils::RefConst<CompetitionEvent>> CompetitionEventManager::getAllConst() const {
    std::map<int, utils::RefConst<CompetitionEvent>> result;
    for (const auto& [id, event] : getEventsMapConst()) {
        result.emplace(id, std::cref(event));
    }
    return result;
}

// 迭代器支持
CompetitionEventManager::iterator CompetitionEventManager::begin() {
    return getEventsMap().begin();
}

CompetitionEventManager::iterator CompetitionEventManager::end() {
    return getEventsMap().end();
}

CompetitionEventManager::const_iterator CompetitionEventManager::begin() const {
    return getEventsMapConst().begin();
}

CompetitionEventManager::const_iterator CompetitionEventManager::end() const {
    return getEventsMapConst().end();
}

// 查找方法
std::optional<utils::Ref<CompetitionEvent>> CompetitionEventManager::findByName(const std::string& name) {
    auto& eventsMap = getEventsMap();
    for (auto& [id, event] : eventsMap) {
        if (event.getName() == name) {
            return std::ref(event);
        }
    }
    return std::nullopt;
}

std::vector<utils::RefConst<CompetitionEvent>> CompetitionEventManager::findByType(EventType type) const {
    std::vector<utils::RefConst<CompetitionEvent>> result;
    for (const auto& [id, event] : getEventsMapConst()) {
        if (event.getEventType() == type) {
            result.push_back(std::cref(event));
        }
    }
    return result;
}

std::vector<utils::RefConst<CompetitionEvent>> CompetitionEventManager::findByGenderRequirement(Gender gender) const {
    std::vector<utils::RefConst<CompetitionEvent>> result;
    for (const auto& [id, event] : getEventsMapConst()) {
        if (event.getGenderRequirement() == gender) {
            result.push_back(std::cref(event));
        }
    }
    return result;
}

std::vector<utils::RefConst<CompetitionEvent>> CompetitionEventManager::findActive() const {
    std::vector<utils::RefConst<CompetitionEvent>> result;
    for (const auto& [id, event] : getEventsMapConst()) {
        if (!event.getIsCancelled()) {
            result.push_back(std::cref(event));
        }
    }
    return result;
}

// 统计方法
size_t CompetitionEventManager::count() const {
    return getEventsMapConst().size();
}

bool CompetitionEventManager::empty() const {
    return getEventsMapConst().empty();
}

bool CompetitionEventManager::contains(int eventId) const {
    return getEventsMapConst().find(eventId) != getEventsMapConst().end();
}

bool CompetitionEventManager::contains(const std::string& eventName) const {
    for (const auto& [id, event] : getEventsMapConst()) {
        if (event.getName() == eventName) {
            return true;
        }
    }
    return false;
}

// 添加getAll方法实现
const std::map<int, CompetitionEvent>& CompetitionEventManager::getAll() const {
    return getEventsMapConst();
} 