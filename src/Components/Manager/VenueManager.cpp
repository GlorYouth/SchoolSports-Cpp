#include "../../../include/Components/Manager/VenueManager.h"
#include "../../../include/Components/Core/SystemSettings.h"

VenueManager::VenueManager(SystemSettings& settings) : settings(settings) {}

// 内部访问方法
std::set<std::string>& VenueManager::getVenuesSet() {
    return settings.getData().venues;
}

const std::set<std::string>& VenueManager::getVenuesSetConst() const {
    return settings.getDataConst().venues;
}

// 基本操作
bool VenueManager::add(const std::string& venueName) {
    if (venueName.empty()) {
        return false;
    }
    // 添加场地，若已存在则返回false
    return getVenuesSet().insert(venueName).second;
}

bool VenueManager::remove(const std::string& venueName) {
    // 移除场地，若不存在则返回false
    return getVenuesSet().erase(venueName) > 0;
}

void VenueManager::clear() {
    getVenuesSet().clear();
}

// 获取方法
const std::set<std::string>& VenueManager::getAll() const {
    return getVenuesSetConst();
}

// 迭代器支持
VenueManager::iterator VenueManager::begin() {
    return getVenuesSet().begin();
}

VenueManager::iterator VenueManager::end() {
    return getVenuesSet().end();
}

VenueManager::const_iterator VenueManager::begin() const {
    return getVenuesSetConst().begin();
}

VenueManager::const_iterator VenueManager::end() const {
    return getVenuesSetConst().end();
}

// 统计方法
size_t VenueManager::count() const {
    return getVenuesSetConst().size();
}

bool VenueManager::empty() const {
    return getVenuesSetConst().empty();
}

bool VenueManager::contains(const std::string& venueName) const {
    return getVenuesSetConst().find(venueName) != getVenuesSetConst().end();
}

// 批量导入场地
void VenueManager::addDefaultVenues() {
    add("主体育场");
    add("室内体育馆");
    add("游泳馆");
    add("田径场");
    add("篮球场");
    add("排球场");
    add("网球场");
} 