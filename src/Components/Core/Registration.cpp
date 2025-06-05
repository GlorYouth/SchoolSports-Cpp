#include "../../../include/Components/Core/Registration.h"
#include "../../../include/Components/Core/SystemSettings.h"
#include "../../../include/Components/Core/Athlete.h"
#include "../../../include/Components/Core/CompetitionEvent.h"
#include "../../../include/UI/UIManager.h"
#include <iostream> // 用于输出信息
#include <ranges>

Registration::Registration(SystemSettings& sysSettings) : settings(sysSettings) {}

// 辅助函数：将"HH:MM"字符串转为分钟数
static int timeStringToMinutes(const std::string& timeStr) {
    if (timeStr.size() != 5 || timeStr[2] != ':') return -1;
    int hour = std::stoi(timeStr.substr(0, 2));
    int min = std::stoi(timeStr.substr(3, 2));
    return hour * 60 + min;
}

bool Registration::registerAthleteForEvent(int athleteId, int eventId) {
    const auto athleteOpt = settings.athletes.get(athleteId);
    const auto eventOpt = settings.events.get(eventId);

    if (!athleteOpt.has_value()) {
        std::cerr << "报名失败: 运动员ID " << athleteId << " 不存在。" << std::endl;
        return false;
    }
    Athlete& athlete_ref = athleteOpt.value().get(); // 使用引用

    if (!eventOpt.has_value()) {
        std::cerr << "报名失败: 项目ID " << eventId << " 不存在。" << std::endl;
        return false;
    }
    CompetitionEvent& event_ref = eventOpt.value().get(); // 使用引用

    // 新增：报名时间冲突检测
    // 仅检测有时间信息的项目
    std::string newStart = event_ref.getStartTime();
    std::string newEnd = event_ref.getEndTime();
    int newStartMin = timeStringToMinutes(newStart);
    int newEndMin = timeStringToMinutes(newEnd);
    if (newStartMin >= 0 && newEndMin > newStartMin) {
        for (int regEventId : athlete_ref.getRegisteredEventIds()) {
            if (regEventId == eventId) continue; // 跳过自己
            auto regEventOpt = settings.events.getConst(regEventId);
            if (!regEventOpt) continue;
            const CompetitionEvent& regEvent = regEventOpt.value().get();
            int regStartMin = timeStringToMinutes(regEvent.getStartTime());
            int regEndMin = timeStringToMinutes(regEvent.getEndTime());
            if (regStartMin >= 0 && regEndMin > regStartMin) {
                // 检查时间重叠
                if (!(newEndMin <= regStartMin || newStartMin >= regEndMin)) {
                    // 有重叠，禁止报名
                    std::string info = "与已报名项目[" + regEvent.getName() + "]时间重叠(" + regEvent.getStartTime() + "-" + regEvent.getEndTime() + ")，无法报名。";
                    UIManager::showRegistrationConflictMessage(info);
                    return false;
                }
                // 检查间隔
                int interval = std::min(abs(newStartMin - regEndMin), abs(regStartMin - newEndMin));
                if (interval < 30) {
                    std::string warn = "与已报名项目[" + regEvent.getName() + "]间隔不足30分钟(" + std::to_string(interval) + "分钟)。";
                    UIManager::showRegistrationIntervalWarning(warn);
                }
            }
        }
    }

    // 委托给AthleteManager来执行报名操作，它会使用RegistrationTransaction确保一致性
    return settings.athletes.registerForEvent(athleteId, eventId);
}

