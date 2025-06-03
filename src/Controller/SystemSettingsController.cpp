//
// Created by GlorYouth on 2025/6/3.
//

#include "../../include/Controller/SystemSettingsController.h"
#include "../../include/UI/UIManager.h"
#include "../../include/Components/Unit.h"
#include "../../include/Components/Athlete.h"
#include "../../include/Components/CompetitionEvent.h"
#include "../../include/Components/ScoreRule.h"
#include "../../include/Components/Schedule.h"
#include <algorithm> // for std::sort, std::find (如果需要)
#include <ranges>    // for std::views::values (C++20)
#include <limits>

SystemSettingsController::SystemSettingsController(SystemSettings& settings)
    : settings_(settings) {}

void SystemSettingsController::manage() {
    int choice;
    do {
        UIManager::displaySystemSettingsMenu(settings_);
        choice = UIManager::getIntInput("请输入您的选择: ", 0, 13);

        switch (choice) {
            case 1: handleAddUnit(); break;
            case 2: handleViewAllUnits(); break;
            case 3: handleAddEvent(); break;
            case 4: handleViewAllEvents(); break;
            case 5: handleAddAthlete(); break;
            case 6: handleViewAllAthletes(); break;
            case 7: handleAddScoreRule(); break;
            case 8: handleViewAllScoreRules(); break;
            case 9: handleSetAthleteMaxEvents(); break;
            case 10: handleSetMinParticipantsToHoldEvent(); break;
            case 11:
                if (!settings_.isScheduleLocked()) {
                    handleVenueManagement();
                } else {
                    UIManager::showErrorMessage("赛程已锁定，无法维护场地表。");
                }
                break;
            case 12:
                handleSessionSettings();
                break;
            case 13:
                handleScheduleGeneration();
                break;
            case 0: UIManager::showMessage("返回主菜单..."); break;
            default: UIManager::showErrorMessage("无效选择，请重新输入。"); break; // 理论上 getIntInput 已经处理了范围
        }
        if (choice != 0) {
            UIManager::pressEnterToContinue();
        }
    } while (choice != 0);
}

void SystemSettingsController::handleAddUnit() const{
    const std::string name = UIManager::getStringInput("请输入单位名称: ");
    if (settings_.addUnit(name)) { // 假设 addUnit 返回 bool 并且不再打印
        UIManager::showSuccessMessage("单位 '" + name + "' 添加成功。");
    } else {
        UIManager::showErrorMessage("单位添加失败 (可能名称已存在或输入无效)。");
    }
}

void SystemSettingsController::handleViewAllUnits() {
    // 从 settings_ 获取数据
    // C++20 ranges approach:
    std::vector<utils::RefConst<Unit>> units;
    for (const auto& val : settings_.getAllUnits() | std::views::values) { // getAllUnits() 返回 const auto&
        units.push_back(std::cref(val));
    }
    UIManager::displayUnits(units);
}

void SystemSettingsController::handleAddEvent() {
    std::string name = UIManager::getStringInput("请输入项目名称: ");
    EventType eventType = UIManager::getEventTypeInput("请选择项目类型");
    Gender gender = UIManager::getGenderInput("请选择性别要求", true); // true 表示允许不限性别

    if (settings_.addCompetitionEvent(name, eventType, gender)) {
        UIManager::showSuccessMessage("项目 '" + name + "' 添加成功。");
    } else {
        UIManager::showErrorMessage("项目添加失败。");
    }
}

void SystemSettingsController::handleViewAllEvents() {
    std::vector<utils::RefConst<CompetitionEvent>> events;
     for (const auto& val : settings_.getAllCompetitionEventsConst() | std::views::values) {
        events.push_back(std::cref(val));
    }
    UIManager::displayEvents(events, settings_); // 可能需要 settings 来获取计分规则描述
}

void SystemSettingsController::handleAddAthlete() {
    if (settings_.getAllUnits().empty()) {
        UIManager::showErrorMessage("请先添加参赛单位。");
        return;
    }
    handleViewAllUnits(); // 显示单位列表供参考

    std::string name = UIManager::getStringInput("请输入运动员姓名: ");
    Gender gender = UIManager::getGenderInput("请选择运动员性别", false); // false 表示不允许不限性别
    if (gender == Gender::UNSPECIFIED) { // 双重检查，尽管 getGenderInput(..., false) 应该阻止
        UIManager::showErrorMessage("运动员性别不能为空。");
        return;
    }
    int unitId = UIManager::getIntInput("请输入运动员所属单位ID: ");

    if (settings_.addAthlete(name, gender, unitId)) {
        UIManager::showSuccessMessage("运动员 '" + name + "' 添加成功。");
    } else {
        UIManager::showErrorMessage("运动员添加失败 (单位ID可能无效或输入错误)。");
    }
}

void SystemSettingsController::handleViewAllAthletes() {
    std::vector<utils::RefConst<Athlete>> athletes;
    for (const auto& val : settings_.getAllAthletes() | std::views::values) {
        athletes.push_back(std::cref(val));
    }
    UIManager::displayAthletes(athletes, settings_);
}

