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
    if (settings_.getAllAthletes().empty()) {
        UIManager::showErrorMessage("系统中没有运动员，请先添加运动员。");
        return;
    }
    if (settings_.getAllCompetitionEvents().empty()) {
        UIManager::showErrorMessage("系统中没有比赛项目，请先添加项目。");
        return;
    }

    UIManager::showMessage("\n--- 运动员列表 ---");
    std::vector<std::reference_wrapper<const Athlete>> athletes_refs;
    for(const auto& pair : settings_.getAllAthletes()) athletes_refs.push_back(std::cref(pair.second));
    UIManager::displayAthletes(athletes_refs, settings_);
    int athleteId = UIManager::getIntInput("请输入要报名的运动员ID: ");

    UIManager::showMessage("\n--- 项目列表 ---");
    std::vector<std::reference_wrapper<const CompetitionEvent>> events_refs;
    for(const auto& pair : settings_.getAllCompetitionEvents()) {
        if (!pair.second.getIsCancelled()) { // 通常只报名未取消的项目
             events_refs.push_back(std::cref(pair.second));
        }
    }
    if(events_refs.empty()){
        UIManager::showMessage("当前没有可报名的项目 (可能所有项目都已取消或未添加)。");
        return;
    }
    UIManager::displayEvents(events_refs, settings_);
    int eventId = UIManager::getIntInput("请输入要报名的项目ID: ");

    // 调用核心注册逻辑 (假设 Registration 类中的方法会处理所有验证并返回状态)
    // 例如: RegistrationResult result = registration_.registerAthleteForEvent(athleteId, eventId);
    // 然后根据 result 显示不同消息。为简化，这里直接调用并假设它内部打印或我们基于返回值打印。
    // 在重构版本中，registration_.registerAthleteForEvent 应该返回一个明确的状态。
    // 暂时我们模仿原代码的行为，它直接在方法内打印消息。
    // 为了更好的分离，`registration_.registerAthleteForEvent` 应该返回一个枚举或布尔值，
    // 然后控制器根据这个返回值调用 UIManager 来显示消息。
    // 比如: if (registration_.registerAthleteForEvent(athleteId, eventId)) { UIManager::showSuccessMessage(...); } else { UIManager::showErrorMessage(...); }

    // 假设 Registration::registerAthleteForEvent 现在返回 bool
    if (registration_.registerAthleteForEvent(athleteId, eventId)) {
        // 成功消息通常由 registration_ 内部处理或由其返回的状态决定
        // UIManager::showSuccessMessage("报名成功。"); // 或者更详细的信息
    } else {
        // 失败消息同上
        // UIManager::showErrorMessage("报名失败，详情请查看 Registration 模块的提示。");
    }
}

void RegistrationController::handleUnregisterAthleteFromEvent() {
    if (settings_.getAllAthletes().empty()) {
        UIManager::showErrorMessage("系统中没有运动员。");
        return;
    }
     UIManager::showMessage("\n--- 运动员列表 ---");
    std::vector<std::reference_wrapper<const Athlete>> athletes_refs;
    for(const auto& pair : settings_.getAllAthletes()) athletes_refs.push_back(std::cref(pair.second));
    UIManager::displayAthletes(athletes_refs, settings_);
    int athleteId = UIManager::getIntInput("请输入要取消报名的运动员ID: ");

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
    std::vector<std::reference_wrapper<const CompetitionEvent>> registered_events_refs;
    bool hasEvents = false;
    for (int eventId : athlete.getRegisteredEventIds()) {
        auto eventOpt = settings_.getCompetitionEventConst(eventId);
        if (eventOpt) {
            registered_events_refs.push_back(eventOpt.value());
            hasEvents = true;
        }
    }
    if (!hasEvents) {
         UIManager::showMessage("未能获取该运动员已报名项目的详细信息，或其报名的项目已被删除。");
         // 或者列出所有项目作为备选
         UIManager::showMessage("\n--- 所有项目列表 (供参考) ---");
         std::vector<std::reference_wrapper<const CompetitionEvent>> all_events_refs;
         for(const auto& pair : settings_.getAllCompetitionEvents()) all_events_refs.push_back(std::cref(pair.second));
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
    if (settings_.getAllCompetitionEvents().empty()) {
        UIManager::showMessage("暂无比赛项目。");
        return;
    }
    std::vector<std::reference_wrapper<const CompetitionEvent>> events_refs;
    for(const auto& val : settings_.getAllCompetitionEvents() | std::views::values) events_refs.push_back(std::cref(val));
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
