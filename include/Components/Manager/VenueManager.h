#ifndef VENUEMANAGER_H
#define VENUEMANAGER_H

#include <string>
#include <set>

// 前向声明
class SystemSettings;

class VenueManager {
public:
    using iterator = std::set<std::string>::iterator;
    using const_iterator = std::set<std::string>::const_iterator;
    
    explicit VenueManager(SystemSettings& settings);
    
    // 基本操作
    bool add(const std::string& venueName);
    bool remove(const std::string& venueName);
    void clear();
    
    // 获取方法
    const std::set<std::string>& getAll() const;
    
    // 迭代器支持
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    
    // 统计方法
    size_t count() const;
    bool empty() const;
    bool contains(const std::string& venueName) const;
    
    // 批量导入场地
    void addDefaultVenues();

private:
    SystemSettings& settings;
    // 内部方法，直接操作场地数据
    std::set<std::string>& getVenuesSet();
    const std::set<std::string>& getVenuesSetConst() const;
};

#endif // VENUEMANAGER_H 