#include "../../../include/Components/Manager/AthleteManager.h"
#include "../../../include/Components/Core/SystemSettings.h"
#include "../../../include/Components/Core/RegistrationTransaction.h"
#include "../../../include/utils.h"
#include <algorithm>

AthleteManager::AthleteManager(SystemSettings& settings) : settings(settings) {}

// 内部访问方法
std::map<int, Athlete>& AthleteManager::getAthletesMap() const{
    return settings.getData().athletesMap;
}

const std::map<int, Athlete>& AthleteManager::getAthletesMapConst() const {
    return settings.getDataConst().athletesMap;
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
    
    // 在进行任何修改前，先复制所有必要信息
    const int unitId = athleteIt->second.getUnitId();
    const std::string athleteName = athleteIt->second.getName();
    std::vector<int> registeredEventIds = athleteIt->second.getRegisteredEventIds(); // 复制报名项目列表
    
    // 从所属单位中移除运动员ID
    if (const auto unit = settings.units.get(unitId)) {
        unit.value().get().removeAthleteId(athleteId);
    }

    // 从所有其报名的项目中移除该运动员的参与记录
    for (const int eventId : registeredEventIds) {
        if (auto event = settings.events.get(eventId); event.has_value()) {
            event.value().get().removeParticipant(athleteId);
        }
    }

    // 从成绩记录中移除该运动员的成绩 (如果需要)
    // 这是一个更复杂的操作，可能需要遍历所有EventResults
    // 为简化，此处不直接处理成绩的移除，成绩查询时若运动员不存在则忽略

    bool removed = athletesMap.erase(athleteId) > 0;
    if (removed) {
        std::cout << "成功移除运动员: " << athleteName << " (ID: " << athleteId << ")" << std::endl;
    }
    return removed;
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
    // 使用事务模式处理跨对象操作
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
    return settings.getAthleteMaxEventsAllowed();
}

bool AthleteManager::validateRegistrationConsistency() const {
    // 这是一个全面的验证，确保系统状态的一致性
    // 检查每个运动员的报名记录与每个比赛项目的参与者记录是否一致
    
    bool isConsistent = true;
    
    // 1. 检查运动员视角
    for (const auto& [athleteId, athlete] : getAthletesMapConst()) {
        for (const int eventId : athlete.getRegisteredEventIds()) {
            auto eventOpt = settings.events.getConst(eventId);
            if (!eventOpt.has_value()) {
                std::cerr << "错误: 运动员 " << athlete.getName() << " (ID: " << athleteId 
                       << ") 报名了不存在的项目 ID: " << eventId << std::endl;
                isConsistent = false;
                continue;
            }
            
            const auto& event = eventOpt.value().get();
            const auto& participants = event.getParticipantAthleteIds();
            if (std::find(participants.begin(), participants.end(), athleteId) == participants.end()) {
                std::cerr << "错误: 运动员 " << athlete.getName() << " (ID: " << athleteId 
                       << ") 报名了项目 '" << event.getName() 
                       << "' 但未在项目参与者列表中找到" << std::endl;
                isConsistent = false;
            }
        }
    }
    
    // 2. 检查比赛项目视角
    for (const auto& [eventId, event] : settings.events.getAll()) {
        for (const int athleteId : event.getParticipantAthleteIds()) {
            auto athleteOpt = getConst(athleteId);
            if (!athleteOpt.has_value()) {
                std::cerr << "错误: 项目 '" << event.getName() << "' (ID: " << eventId 
                       << ") 包含不存在的参与者 ID: " << athleteId << std::endl;
                isConsistent = false;
                continue;
            }
            
            const auto& athlete = athleteOpt.value().get();
            const auto& registeredEvents = athlete.getRegisteredEventIds();
            if (std::find(registeredEvents.begin(), registeredEvents.end(), eventId) == registeredEvents.end()) {
                std::cerr << "错误: 项目 '" << event.getName() << "' (ID: " << eventId 
                       << ") 包含参与者 " << athlete.getName() 
                       << " 但该运动员未报名此项目" << std::endl;
                isConsistent = false;
            }
        }
    }
    
    return isConsistent;
} 