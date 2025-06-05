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
    std::cout << "\n正在检查并根据参赛人数不足取消相关项目..." << std::endl;
    
    // 收集所有项目ID
    std::vector<int> eventIdsToCheck;
    for(const auto& pair : settings.events.getAllConst()){
        if (!pair.second.get().getIsCancelled()) {  // 只收集未取消的项目
            eventIdsToCheck.push_back(pair.first);
        }
    }

    const int totalEvents = eventIdsToCheck.size();
    std::cout << "共需检查 " << totalEvents << " 个未取消的项目" << std::endl;

    if (totalEvents == 0) {
        std::cout << "没有需要检查的项目，跳过检测。" << std::endl;
        return 0;
    }

    int eventToCancelCount = 0;
    std::vector<std::string> canceledEventNames;
    
    for (const int eventId : eventIdsToCheck) {
        // 强有力的防止异常循环
        if (eventToCancelCount > totalEvents || eventToCancelCount > 100) {
            std::cerr << "警告: 检测到异常的取消项目数量(" << eventToCancelCount 
                      << ")，可能存在循环引用，中断处理" << std::endl;
            break;
        }

        auto eventOpt = settings.events.get(eventId);
        if (!eventOpt.has_value()) {
            std::cerr << "错误: 项目ID " << eventId << " 不存在，跳过处理" << std::endl;
            continue;
        }

        CompetitionEvent& event = eventOpt.value().get();
        
        // 获取项目配置的计分规则ID
        int ruleId = event.getScoreRuleId();
        
        // 如果项目未指定计分规则使用默认规则ID=1
        if (ruleId <= 0) {
            ruleId = 1;
        }
        
        std::cout << "检查项目: \"" << event.getName() << "\" (ID: " << event.getId()
                  << ") 当前参赛人数: " << event.getParticipantCount() 
                  << ", 使用规则ID: " << ruleId << std::endl;
        
        // 通过规则ID获取对应的计分规则对象
        auto ruleOpt = settings.rules.get(ruleId);
        if (!ruleOpt.has_value()) {
            std::cerr << "错误: 项目 \"" << event.getName() << "\" (ID: " << event.getId()
                      << ") 指定的计分规则ID " << ruleId << " 不存在。无法检查参赛人数要求。" << std::endl;
            continue;
        }
        
        ScoreRule& rule = ruleOpt.value().get();
        int currentParticipants = event.getParticipantCount();
        
        // 检查规则本身是否适用于当前参赛人数
        std::cout << "  直接检查规则适用性: ";
        bool basicApplies = rule.appliesTo(currentParticipants);
        std::cout << (basicApplies ? "适用" : "不适用") << std::endl;
        
        // 对于复合规则，要找到适用于当前参赛人数的具体子规则
        std::cout << "  尝试获取适用规则: ";
        auto applicableRuleOpt = rule.getApplicableRule(currentParticipants);

        bool shouldCancel = false;
        std::string cancelReason;

        // 如果未找到适用规则，参赛人数不满足要求
        if (!applicableRuleOpt.has_value()) {
            shouldCancel = true;
            cancelReason = "无适用的计分规则";
            std::cout << "失败 - " << cancelReason << std::endl;
        } else {
            std::cout << "成功 - 找到适用规则ID: " << applicableRuleOpt.value().get().getId() << std::endl;

            // 即使找到了规则，再次确认它确实适用
            bool actuallyApplies = applicableRuleOpt.value().get().appliesTo(currentParticipants);
            std::cout << "  二次确认适用性: " << (actuallyApplies ? "适用" : "不适用") << std::endl;

            if (!actuallyApplies) {
                shouldCancel = true;
                cancelReason = "计分规则不适用于当前参赛人数";
            }
        }

        if (shouldCancel) {
            // 将项目标记为已取消
            event.setCancelled(true);
            eventToCancelCount++;
            canceledEventNames.push_back(event.getName());

            std::cout << "已取消: 项目 \"" << event.getName() << "\" (ID: " << event.getId()
                      << ") 参赛人数 (" << currentParticipants
                      << ") 不满足计分规则要求，原因: " << cancelReason << std::endl;
        } else {
            std::cout << "项目 \"" << event.getName() << "\" 参赛人数满足要求" << std::endl;
        }
        
        std::cout << std::endl; // 添加空行分隔不同项目
    }
    
    if (eventToCancelCount > 0) {
        std::cout << "共有 " << eventToCancelCount << " 个项目因参赛人数不足被取消: ";
        for (const auto& name : canceledEventNames) {
            std::cout << "\"" << name << "\" ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "检查完成，所有项目参赛人数满足要求。" << std::endl;
    }
    
    // 验证运动员报名数据一致性
    settings.athletes.validateRegistrationConsistency();
    
    return eventToCancelCount;
}
