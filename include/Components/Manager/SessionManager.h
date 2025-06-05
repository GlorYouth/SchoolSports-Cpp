#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <string>
#include <utility>

// 前向声明
class SystemSettings;

class SessionManager {
public:
    explicit SessionManager(SystemSettings& settings);
    
    // 设置上午/下午时间段
    void setMorningSession(const std::string& start, const std::string& end);
    void setAfternoonSession(const std::string& start, const std::string& end);
    
    // 获取上午/下午时间段
    std::pair<std::string, std::string> getMorningSession() const;
    std::pair<std::string, std::string> getAfternoonSession() const;
    
    // 检查时间格式是否有效
    static bool isValidTimeFormat(const std::string& time);
    
    // 快捷设置默认时间段
    void setDefaultSessions();

private:
    SystemSettings& settings;
};

#endif // SESSIONMANAGER_H 