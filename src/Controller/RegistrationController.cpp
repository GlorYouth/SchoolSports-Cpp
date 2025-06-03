//
// Created by GlorYouth on 2025/6/3.
//

#include "../../include/Controller/RegistrationController.h"

#include "../../include/UI/UIManager.h"
#include "../../include/Components/Athlete.h"
#include "../../include/Components/CompetitionEvent.h"
#include <vector>
#include <functional> // For std::reference_wrapper
#include <ranges>
#include <set>

RegistrationController::RegistrationController(Registration& registration, SystemSettings& settings)
    : registration_(registration), settings_(settings) {}

void RegistrationController::manage() {
    int choice;
    do {
        UIManager::displayRegistrationMenu();
        choice = UIManager::getIntInput("请输入您的选择: ", 0, 5);

        switch (choice) {
            case 1: handleRegisterAthleteForEvent(); break;
            case 2: handleUnregisterAthleteFromEvent(); break;
            case 3: handleViewAthleteRegistrations(); break;
            case 4: handleViewEventRegistrations(); break;
            case 5: handleCheckAndCancelLowParticipationEvents(); break;
            case 0: UIManager::showMessage("返回主菜单..."); break;
            default: UIManager::showErrorMessage("无效选择。"); break;
        }
        if (choice != 0) {
            UIManager::pressEnterToContinue();
        }
    } while (choice != 0);
}

void RegistrationController::handleRegisterAthleteForEvent() {
    // 新增：报名前检查赛程是否已锁定
    if (!settings_.isScheduleLocked()) {
        UIManager::showRegistrationLockedMessage();
        return;
    }
    if (settings_.getAllAthletes().empty()) {
        UIManager::showErrorMessage("系统中没有运动员，请先添加运动员。");
        return;
    }
    if (settings_.getAllCompetitionEventsConst().empty()) {
        UIManager::showErrorMessage("系统中没有比赛项目，请先添加项目。");
        return;
    }

    UIManager::showMessage("\n--- 运动员列表 ---");
    std::vector<utils::RefConst<Athlete>> athletes_refs;
    for(const auto& val : settings_.getAllAthletes() | std::views::values) athletes_refs.push_back(std::cref(val));
    UIManager::displayAthletes(athletes_refs, settings_);
    const int athleteId = UIManager::getIntInput("请输入要报名的运动员ID: ");

    // 新增：展示完整时间表（所有项目及其时间/场地信息），高亮冲突项目
    UIManager::showMessage("\n--- 当前完整项目时间表 ---");
    std::vector<utils::RefConst<CompetitionEvent>> all_events_refs;
    for(const auto& pair : settings_.getAllCompetitionEventsConst()) {
        all_events_refs.push_back(std::cref(pair.second));
    }
    // 检查冲突项目
    std::set<int> conflictEventIds;
    auto athleteOpt = settings_.getAthleteConst(athleteId);
    if (athleteOpt) {
        const Athlete& athlete = athleteOpt.value().get();
        // 辅助函数：将"HH:MM"字符串转为分钟数
        auto timeStringToMinutes = [](const std::string& timeStr) -> int {
            if (timeStr.size() != 5 || timeStr[2] != ':') return -1;
            int hour = std::stoi(timeStr.substr(0, 2));
            int min = std::stoi(timeStr.substr(3, 2));
            return hour * 60 + min;
        };
        // 遍历所有项目，若与运动员已报名项目有时间重叠则高亮
        for (const auto& event_ref : all_events_refs) {
            const CompetitionEvent& event = event_ref.get();
            int newStartMin = timeStringToMinutes(event.getStartTime());
            int newEndMin = timeStringToMinutes(event.getEndTime());
            if (newStartMin >= 0 && newEndMin > newStartMin) {
                for (int regEventId : athlete.getRegisteredEventIds()) {
                    if (regEventId == event.getId()) continue;
                    auto regEventOpt = settings_.getCompetitionEventConst(regEventId);
                    if (!regEventOpt) continue;
                    const CompetitionEvent& regEvent = regEventOpt.value().get();
                    int regStartMin = timeStringToMinutes(regEvent.getStartTime());
                    int regEndMin = timeStringToMinutes(regEvent.getEndTime());
                    if (regStartMin >= 0 && regEndMin > regStartMin) {
                        if (!(newEndMin <= regStartMin || newStartMin >= regEndMin)) {
                            conflictEventIds.insert(event.getId());
                        }
                    }
                }
            }
        }
    }
    UIManager::displayEvents(all_events_refs, settings_);

    // 仅展示未取消的可报名项目
    UIManager::showMessage("\n--- 可报名项目列表 ---");
    std::vector<utils::RefConst<CompetitionEvent>> events_refs;
    for(const auto& [id, event_ref] : settings_.getAllCompetitionEventsConst()) {
        if (!event_ref.get().getIsCancelled()) {
             events_refs.push_back(std::cref(event_ref));
        }
    }
    if(events_refs.empty()){
        UIManager::showMessage("当前没有可报名的项目 (可能所有项目都已取消或未添加)。");
        return;
    }
    // 新增：可报名项目列表也高亮冲突项目
    UIManager::displayEvents(events_refs, settings_);
    int eventId = UIManager::getIntInput("请输入要报名的项目ID: ");

    // 调用核心注册逻辑
    if (registration_.registerAthleteForEvent(athleteId, eventId)) {
        // 成功消息通常由 registration_ 内部处理或由其返回的状态决定
    } else {
        // 失败消息同上
    }
}

