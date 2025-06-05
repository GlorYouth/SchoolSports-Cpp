//
// Created by GlorYouth on 2025/6/2.
//

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

bool Registration::registerAthleteForEvent(const int athleteId, const int eventId) {
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

    if (event_ref.getIsCancelled()){
        std::cerr << "报名失败: 项目 \"" << event_ref.getName() << "\" 已取消。" << std::endl;
        return false;
    }

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

    // 1. 检查运动员性别是否符合项目要求
    if (!event_ref.canAthleteRegister(athlete_ref.getGender())) {
        std::cerr << "报名失败: 运动员 " << athlete_ref.getName()
                  << " 性别(" << genderToString(athlete_ref.getGender())
                  << ") 不符合项目 \"" << event_ref.getName()
                  << "\" 性别要求(" << genderToString(event_ref.getGenderRequirement()) << ")." << std::endl;
        return false;
    }

    // 2. 尝试在运动员处注册项目 (Athlete::registerForEvent 会检查数量和重复)
    if (!athlete_ref.registerForEvent(eventId, settings.athletes.getMaxEventsAllowed())) {
        // Athlete::registerForEvent 内部已经处理了重复报名和超限额的情况
        // 此处可以根据具体失败原因给出更明确的提示，但 Athlete 类目前不返回具体原因
        if (athlete_ref.isRegisteredForEvent(eventId)) {
             std::cerr << "报名失败: 运动员 " << athlete_ref.getName() << " 已报名项目 \"" << event_ref.getName() << "\"。" << std::endl;
        } else if (athlete_ref.getRegisteredEventsCount() >= settings.athletes.getMaxEventsAllowed()) {
             std::cerr << "报名失败: 运动员 " << athlete_ref.getName() << " 已达到最大报名项目数 ("
                      << settings.athletes.getMaxEventsAllowed() << ")." << std::endl;
        } else {
            std::cerr << "报名失败: 运动员 " << athlete_ref.getName() << " 无法报名项目 \"" << event_ref.getName() << "\" (未知原因)。" << std::endl;
        }
        return false;
    }

    // 3. 尝试将运动员添加到项目参与者列表
    if (!event_ref.addParticipant(athleteId)) {
        // 如果运动员已在项目中（理论上不应发生，因为上面 athlete_ref.registerForEvent 成功了）
        // 需要回滚运动员的报名操作
        athlete_ref.unregisterFromEvent(eventId); // 回滚
        std::cerr << "报名失败: 运动员 " << athlete_ref.getName() << " 已在项目 \"" << event_ref.getName() << "\" 的参与者列表中 (理论上不应发生，已回滚运动员报名)。" << std::endl;
        return false;
    }

    std::cout << "报名成功: 运动员 " << athlete_ref.getName() << " 已报名参加项目 \"" << event_ref.getName() << "\"" << std::endl;
    return true;
}

bool Registration::unregisterAthleteFromEvent(const int athleteId, const int eventId) {
    const auto athleteOpt = settings.athletes.get(athleteId);
    const auto eventOpt = settings.events.get(eventId);

    if (!athleteOpt.has_value()) {
        std::cerr << "取消报名失败: 运动员ID " << athleteId << " 不存在。" << std::endl;
        return false;
    }   
    Athlete& athlete_ref = athleteOpt.value().get(); // 使用引用

    if (!eventOpt.has_value()) {
        std::cerr << "取消报名失败: 项目ID " << eventId << " 不存在。" << std::endl;
        return false;
    }
    CompetitionEvent& event_ref = eventOpt.value().get(); // 使用引用

    // 尝试从运动员的报名列表中移除
    if (!athlete_ref.unregisterFromEvent(eventId)) {
        std::cerr << "取消报名失败: 运动员 " << athlete_ref.getName() << " 未报名项目 \"" << event_ref.getName() << "\" (或无法从运动员记录中移除)。" << std::endl;
        return false;
    }

    // 尝试从项目的参与者列表中移除
    if (!event_ref.removeParticipant(athleteId)) {
        // 如果运动员不在项目参与者中（理论上不应发生，因为上面 athlete_ref.unregisterFromEvent 成功了）
        // 可能需要考虑是否要回滚运动员的取消报名操作（但这会使逻辑更复杂）
        // 这里假设如果运动员记录中取消成功，项目记录中也应该能对应上
        std::cerr << "警告: 运动员 " << athlete_ref.getName() << " 已从个人报名列表取消项目 \"" << event_ref.getName() << "\", 但未在项目参与者列表中找到 (数据可能不一致)。" << std::endl;
        // 即使这里失败，也认为运动员的取消操作是主要的，所以继续
    }

    std::cout << "取消报名成功: 运动员 " << athlete_ref.getName() << " 已取消报名项目 \"" << event_ref.getName() << "\"" << std::endl;

    // 取消报名后，检查项目是否会因此人数不足而需要取消
    // 使用项目自身的计分规则来判断最少人数
    if (!event_ref.getIsCancelled()) {
        // 获取项目关联的计分规则ID
        int ruleId = event_ref.getScoreRuleId();
        
        // 如果项目没有关联计分规则，则使用默认规则（ID=1）
        if (ruleId <= 0) {
            ruleId = 1;
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
    return true;
}

int Registration::checkAndCancelEventsDueToLowParticipation() {
    std::cout << "\n正在检查并处理因人数不足而需取消的项目..." << std::endl;
    bool changed = false;
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
            
            // 尝试获取对应的计分规则
            auto scoreRuleOpt = settings.rules.get(ruleId);
            if (!scoreRuleOpt.has_value()) {
                std::cerr << "警告: 项目 \"" << event_ref.getName() << "\" (ID: " << eventId 
                          << ") 关联的计分规则 (ID: " << ruleId << ") 不存在，无法检查最低参赛人数要求。" << std::endl;
                continue;
            }
            
            // 获取适用于当前参赛人数的具体规则（可能是子规则）
            ScoreRule& rule = scoreRuleOpt.value().get();
            auto applicableRuleOpt = rule.getApplicableRule(event_ref.getParticipantCount());
            
            // 如果找不到适用规则或参赛人数不满足规则要求
            if (!applicableRuleOpt.has_value() || 
                !applicableRuleOpt.value().get().appliesTo(event_ref.getParticipantCount())) {
                event_ref.setCancelled(true);
                std::cout << "项目 \"" << event_ref.getName() << "\" (ID: " << event_ref.getId()
                          << ") 因参赛人数 (" << event_ref.getParticipantCount()
                          << ") 不满足计分规则要求，已被取消。" << std::endl;
                changed = true;
                eventToCancelCount++;
            }
        }
    }
    
    if (!changed) {
        std::cout << "没有项目因人数不足而需要取消。" << std::endl;
    }
    return eventToCancelCount;
}
