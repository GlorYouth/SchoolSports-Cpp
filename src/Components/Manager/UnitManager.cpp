#include "../../../include/Components/Manager/UnitManager.h"
#include "../../../include/Components/Core/SystemSettings.h"
#include "../../../include/utils.h"

UnitManager::UnitManager(SystemSettings& settings) : settings(settings) {}

// 内部访问方法
std::map<int, Unit>& UnitManager::getUnitsMap() {
    return settings.getData().unitsMap;
}

const std::map<int, Unit>& UnitManager::getUnitsMapConst() const {
    return settings.getDataConst().unitsMap;
}

// 基本操作
bool UnitManager::add(const std::string& unitName) {
    // 检查单位名是否已存在
    auto& unitsMap = getUnitsMap();
    for (const auto& [id, unit] : unitsMap) {
        if (unit.getName() == unitName) {
            std::cerr << "错误: 单位名称 '" << unitName << "' 已存在。" << std::endl;
            return false; // 单位名已存在
        }
    }
    Unit newUnit(unitName);
    unitsMap.insert({newUnit.getId(), newUnit});
    return true;
}

bool UnitManager::remove(int unitId) {
    // 注意：移除单位前，应处理该单位下的运动员
    auto& unitsMap = getUnitsMap();
    auto unitToRemove = unitsMap.find(unitId);
    if (unitToRemove == unitsMap.end()) {
        return false; // 单位不存在
    }

    // 移除该单位下的所有运动员
    std::vector<int> athletesToRemove = unitToRemove->second.getAthleteIds();
    // 使用迭代器或基于索引的循环来安全地移除，避免在遍历时修改导致的问题
    for (size_t i = 0; i < athletesToRemove.size(); ++i) {
        settings.athletes.remove(athletesToRemove[i]); // 调用 removeAthlete 来处理相关清理
    }

    return unitsMap.erase(unitId) > 0;
}

void UnitManager::clear() {
    // 清除单位前，需要确保所有关联的运动员也被处理
    auto& unitsMap = getUnitsMap();
    std::vector<int> unitIds;
    for(const auto& pair : unitsMap) {
        unitIds.push_back(pair.first);
    }
    for(int id : unitIds) {
        remove(id); // 使用自己的remove方法处理其下的运动员
    }
    unitsMap.clear(); // 最后确保 map 清空
}

void UnitManager::resetIdCounter() {
    Unit::resetNextId(1);
}

// 获取方法
std::optional<utils::Ref<Unit>> UnitManager::get(int unitId) {
    auto& unitsMap = getUnitsMap();
    if (const auto it = unitsMap.find(unitId); it != unitsMap.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<utils::RefConst<Unit>> UnitManager::getConst(int unitId) const {
    const auto& unitsMap = getUnitsMapConst();
    if (const auto it = unitsMap.find(unitId); it != unitsMap.end()) {
        return it->second;
    }
    return std::nullopt;
}

const std::map<int, Unit>& UnitManager::getAll() const {
    return getUnitsMapConst();
}

// 迭代器支持
UnitManager::iterator UnitManager::begin() {
    return getUnitsMap().begin();
}

UnitManager::iterator UnitManager::end() {
    return getUnitsMap().end();
}

UnitManager::const_iterator UnitManager::begin() const {
    return getUnitsMapConst().begin();
}

UnitManager::const_iterator UnitManager::end() const {
    return getUnitsMapConst().end();
}

// 查找方法
std::optional<utils::Ref<Unit>> UnitManager::findByName(const std::string& name) {
    const auto& allUnits = getAll();
    for (auto& [id, unit] : allUnits) {
        if (unit.getName() == name) {
            return get(id);
        }
    }
    return std::nullopt;
}

// 统计方法
size_t UnitManager::count() const {
    return getUnitsMapConst().size();
}

bool UnitManager::empty() const {
    return getUnitsMapConst().empty();
}

bool UnitManager::contains(int unitId) const {
    return getConst(unitId).has_value();
}

bool UnitManager::contains(const std::string& unitName) const {
    const auto& allUnits = getAll();
    for (const auto& [id, unit] : allUnits) {
        if (unit.getName() == unitName) {
            return true;
        }
    }
    return false;
}

// 得分管理
void UnitManager::resetAllScores() {
    auto& unitsMap = getUnitsMap();
    for (auto& [id, unit] : unitsMap) {
        unit.resetScore();
    }
}

void UnitManager::addScore(int unitId, double score) {
    if (const auto unitOpt = get(unitId)) {
        unitOpt.value().get().addScore(score);
    } else {
        std::cerr << "错误: 尝试为不存在的单位ID " << unitId << " 加分失败。" << std::endl;
    }
} 