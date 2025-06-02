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

bool Registration::registerAthleteForEvent(int athleteId, int eventId) const {
    auto athlete = settings.getAthlete(athleteId);
    auto event = settings.getCompetitionEvent(eventId);

    if (!athlete.has_value()) {
        std::cerr << "报名失败: 运动员ID " << athleteId << " 不存在。" << std::endl;
        return false;
    }
    auto athlete_ref = athlete.value().get();
    if (!event) {
        std::cerr << "报名失败: 项目ID " << eventId << " 不存在。" << std::endl;
        return false;
    }
    auto event_ref = event.value().get();

    if (event_ref.getIsCancelled()){
        std::cerr << "报名失败: 项目 " << event_ref.getName() << " 已取消。" << std::endl;
        return false;
    }

    // 1. 检查运动员性别是否符合项目要求
    if (!event_ref.canAthleteRegister(athlete_ref.getGender())) {
        std::cerr << "报名失败: 运动员 " << athlete_ref.getName()
                  << " 性别(" << genderToString(athlete_ref.getGender())
                  << ") 不符合项目 " << event_ref.getName()
                  << " 性别要求(" << genderToString(event_ref.getGenderRequirement()) << ")." << std::endl;
        return false;
    }

    // 2. 检查运动员报名项目数量是否已达上限
    if (athlete_ref.getRegisteredEventsCount() >= settings.getAthleteMaxEventsAllowed()) {
        std::cerr << "报名失败: 运动员 " << athlete_ref.getName() << " 已达到最大报名项目数 ("
                  << settings.getAthleteMaxEventsAllowed() << ")." << std::endl;
        return false;
    }

    // 3. 检查运动员是否已报名该项目
    if (athlete_ref.isRegisteredForEvent(eventId)) {
        std::cerr << "报名失败: 运动员 " << athlete_ref.getName() << " 已报名项目 " << event_ref.getName() << "。" << std::endl;
        return false; // 或者可以认为这是成功的，取决于业务逻辑
    }

    // 执行报名
    if (athlete_ref.registerForEvent(eventId, settings.getAthleteMaxEventsAllowed())) {
        event_ref.addParticipant(athleteId);
        std::cout << "报名成功: 运动员 " << athlete_ref.getName() << " 已报名参加项目 " << event_ref.getName() << std::endl;
        return true;
    }
    // 此处理论上不应到达，因为前面已经检查过数量限制
    std::cerr << "报名失败: 运动员 " << athlete_ref.getName() << " 报名项目 " << event_ref.getName() << " 时发生未知错误。" << std::endl;
    return false;
}

bool Registration::unregisterAthleteFromEvent(int athleteId, int eventId) const {
    auto athlete = settings.getAthlete(athleteId);
    auto event = settings.getCompetitionEvent(eventId);

    if (!athlete.has_value()) {
        std::cerr << "取消报名失败: 运动员ID " << athleteId << " 不存在。" << std::endl;
        return false;
    }
    auto athlete_ref = athlete.value().get();
    if (!event) {
        std::cerr << "取消报名失败: 项目ID " << eventId << " 不存在。" << std::endl;
        return false;
    }
    auto event_ref = event.value().get();

    if (!athlete_ref.isRegisteredForEvent(eventId)) {
        std::cerr << "取消报名失败: 运动员 " << athlete_ref.getName() << " 未报名项目 " << event_ref.getName() << "。" << std::endl;
        return false;
    }

    athlete_ref.unregisterFromEvent(eventId);
    event_ref.removeParticipant(athleteId);
    std::cout << "取消报名成功: 运动员 " << athlete_ref.getName() << " 已取消报名项目 " << event_ref.getName() << std::endl;

    // 取消报名后，检查项目是否会因此人数不足而需要取消
    checkAndCancelEventsDueToLowParticipation(); // 或者仅针对此项目检查
    if (event_ref.getParticipantCount() < settings.getMinParticipantsToHoldEvent() && !event_ref.getIsCancelled()) {
        event_ref.setCancelled(true);
        std::cout << "注意: 项目 " << event_ref.getName() << " 因人数不足 ("
                  << event_ref.getParticipantCount() << ") 已被自动取消。" << std::endl;
    }

    return true;
}

void Registration::checkAndCancelEventsDueToLowParticipation() const {
    std::cout << "\n正在检查并处理因人数不足而需取消的项目..." << std::endl;
    bool changed = false;
    for (const auto &key: settings.getAllCompetitionEvents() | std::views::keys) {
        if (auto event = settings.getCompetitionEvent(key); event.has_value() && !event.value().get().getIsCancelled()) {
            auto event_ref = event.value().get();
            if (event_ref.getParticipantCount() < settings.getMinParticipantsToHoldEvent()) {
                event_ref.setCancelled(true);
                std::cout << "项目 \"" << event_ref.getName() << "\" (ID: " << event_ref.getId()
                          << ") 因参赛人数 (" << event_ref.getParticipantCount()
                          << ") 少于最少要求 (" << settings.getMinParticipantsToHoldEvent()
                          << ") 已被取消。" << std::endl;
                changed = true;
            }
        }
    }
    if (!changed) {
        std::cout << "没有项目因人数不足而需要取消。" << std::endl;
    }
}
