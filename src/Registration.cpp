//
// Created by GlorYouth on 2025/6/2.
//

#include "../include/Registration.h"
#include "../include/SystemSettings.h"
#include "../include/Athlete.h"
#include "../include/CompetitionEvent.h"
#include <iostream> // 用于输出信息
#include <ranges>

Registration::Registration(SystemSettings& sysSettings) : settings(sysSettings) {}

bool Registration::registerAthleteForEvent(const int athleteId, const int eventId) const { // 移除了 const
    auto athleteOpt = settings.getAthlete(athleteId);
    auto eventOpt = settings.getCompetitionEvent(eventId);

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

    // 1. 检查运动员性别是否符合项目要求
    if (!event_ref.canAthleteRegister(athlete_ref.getGender())) {
        std::cerr << "报名失败: 运动员 " << athlete_ref.getName()
                  << " 性别(" << genderToString(athlete_ref.getGender())
                  << ") 不符合项目 \"" << event_ref.getName()
                  << "\" 性别要求(" << genderToString(event_ref.getGenderRequirement()) << ")." << std::endl;
        return false;
    }

    // 2. 尝试在运动员处注册项目 (Athlete::registerForEvent 会检查数量和重复)
    if (!athlete_ref.registerForEvent(eventId, settings.getAthleteMaxEventsAllowed())) {
        // Athlete::registerForEvent 内部已经处理了重复报名和超限额的情况
        // 此处可以根据具体失败原因给出更明确的提示，但 Athlete 类目前不返回具体原因
        if (athlete_ref.isRegisteredForEvent(eventId)) {
             std::cerr << "报名失败: 运动员 " << athlete_ref.getName() << " 已报名项目 \"" << event_ref.getName() << "\"。" << std::endl;
        } else if (athlete_ref.getRegisteredEventsCount() >= settings.getAthleteMaxEventsAllowed()) {
             std::cerr << "报名失败: 运动员 " << athlete_ref.getName() << " 已达到最大报名项目数 ("
                      << settings.getAthleteMaxEventsAllowed() << ")." << std::endl;
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

bool Registration::unregisterAthleteFromEvent(const int athleteId, const int eventId) const { // 移除了 const
    auto athleteOpt = settings.getAthlete(athleteId);
    auto eventOpt = settings.getCompetitionEvent(eventId);

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
    // checkAndCancelEventsDueToLowParticipation(); // 可以检查所有项目
    // 或者仅针对此项目检查，效率更高
    if (!event_ref.getIsCancelled() && event_ref.getParticipantCount() < settings.getMinParticipantsToHoldEvent()) {
        event_ref.setCancelled(true);
        std::cout << "注意: 项目 \"" << event_ref.getName() << "\" (ID: " << event_ref.getId()
                  << ") 因参赛人数 (" << event_ref.getParticipantCount()
                  << ") 少于最少要求 (" << settings.getMinParticipantsToHoldEvent()
                  << ") 已被自动取消。" << std::endl;
    }
    return true;
}

void Registration::checkAndCancelEventsDueToLowParticipation() const { // 移除了 const
    std::cout << "\n正在检查并处理因人数不足而需取消的项目..." << std::endl;
    bool changed = false;
    // 需要迭代 map 的 value，并且可能修改它，所以不能用 range-based for loop 直接在 const map 上
    // 或者，如果 SystemSettings::getAllCompetitionEvents 返回的是非 const 引用，则可以
    // 当前 SystemSettings::getAllCompetitionEvents() 返回 const&
    // 所以我们需要通过ID获取非const的event对象来修改
    std::vector<int> eventIdsToCheck;
    for(const auto& pair : settings.getAllCompetitionEvents()){
        eventIdsToCheck.push_back(pair.first);
    }

    for (const int eventId : eventIdsToCheck) {
        if (auto eventOpt = settings.getCompetitionEvent(eventId); eventOpt.has_value()) {
            if (CompetitionEvent& event_ref = eventOpt.value().get(); !event_ref.getIsCancelled()) { // 只检查未被取消的项目
                if (event_ref.getParticipantCount() < settings.getMinParticipantsToHoldEvent()) {
                    event_ref.setCancelled(true); // 修改获取到的非 const 对象
                    std::cout << "项目 \"" << event_ref.getName() << "\" (ID: " << event_ref.getId()
                              << ") 因参赛人数 (" << event_ref.getParticipantCount()
                              << ") 少于最少要求 (" << settings.getMinParticipantsToHoldEvent()
                              << ") 已被取消。" << std::endl;
                    changed = true;
                }
            }
        }
    }
    if (!changed) {
        std::cout << "没有项目因人数不足而需要取消。" << std::endl;
    }
}
