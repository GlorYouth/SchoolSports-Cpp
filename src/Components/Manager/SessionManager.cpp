#include "../../../include/Components/Manager/SessionManager.h"
#include "../../../include/Components/Core/SystemSettings.h"
#include <regex>

SessionManager::SessionManager(SystemSettings& settings) : settings(settings) {}

void SessionManager::setMorningSession(const std::string& start, const std::string& end) {
    if (!isValidTimeFormat(start) || !isValidTimeFormat(end)) {
        throw std::invalid_argument("无效的时间格式，请使用HH:MM格式");
    }
    settings._morningSessionStart = start;
    settings._morningSessionEnd = end;
    // 同步到DataContainer
    settings.getData().morningSessionStart = start;
    settings.getData().morningSessionEnd = end;
}

void SessionManager::setAfternoonSession(const std::string& start, const std::string& end) {
    if (!isValidTimeFormat(start) || !isValidTimeFormat(end)) {
        throw std::invalid_argument("无效的时间格式，请使用HH:MM格式");
    }
    settings._afternoonSessionStart = start;
    settings._afternoonSessionEnd = end;
    // 同步到DataContainer
    settings.getData().afternoonSessionStart = start;
    settings.getData().afternoonSessionEnd = end;
}

std::pair<std::string, std::string> SessionManager::getMorningSession() const {
    return {settings._morningSessionStart, settings._morningSessionEnd};
}

std::pair<std::string, std::string> SessionManager::getAfternoonSession() const {
    return {settings._afternoonSessionStart, settings._afternoonSessionEnd};
}

bool SessionManager::isValidTimeFormat(const std::string& time) {
    std::regex timePattern("^([01]?[0-9]|2[0-3]):([0-5][0-9])$");
    return std::regex_match(time, timePattern);
}

void SessionManager::setDefaultSessions() {
    setMorningSession("08:00", "12:00");
    setAfternoonSession("14:00", "18:00");
} 