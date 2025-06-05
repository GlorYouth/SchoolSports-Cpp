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
        const std::vector<Result> resultsList = results_ref.getResultsList(); // 复制结果列表
        std::cout << "正在清除项目ID " << eventId << " 的 " << resultsList.size() << " 条成绩记录..." << std::endl;
        
        // 从各单位分数中扣除该项目的得分
        for (const auto& result : resultsList) {
            const int athleteId = result.getAthleteId();
            const double points = result.getPointsAwarded();
            
            auto athleteOpt = settings.athletes.getConst(athleteId);
            if (athleteOpt) {
                const int unitId = athleteOpt.value().get().getUnitId();
                auto unitOpt = settings.units.get(unitId);
                if (unitOpt) {
                    // 从单位总分中扣除此项目得分
                    unitOpt.value().get().addScore(-points);
                    std::cout << "  从单位ID " << unitId << " 的总分中扣除 " << points << " 分" << std::endl;
                }
            }
        }
        
        // 移除项目成绩记录
        resultsMap.erase(resultsIt); 
        std::cout << "成功清除项目ID " << eventId << " 的所有成绩" << std::endl;
    } else {
        std::cout << "项目ID " << eventId << " 没有成绩记录，无需清除" << std::endl;
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