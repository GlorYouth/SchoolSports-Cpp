#include "../../../include/Components/Manager/CompetitionEventManager.h"
#include "../../../include/Components/Core/SystemSettings.h"
#include "../../../include/utils.h"

CompetitionEventManager::CompetitionEventManager(SystemSettings& settings) : settings(settings) {}

// 内部访问方法
std::map<int, CompetitionEvent>& CompetitionEventManager::getEventsMap() {
    return settings._competitionEvents;
}

const std::map<int, CompetitionEvent>& CompetitionEventManager::getEventsMapConst() const {
    return settings._competitionEvents;
}

// 基本操作
int CompetitionEventManager::add(const std::string& eventName, EventType type, Gender genderReq, int scoreRuleId) {
    CompetitionEvent newEvent(eventName, type, genderReq, scoreRuleId);
    auto& eventsMap = getEventsMap();
    int newEventId = newEvent.getId();
    eventsMap.insert({newEventId, newEvent});
    return newEventId;
}

bool CompetitionEventManager::remove(int eventId) {
    auto& eventsMap = getEventsMap();
    const auto eventIt = eventsMap.find(eventId);
    if (eventIt == eventsMap.end()) {
        return false; // 项目不存在
    }
    const CompetitionEvent& event_ref = eventIt->second;

    // 从所有已报名该项目的运动员的报名列表中移除该项目
    for (const int athleteId : event_ref.getParticipantAthleteIds()) {
        if (auto athlete = settings.athletes.get(athleteId); athlete.has_value()) {
            athlete.value().get().unregisterFromEvent(eventId);
        }
    }

    // 移除该项目的成绩记录
    settings.results.clearForEvent(eventId);

    return eventsMap.erase(eventId) > 0;
}

void CompetitionEventManager::clear() {
    // 移除项目会处理其运动员关联和成绩
    auto& eventsMap = getEventsMap();
    std::vector<int> eventIds;
    for(const auto& pair : eventsMap) {
        eventIds.push_back(pair.first);
    }
    for(int id : eventIds) {
        remove(id);
    }
    eventsMap.clear(); // 最后确保 map 清空
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
    auto map = std::map<int, utils::RefConst<CompetitionEvent>>();
    const auto& eventsMap = getEventsMapConst();
    for (const auto& [id, event] : eventsMap){
        map.insert({id, std::cref(event)});
    }
    return map;
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
            return get(id);
        }
    }
    return std::nullopt;
}

std::vector<utils::RefConst<CompetitionEvent>> CompetitionEventManager::findByType(EventType type) const {
    std::vector<utils::RefConst<CompetitionEvent>> result;
    const auto& eventsMap = getEventsMapConst();
    
    for (const auto& [id, event] : eventsMap) {
        if (event.getEventType() == type) {
            result.push_back(std::cref(event));
        }
    }
    
    return result;
}

std::vector<utils::RefConst<CompetitionEvent>> CompetitionEventManager::findByGenderRequirement(Gender gender) const {
    std::vector<utils::RefConst<CompetitionEvent>> result;
    const auto& eventsMap = getEventsMapConst();
    
    for (const auto& [id, event] : eventsMap) {
        if (event.canAthleteRegister(gender)) {
            result.push_back(std::cref(event));
        }
    }
    
    return result;
}

std::vector<utils::RefConst<CompetitionEvent>> CompetitionEventManager::findActive() const {
    std::vector<utils::RefConst<CompetitionEvent>> result;
    const auto& eventsMap = getEventsMapConst();
    
    for (const auto& [id, event] : eventsMap) {
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
    return getConst(eventId).has_value();
}

bool CompetitionEventManager::contains(const std::string& eventName) const {
    const auto& eventsMap = getEventsMapConst();
    for (const auto& [id, event] : eventsMap) {
        if (event.getName() == eventName) {
            return true;
        }
    }
    return false;
} 