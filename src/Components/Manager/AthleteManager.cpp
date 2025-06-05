#include "../../../include/Components/Manager/AthleteManager.h"
#include "../../../include/Components/Core/SystemSettings.h"
#include "../../../include/Components/Core/RegistrationTransaction.h"
#include "../../../include/utils.h"
#include <algorithm>

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

// 报名相关方法，使用RegistrationTransaction
bool AthleteManager::registerForEvent(int athleteId, int eventId) {
    // 使用事务模式处理跨对象操作
    RegistrationTransaction transaction(settings);
    if (!transaction.begin()) {
        std::cerr << "错误：无法开始报名事务。" << std::endl;
        return false;
    }
    
    bool success = transaction.registerForEvent(athleteId, eventId);
    
    if (success) {
        transaction.commit();
    } else {
        transaction.rollback();
    }
    
    return success;
}

bool AthleteManager::unregisterFromEvent(int athleteId, int eventId) {
    RegistrationTransaction transaction(settings);
    if (!transaction.begin()) {
        std::cerr << "错误：无法开始取消报名事务。" << std::endl;
        return false;
    }
    
    bool success = transaction.unregisterFromEvent(athleteId, eventId);
    
    if (success) {
        transaction.commit();
    } else {
        transaction.rollback();
    }
    
    return success;
}

int AthleteManager::getMaxEventsAllowed() const {
    return settings._athleteMaxEventsAllowed;
}

// 新增：验证运动员报名数据一致性
bool AthleteManager::validateRegistrationConsistency() const {
    bool consistencyOk = true;
    
    // 检查所有运动员的报名项目
    for (const auto& [athleteId, athlete] : getAthletesMapConst()) {
        for (int eventId : athlete.getRegisteredEventIds()) {
            auto eventOpt = settings.events.getConst(eventId);
            if (!eventOpt) {
                std::cerr << "数据一致性错误: 运动员 " << athlete.getName() 
                          << " 报名了不存在的项目ID " << eventId << std::endl;
                consistencyOk = false;
                continue;
            }
            
            const CompetitionEvent& event = eventOpt.value().get();
            const auto& participants = event.getParticipantAthleteIds();
            if (std::find(participants.begin(), participants.end(), athleteId) == participants.end()) {
                std::cerr << "数据一致性错误: 运动员 " << athlete.getName() 
                          << " 报名了项目 " << event.getName() 
                          << " 但该项目的参赛者列表中没有此运动员" << std::endl;
                consistencyOk = false;
            }
        }
    }
    
    // 检查所有比赛项目的参赛运动员
    for (const auto& [eventId, event] : settings.events.getAllConst()) {
        for (int athleteId : event.get().getParticipantAthleteIds()) {
            auto athleteOpt = getConst(athleteId);
            if (!athleteOpt) {
                std::cerr << "数据一致性错误: 项目 " << event.get().getName() 
                          << " 包含不存在的运动员ID " << athleteId << std::endl;
                consistencyOk = false;
                continue;
            }
            
            const Athlete& athlete = athleteOpt.value().get();
            if (!athlete.isRegisteredForEvent(eventId)) {
                std::cerr << "数据一致性错误: 项目 " << event.get().getName() 
                          << " 包含运动员 " << athlete.getName() 
                          << " 但该运动员的报名列表中没有此项目" << std::endl;
                consistencyOk = false;
            }
        }
    }
    
    if (consistencyOk) {
        std::cout << "验证通过: 运动员报名数据一致性检查成功。" << std::endl;
    } else {
        std::cerr << "验证失败: 发现运动员报名数据一致性问题，请考虑修复。" << std::endl;
    }
    
    return consistencyOk;
} 