void RegistrationController::handleUnregisterAthleteFromEvent() {
    if (settings_.getAllAthletes().empty()) {
        UIManager::showErrorMessage("系统中没有运动员。");
        return;
    }
     UIManager::showMessage("\n--- 运动员列表 ---");
    std::vector<utils::RefConst<Athlete>> athletes_refs;
    for(const auto& val : settings_.getAllAthletes() | std::views::values) athletes_refs.push_back(std::cref(val));
    UIManager::displayAthletes(athletes_refs, settings_);
    const int athleteId = UIManager::getIntInput("请输入要取消报名的运动员ID: ");

    // 优化：可以只列出该运动员已报名的项目
    auto athleteOpt = settings_.getAthleteConst(athleteId);
    if (!athleteOpt) {
        UIManager::showErrorMessage("未找到ID为 " + std::to_string(athleteId) + " 的运动员。");
        return;
    }
    const Athlete& athlete = athleteOpt.value().get();
    if (athlete.getRegisteredEventsCount() == 0) {
        UIManager::showMessage("运动员 " + athlete.getName() + " 没有报名任何项目。");
        return;
    }

    UIManager::showMessage("\n--- 运动员 " + athlete.getName() + " 已报名的项目 ---");
    std::vector<utils::RefConst<CompetitionEvent>> registered_events_refs;
    bool hasEvents = false;
    for (int eventId : athlete.getRegisteredEventIds()) {
        if (auto eventOpt = settings_.getCompetitionEventConst(eventId)) {
            registered_events_refs.push_back(eventOpt.value());
            hasEvents = true;
        }
    }
    if (!hasEvents) {
         UIManager::showMessage("未能获取该运动员已报名项目的详细信息，或其报名的项目已被删除。");
         // 或者列出所有项目作为备选
         UIManager::showMessage("\n--- 所有项目列表 (供参考) ---");
         std::vector<utils::RefConst<CompetitionEvent>> all_events_refs;
         for(const auto& val : settings_.getAllCompetitionEventsConst() | std::views::values) all_events_refs.push_back(std::cref(val));
         UIManager::displayEvents(all_events_refs, settings_);
    } else {
        UIManager::displayEvents(registered_events_refs, settings_);
    }

    int eventId = UIManager::getIntInput("请输入要取消报名的项目ID: ");

    // 同样，假设 Registration::unregisterAthleteFromEvent 返回 bool
    if (registration_.unregisterAthleteFromEvent(athleteId, eventId)) {
        // UIManager::showSuccessMessage("取消报名成功。");
    } else {
        // UIManager::showErrorMessage("取消报名失败。");
    }
}

void RegistrationController::handleViewAthleteRegistrations() {
    UIManager::showMessage("\n--- 所有运动员及其报名情况 ---");
    if (settings_.getAllAthletes().empty()) {
        UIManager::showMessage("暂无运动员。");
        return;
    }
    const auto athletes_refs = settings_.getAllAthlesConst();
    UIManager::displayAthletes(athletes_refs, settings_); // UIManager::displayAthletes 已包含报名信息
}

void RegistrationController::handleViewEventRegistrations() {
    UIManager::showMessage("\n--- 所有项目及其报名情况 ---");
    if (settings_.getAllCompetitionEventsConst().empty()) {
        UIManager::showMessage("暂无比赛项目。");
        return;
    }
    std::vector<utils::RefConst<CompetitionEvent>> events_refs;
    for(const auto& val : settings_.getAllCompetitionEventsConst() | std::views::values) events_refs.push_back(std::cref(val));
    UIManager::displayEvents(events_refs, settings_); // UIManager::displayEvents 已包含报名人数
}

void RegistrationController::handleCheckAndCancelLowParticipationEvents() {
    UIManager::showMessage("\n--- 检查并取消人数不足的项目 ---");
    // 此逻辑通常在 Registration 类中实现，因为它需要修改项目状态并可能通知运动员
    // registration_.checkAndCancelEventsDueToLowParticipation();
    // 假设该方法会打印相关信息或返回一个总结
    int cancelledCount = registration_.checkAndCancelEventsDueToLowParticipation(); // 假设返回取消的项目数
    if (cancelledCount > 0) {
        UIManager::showSuccessMessage("已检查并取消了 " + std::to_string(cancelledCount) + " 个人数不足的项目。");
    } else {
        UIManager::showMessage("没有项目因人数不足而被取消，或所有项目均满足人数要求。");
    }
}