void SystemSettingsController::handleAddScoreRule() {
    std::string desc = UIManager::getStringInput("请输入规则描述 (例如: 超过6人取前5名): ");
    int minP = UIManager::getIntInput("请输入适用此规则的最小参赛人数: ");
    int maxP = UIManager::getIntInput("请输入适用此规则的最大参赛人数 (-1表示无上限): ");
    int ranks = UIManager::getIntInput("请输入录取名次数: ", 1, 100); // 假设名次数至少为1，上限100

    std::map<int, double> scoresMap;
    for (int i = 1; i <= ranks; ++i) {
        double scoreVal = UIManager::getIntInput("请输入第 " + std::to_string(i) + " 名的分数: "); // 假设分数也是整数或可转为double
        scoresMap[i] = scoreVal;
    }

    if (settings_.addScoreRule(desc, minP, maxP, ranks, scoresMap)) {
        UIManager::showSuccessMessage("计分规则添加成功。");
    } else {
        UIManager::showErrorMessage("计分规则添加失败。");
    }
}

void SystemSettingsController::handleViewAllScoreRules() {
     std::vector<utils::RefConst<ScoreRule>> rules;
     for (const auto& val : settings_.getAllScoreRules() | std::views::values) {
        rules.push_back(std::cref(val));
    }
    UIManager::displayScoreRules(rules);
}

void SystemSettingsController::handleSetAthleteMaxEvents() {
    int maxEvents = UIManager::getIntInput("请输入新的运动员最大参赛项目数: ", 1, 10); // 假设范围1-10
    settings_.setAthleteMaxEventsAllowed(maxEvents); // 假设此方法存在且不直接打印
    UIManager::showSuccessMessage("运动员最大参赛项目数已更新为: " + std::to_string(maxEvents));
}

void SystemSettingsController::handleSetMinParticipantsToHoldEvent() {
    int minParticipants = UIManager::getIntInput("请输入新的项目最少举行人数: ", 1, 100); // 假设范围1-100
    settings_.setMinParticipantsToHoldEvent(minParticipants); // 假设此方法存在且不直接打印
    UIManager::showSuccessMessage("项目最少举行人数已更新为: " + std::to_string(minParticipants));
}

void SystemSettingsController::handleVenueManagement() {
    int choice;
    do {
        UIManager::displayVenueManagementMenu();
        choice = UIManager::getIntInput("请输入您的选择: ", 0, 3);
        switch (choice) {
            case 1: {
                std::string venueName = UIManager::getStringInput("请输入新场地名称: ");
                if (settings_.addVenue(venueName)) {
                    UIManager::showSuccessMessage("场地添加成功。");
                } else {
                    UIManager::showErrorMessage("场地已存在，添加失败。");
                }
                break;
            }
            case 2: {
                std::string venueName = UIManager::getStringInput("请输入要删除的场地名称: ");
                if (settings_.removeVenue(venueName)) {
                    UIManager::showSuccessMessage("场地删除成功。");
                } else {
                    UIManager::showErrorMessage("未找到该场地，删除失败。");
                }
                break;
            }
            case 3: {
                UIManager::displayVenues(settings_.getAllVenues());
                break;
            }
            case 0:
                UIManager::showMessage("返回上一级菜单...");
                break;
            default:
                UIManager::showErrorMessage("无效选择。");
                break;
        }
        if (choice != 0) {
            UIManager::pressEnterToContinue();
        }
    } while (choice != 0);
}

void SystemSettingsController::handleSessionSettings() {
    int choice;
    do {
        UIManager::displaySessionSettingsMenu(settings_);
        choice = UIManager::getIntInput("请输入您的选择: ", 0, 2);
        switch (choice) {
            case 1: {
                std::string start = UIManager::getStringInput("请输入上午开始时间(格式08:00): ");
                std::string end = UIManager::getStringInput("请输入上午结束时间(格式12:00): ");
                settings_.setMorningSession(start, end);
                UIManager::showSuccessMessage("上午时间段设置成功。");
                break;
            }
            case 2: {
                std::string start = UIManager::getStringInput("请输入下午开始时间(格式14:00): ");
                std::string end = UIManager::getStringInput("请输入下午结束时间(格式18:00): ");
                settings_.setAfternoonSession(start, end);
                UIManager::showSuccessMessage("下午时间段设置成功。");
                break;
            }
            case 0:
                UIManager::showMessage("返回上一级菜单...");
                break;
            default:
                UIManager::showErrorMessage("无效选择。");
                break;
        }
        if (choice != 0) {
            UIManager::pressEnterToContinue();
        }
    } while (choice != 0);
}

void SystemSettingsController::handleScheduleGeneration() {
    Schedule schedule(settings_);
    int choice;
    do {
        UIManager::displayScheduleGenerationMenu();
        choice = UIManager::getIntInput("请输入您的选择: ", 0, 2);
        switch (choice) {
            case 1: {
                if (schedule.generateSchedule()) {
                    UIManager::showSuccessMessage("赛程生成成功！");
                } else {
                    UIManager::showErrorMessage("赛程生成失败，请检查项目、场地和时间段设置。");
                }
                break;
            }
            case 2: {
                schedule.printSchedule();
                break;
            }
            case 0:
                UIManager::showMessage("返回上一级菜单...");
                break;
            default:
                UIManager::showErrorMessage("无效选择。");
                break;
        }
        if (choice != 0) {
            UIManager::pressEnterToContinue();
        }
    } while (choice != 0);
}