bool Registration::unregisterAthleteFromEvent(int athleteId, int eventId) {
    // 委托给AthleteManager来执行取消报名操作，它会使用RegistrationTransaction确保一致性
    bool success = settings.athletes.unregisterFromEvent(athleteId, eventId);
    
    // 取消报名成功后，检查项目是否会因此人数不足而需要取消
    if (success) {
        auto eventOpt = settings.events.get(eventId);
        if (eventOpt.has_value()) {
            CompetitionEvent& event_ref = eventOpt.value().get();
            
            // 只对未取消的项目进行检查
            if (!event_ref.getIsCancelled()) {
                // 获取项目关联的计分规则ID
                int ruleId = event_ref.getScoreRuleId();
                
                // 如果项目没有关联计分规则，则使用默认规则
                if (ruleId <= 0) {
                    ruleId = 1; // 使用默认规则ID
                }
                
                auto scoreRuleOpt = settings.rules.get(ruleId);
                if (scoreRuleOpt.has_value()) {
                    ScoreRule& rule = scoreRuleOpt.value().get();
                    
                    // 对于复合规则，需要获取适用于当前参赛人数的具体子规则
                    auto applicableRuleOpt = rule.getApplicableRule(event_ref.getParticipantCount());
                    
                    // 如果找不到适用规则或参赛人数不满足规则要求
                    if (!applicableRuleOpt.has_value() || 
                        !applicableRuleOpt.value().get().appliesTo(event_ref.getParticipantCount())) {
                        event_ref.setCancelled(true);
                        std::cout << "注意: 项目 \"" << event_ref.getName() << "\" (ID: " << event_ref.getId()
                              << ") 因参赛人数 (" << event_ref.getParticipantCount()
                              << ") 不满足计分规则要求，已被自动取消。" << std::endl;
                    }
                } else {
                    std::cerr << "警告: 找不到项目 \"" << event_ref.getName() << "\" 关联的计分规则 (ID: " 
                          << ruleId << ")，无法检查最低参赛人数要求。" << std::endl;
                }
            }
        }
    }
    
    return success;
}

int Registration::checkAndCancelEventsDueToLowParticipation() {
    std::cout << "\n正在检查并处理因人数不足而需取消的项目..." << std::endl;
    
    // 收集所有项目ID
    std::vector<int> eventIdsToCheck;
    for(const auto& pair : settings.events.getAllConst()){
        eventIdsToCheck.push_back(pair.first);
    }

    int eventToCancelCount = 0;
    for (const int eventId : eventIdsToCheck) {
        if (auto eventOpt = settings.events.get(eventId); eventOpt.has_value()) {
            CompetitionEvent& event_ref = eventOpt.value().get();
            
            // 只检查未被取消的项目
            if (event_ref.getIsCancelled()) {
                continue;
            }
            
            // 获取项目自身的计分规则ID
            int ruleId = event_ref.getScoreRuleId();
            
            // 如果项目未指定计分规则，使用默认规则（ID=1）
            if (ruleId <= 0) {
                ruleId = 1;
            }
            
            // 通过规则ID获取对应的计分规则对象
            auto ruleOpt = settings.rules.get(ruleId);
            if (!ruleOpt.has_value()) {
                std::cerr << "警告: 项目 \"" << event_ref.getName() << "\" (ID: " << event_ref.getId()
                          << ") 指定的计分规则ID " << ruleId << " 不存在。无法检查最低人数要求。" << std::endl;
                continue;
            }
            
            ScoreRule& rule = ruleOpt.value().get();
            int currentParticipants = event_ref.getParticipantCount();
            
            // 对于复合规则，需要找到适用于当前参赛人数的具体子规则
            auto applicableRuleOpt = rule.getApplicableRule(currentParticipants);
            
            // 如果未找到适用规则或当前人数不满足规则要求
            if (!applicableRuleOpt.has_value() || 
                !applicableRuleOpt.value().get().appliesTo(currentParticipants)) {
                
                // 将项目标记为已取消
                event_ref.setCancelled(true);
                eventToCancelCount++;
                
                std::cout << "已取消: 项目 \"" << event_ref.getName() << "\" (ID: " << event_ref.getId()
                          << ") 因参赛人数 (" << currentParticipants
                          << ") 不满足计分规则要求。" << std::endl;
            }
        }
    }
    
    if (eventToCancelCount > 0) {
        std::cout << "共有 " << eventToCancelCount << " 个项目因参赛人数不足被取消。" << std::endl;
    } else {
        std::cout << "检查完成，所有项目参赛人数均满足要求。" << std::endl;
    }
    
    // 验证运动员与赛事数据一致性
    settings.athletes.validateRegistrationConsistency();
    
    return eventToCancelCount;
}
