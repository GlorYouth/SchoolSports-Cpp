#include "../../../include/Components/Manager/EventResultsManager.h"
#include "../../../include/Components/Core/SystemSettings.h"
#include "../../../include/utils.h"

EventResultsManager::EventResultsManager(SystemSettings& settings) : settings(settings) {}

// 内部访问方法
std::map<int, EventResults>& EventResultsManager::getResultsMap() {
    return settings._eventResultsMap;
}

const std::map<int, EventResults>& EventResultsManager::getResultsMapConst() const {
    return settings._eventResultsMap;
}

// 基本操作
bool EventResultsManager::addOrUpdate(const EventResults& er) {
    if (!settings.events.contains(er.getEventId())) {
        std::cerr << "错误: 无法为不存在的项目ID " << er.getEventId() << " 添加成绩。" << std::endl;
        return false;
    }
    getResultsMap().insert_or_assign(er.getEventId(), er); // 插入或更新
    return true;
}

void EventResultsManager::clear() {
    getResultsMap().clear();
    resetAllUnitScores(); // 清除成绩记录后，所有单位的总分也应重置
    std::cout << "所有比赛结果及单位累计分数已清除。" << std::endl;
}

void EventResultsManager::clearForEvent(const int eventId) {
    auto& resultsMap = getResultsMap();
    if (const auto resultsIt = resultsMap.find(eventId); resultsIt != resultsMap.end()) {
        const EventResults& results_ref = resultsIt->second;
        for (const auto& result : results_ref.getResultsList()) {
            if (auto athleteOpt = settings.athletes.getConst(result.getAthleteId())) {
                if (auto unitOpt = settings.units.get(athleteOpt.value().get().getUnitId())) {
                    // 从单位分数中减去此成绩的分数
                    // 假设 Unit::addScore 可以接受负值来扣分
                    unitOpt.value().get().addScore(-result.getPointsAwarded()); 
                }
            }
        }
        resultsMap.erase(resultsIt); // 移除项目成绩记录
    }
}

// 获取方法
std::optional<utils::Ref<EventResults>> EventResultsManager::get(int eventId) {
    auto& resultsMap = getResultsMap();
    if (const auto it = resultsMap.find(eventId); it != resultsMap.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<utils::RefConst<EventResults>> EventResultsManager::getConst(int eventId) const {
    const auto& resultsMap = getResultsMapConst();
    if (const auto it = resultsMap.find(eventId); it != resultsMap.end()) {
        return it->second;
    }
    return std::nullopt;
}

const std::map<int, EventResults>& EventResultsManager::getAll() const {
    return getResultsMapConst();
}

// 迭代器支持
EventResultsManager::iterator EventResultsManager::begin() {
    return getResultsMap().begin();
}

EventResultsManager::iterator EventResultsManager::end() {
    return getResultsMap().end();
}

EventResultsManager::const_iterator EventResultsManager::begin() const {
    return getResultsMapConst().begin();
}

EventResultsManager::const_iterator EventResultsManager::end() const {
    return getResultsMapConst().end();
}

// 单位得分管理
void EventResultsManager::resetAllUnitScores() {
    settings.units.resetAllScores();
}

void EventResultsManager::addScoreToUnit(int unitId, double score) {
    settings.units.addScore(unitId, score);
}

// 统计方法
size_t EventResultsManager::count() const {
    return getResultsMapConst().size();
}

bool EventResultsManager::empty() const {
    return getResultsMapConst().empty();
}

bool EventResultsManager::contains(int eventId) const {
    return getResultsMapConst().find(eventId) != getResultsMapConst().end();
} 