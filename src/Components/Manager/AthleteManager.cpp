#include "../../../include/Components/Manager/AthleteManager.h"
#include "../../../include/Components/Core/SystemSettings.h"
#include "../../../include/utils.h"

AthleteManager::AthleteManager(SystemSettings& settings) : settings(settings) {}

// 内部访问方法
std::map<int, Athlete>& AthleteManager::getAthletesMap() const{
    return settings._athletesMap;
}

const std::map<int, Athlete>& AthleteManager::getAthletesMapConst() const {
    return settings._athletesMap;
}

// 基本操作
bool AthleteManager::add(const std::string& name, Gender gender, int unitId) {
    const auto optional_unit = settings.units.get(unitId);
    if (!optional_unit.has_value()) {
        std::cerr << "错误: 添加运动员失败，单位ID " << unitId << " 不存在。" << std::endl;
        return false; // 单位不存在
    }
    Athlete newAthlete(name, gender, unitId);
    auto& athletesMap = getAthletesMap();
    athletesMap.insert({newAthlete.getId(), newAthlete});
    optional_unit.value().get().addAthleteId(newAthlete.getId()); // 将运动员ID关联到单位
    return true;
}

bool AthleteManager::remove(int athleteId) {
    auto& athletesMap = getAthletesMap();
    const auto athleteIt = athletesMap.find(athleteId);
    if (athleteIt == athletesMap.end()) {
        return false; // 运动员不存在
    }
    const Athlete& athlete_ref = athleteIt->second;

    // 从所属单位中移除运动员ID
    if (const auto unit = settings.units.get(athlete_ref.getUnitId())) {
        unit.value().get().removeAthleteId(athleteId);
    }

    // 从所有其报名的项目中移除该运动员的参与记录
    for (const int eventId : athlete_ref.getRegisteredEventIds()) {
        if (auto event = settings.events.get(eventId); event.has_value()) {
            event.value().get().removeParticipant(athleteId);
        }
    }

    // 从成绩记录中移除该运动员的成绩 (如果需要)
    // 这是一个更复杂的操作，可能需要遍历所有EventResults
    // 为简化，此处不直接处理成绩的移除，成绩查询时若运动员不存在则忽略

    return athletesMap.erase(athleteId) > 0;
}

void AthleteManager::clear() {
    // 移除运动员会处理其在单位和项目中的关联
    auto& athletesMap = getAthletesMap();
    std::vector<int> athleteIds;
    for(const auto& pair : athletesMap) {
        athleteIds.push_back(pair.first);
    }
    for(int id : athleteIds) {
        remove(id);
    }
    athletesMap.clear(); // 最后确保 map 清空
}

void AthleteManager::resetIdCounter() {
    Athlete::resetNextId(1);
}

// 获取方法
std::optional<utils::Ref<Athlete>> AthleteManager::get(int athleteId) {
    auto& athletesMap = getAthletesMap();
    if (const auto it = athletesMap.find(athleteId); it != athletesMap.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<utils::RefConst<Athlete>> AthleteManager::getConst(int athleteId) const {
    const auto& athletesMap = getAthletesMapConst();
    if (const auto it = athletesMap.find(athleteId); it != athletesMap.end()) {
        return it->second;
    }
    return std::nullopt;
}

const std::map<int, Athlete>& AthleteManager::getAll() const {
    return getAthletesMapConst();
}

// 迭代器支持
AthleteManager::iterator AthleteManager::begin() {
    return getAthletesMap().begin();
}

AthleteManager::iterator AthleteManager::end() {
    return getAthletesMap().end();
}

AthleteManager::const_iterator AthleteManager::begin() const {
    return getAthletesMapConst().begin();
}

AthleteManager::const_iterator AthleteManager::end() const {
    return getAthletesMapConst().end();
}

// 查找方法
std::optional<utils::Ref<Athlete>> AthleteManager::findByName(const std::string& name) {
    const auto& allAthletes = getAll();
    for (auto& [id, athlete] : allAthletes) {
        if (athlete.getName() == name) {
            return get(id);
        }
    }
    return std::nullopt;
}

std::vector<utils::Ref<Athlete>> AthleteManager::findByUnitId(int unitId) {
    std::vector<utils::Ref<Athlete>> result;
    const auto& allAthletes = getAll();
    
    for (auto& [id, athlete] : allAthletes) {
        if (athlete.getUnitId() == unitId) {
            auto athleteRef = get(id);
            if (athleteRef.has_value()) {
                result.push_back(athleteRef.value());
            }
        }
    }
    
    return result;
}

std::vector<utils::RefConst<Athlete>> AthleteManager::getAllConst() const {
    std::vector<utils::RefConst<Athlete>> refs;
    const auto& athletesMap = getAthletesMapConst();
    refs.reserve(athletesMap.size());
    for (const auto& [id, athlete] : athletesMap) {
        refs.push_back(std::cref(athlete));
    }
    return refs;
}

// 统计方法
size_t AthleteManager::count() const {
    return getAthletesMapConst().size();
}

bool AthleteManager::empty() const {
    return getAthletesMapConst().empty();
}

bool AthleteManager::contains(int athleteId) const {
    return getConst(athleteId).has_value();
}

bool AthleteManager::contains(const std::string& name) const {
    const auto& allAthletes = getAll();
    for (const auto& [id, athlete] : allAthletes) {
        if (athlete.getName() == name) {
            return true;
        }
    }
    return false;
}

// 报名相关
bool AthleteManager::registerForEvent(int athleteId, int eventId) {
    auto athleteOpt = get(athleteId);
    auto eventOpt = settings.events.get(eventId);

    if (!athleteOpt) {
        std::cerr << "错误：报名失败，运动员ID " << athleteId << " 未找到。" << std::endl;
        return false;
    }
    if (!eventOpt) {
        std::cerr << "错误：报名失败，项目ID " << eventId << " 未找到。" << std::endl;
        return false;
    }

    Athlete& athlete = athleteOpt.value().get();
    CompetitionEvent& event = eventOpt.value().get();

    // 检查性别是否符合
    if (!event.canAthleteRegister(athlete.getGender())) {
        std::cerr << "错误：运动员 " << athlete.getName() << " (ID: " << athleteId
                  << ") 的性别不符合项目 " << event.getName() << " (ID: " << eventId << ") 的要求。" << std::endl;
        return false;
    }

    // 检查运动员报名项目数量是否超限
    int maxAllowed = getMaxEventsAllowed();
    if (athlete.getRegisteredEventsCount() >= maxAllowed) {
        std::cerr << "错误：运动员 " << athlete.getName() << " (ID: " << athleteId
                  << ") 已达到最大报名项目数 (" << maxAllowed << ")。" << std::endl;
        return false;
    }

    // 尝试将运动员注册到项目中
    if (!athlete.registerForEvent(eventId, maxAllowed)) {
        // registerForEvent 内部可能已打印错误，或有其他逻辑 (如重复报名)
        std::cerr << "运动员 " << athlete.getName() << " (ID: " << athleteId
                  << ") 报名项目 " << event.getName() << " (ID: " << eventId << ") 失败（运动员侧）。" << std::endl;
        return false;
    }

    // 尝试将项目添加到运动员的参与列表
    if (!event.addParticipant(athleteId)) {
        // 如果 addParticipant 失败，需要回滚运动员的报名操作
        athlete.unregisterFromEvent(eventId);
        std::cerr << "运动员 " << athlete.getName() << " (ID: " << athleteId
                  << ") 报名项目 " << event.getName() << " (ID: " << eventId << ") 失败（项目侧）。" << std::endl;
        return false;
    }

    std::cout << "信息：运动员 " << athlete.getName() << " (ID: " << athleteId
              << ") 成功报名项目 " << event.getName() << " (ID: " << eventId << ")。" << std::endl;
    return true;
}

bool AthleteManager::unregisterFromEvent(int athleteId, int eventId) {
    const auto athlete = get(athleteId);
    if (!athlete.has_value()) {
        return false;
    }

    if (const auto eventOpt = settings.events.get(eventId); eventOpt.has_value()) {
        eventOpt.value().get().removeParticipant(athleteId);
    }
    
    return athlete.value().get().unregisterFromEvent(eventId);
}

int AthleteManager::getMaxEventsAllowed() const {
    return settings._athleteMaxEventsAllowed;
} 