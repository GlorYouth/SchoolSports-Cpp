#include <iostream>
#include <limits>
#include <ranges>
#include <vector>
#include <string>
#include "include/SystemSettings.h"
#include "include/Registration.h"
#include "include/Schedule.h"
#include "include/DataManager.h"
#include "include/Constants.h" // 包含 genderToString, eventTypeToString

// 函数声明
void displayMainMenu();
void manageSystemSettings(SystemSettings& settings);
void manageRegistration(Registration& registration, SystemSettings& settings);
void manageSchedule(Schedule& schedule, SystemSettings& settings);
void manageResults(SystemSettings& settings); // 待实现
void manageData(DataManager& dataManager);

void listUnits(const SystemSettings& settings);
void listAthletes(const SystemSettings& settings);
void listEvents(const SystemSettings& settings);
void listScoreRules(const SystemSettings& settings);


int main() {
    SystemSettings settings;
    settings.initializeDefaultSettings(); // 初始化一些默认数据

    Registration registration(settings);
    Schedule schedule(settings);
    DataManager dataManager(settings);

    int choice;
    do {
        displayMainMenu();
        std::cout << "请输入您的选择: ";
        std::cin >> choice;

        // 清理输入缓冲区
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');


        switch (choice) {
            case 1:
                manageSystemSettings(settings);
                break;
            case 2: // 参赛项目发布 (通常是查看已有的)
                std::cout << "\n--- 当前发布的比赛项目 ---" << std::endl;
                listEvents(settings);
                break;
            case 3:
                manageRegistration(registration, settings);
                break;
            case 4: // 参赛信息查询 (简单实现)
                std::cout << "\n--- 参赛信息查询 ---" << std::endl;
                std::cout << "1. 查看参赛单位信息" << std::endl;
                std::cout << "2. 查看比赛项目信息" << std::endl;
                std::cout << "请选择查询类型: ";
                int queryChoice;
                std::cin >> queryChoice;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (queryChoice == 1) listUnits(settings);
                else if (queryChoice == 2) listEvents(settings);
                else std::cout << "无效选择。" << std::endl;
                break;
            case 5:
                manageSchedule(schedule, settings);
                break;
            case 6:
                manageResults(settings); // 比赛成绩录入与查询
                break;
            case 7: // 比赛成绩统计 (目前仅显示单位总分)
                std::cout << "\n--- 学院得分统计 ---" << std::endl;
                for(const auto& unit_pair : settings.getAllUnits()){
                    const Unit& unit = unit_pair.second;
                    // 实际总分应在成绩录入后计算并更新到Unit对象
                    // 这里仅为演示，假设分数已存在
                    std::cout << unit.getName() << ": " << unit.getTotalScore() << " 分" << std::endl;
                }
                break;
            case 8:
                manageData(dataManager);
                break;
            case 0:
                std::cout << "感谢使用学校运动会管理系统！正在退出..." << std::endl;
                break;
            default:
                std::cout << "无效选择，请重新输入。" << std::endl;
        }
        if (choice != 0) {
            std::cout << "\n按 Enter键 返回主菜单...";
            std::cin.get();
        }
    } while (choice != 0);

    return 0;
}

void displayMainMenu() {
    std::cout << "\n========== 学校运动会管理系统 ==========" << std::endl;
    std::cout << "1. 系统设置管理 (单位、项目、运动员、计分规则)" << std::endl;
    std::cout << "2. 查看参赛项目" << std::endl;
    std::cout << "3. 运动员报名登记与管理" << std::endl;
    std::cout << "4. 参赛信息查询" << std::endl;
    std::cout << "5. 秩序册生成与查看" << std::endl;
    std::cout << "6. 比赛成绩录入与查询 (待实现)" << std::endl;
    std::cout << "7. 比赛成绩统计 (待实现)" << std::endl;
    std::cout << "8. 数据备份与恢复 (部分实现)" << std::endl;
    std::cout << "0. 退出系统" << std::endl;
    std::cout << "======================================" << std::endl;
}

void manageSystemSettings(SystemSettings& settings) {
    int choice;
    do {
        std::cout << "\n--- 系统设置管理 ---" << std::endl;
        std::cout << "1. 添加参赛单位" << std::endl;
        std::cout << "2. 查看所有单位" << std::endl;
        std::cout << "3. 添加比赛项目" << std::endl;
        std::cout << "4. 查看所有项目" << std::endl;
        std::cout << "5. 添加运动员" << std::endl;
        std::cout << "6. 查看所有运动员" << std::endl;
        std::cout << "7. 添加计分规则" << std::endl;
        std::cout << "8. 查看所有计分规则" << std::endl;
        std::cout << "9. 设置运动员最大参赛项目数 (当前: " << settings.getAthleteMaxEventsAllowed() << ")" << std::endl;
        std::cout << "10. 设置项目最少举行人数 (当前: " << settings.getMinParticipantsToHoldEvent() << ")" << std::endl;
        std::cout << "0. 返回主菜单" << std::endl;
        std::cout << "请输入您的选择: ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::string name, desc;
        int id, unitId, minP, maxP, ranks, tempInt;
        double scoreVal;
        Gender gender;
        EventType eventType;
        std::map<int, double> scoresMap;

        switch (choice) {
            case 1: // 添加单位
                std::cout << "请输入单位名称: ";
                std::getline(std::cin, name);
                if(settings.addUnit(name)) std::cout << "单位 '" << name << "' 添加成功。" << std::endl;
                else std::cout << "单位添加失败 (可能已存在)。" << std::endl;
                break;
            case 2: listUnits(settings); break;
            case 3: // 添加项目
                std::cout << "请输入项目名称: ";
                std::getline(std::cin, name);
                std::cout << "请选择项目类型 (0: 径赛, 1: 田赛): ";
                std::cin >> tempInt;
                eventType = tempInt == 0 ? EventType::TRACK : EventType::FIELD;
                std::cout << "请选择性别要求 (0: 男子, 1: 女子, 2: 不限): ";
                std::cin >> tempInt;
                gender = tempInt == 0 ? Gender::MALE : ((tempInt == 1) ? Gender::FEMALE : Gender::UNSPECIFIED);
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if(settings.addCompetitionEvent(name, eventType, gender)) std::cout << "项目 '" << name << "' 添加成功。" << std::endl;
                else std::cout << "项目添加失败。" << std::endl;
                break;
            case 4: listEvents(settings); break;
            case 5: // 添加运动员
                std::cout << "请输入运动员姓名: ";
                std::getline(std::cin, name);
                std::cout << "请选择运动员性别 (0: 男, 1: 女): ";
                std::cin >> tempInt;
                gender = (tempInt == 0) ? Gender::MALE : Gender::FEMALE;
                listUnits(settings);
                std::cout << "请输入运动员所属单位ID: ";
                std::cin >> unitId;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if(settings.addAthlete(name, gender, unitId)) std::cout << "运动员 '" << name << "' 添加成功。" << std::endl;
                else std::cout << "运动员添加失败 (单位ID可能无效)。" << std::endl;
                break;
            case 6: listAthletes(settings); break;
            case 7: // 添加计分规则
                std::cout << "请输入规则描述 (例如: 超过6人取前5名): ";
                std::getline(std::cin, desc);
                std::cout << "请输入适用此规则的最小参赛人数: ";
                std::cin >> minP;
                std::cout << "请输入适用此规则的最大参赛人数 (-1表示无上限): ";
                std::cin >> maxP;
                std::cout << "请输入录取名次数: ";
                std::cin >> ranks;
                scoresMap.clear();
                for (int i = 1; i <= ranks; ++i) {
                    std::cout << "请输入第 " << i << " 名的分数: ";
                    std::cin >> scoreVal;
                    scoresMap[i] = scoreVal;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if(settings.addScoreRule(desc, minP, maxP, ranks, scoresMap)) std::cout << "计分规则添加成功。" << std::endl;
                else std::cout << "计分规则添加失败。" << std::endl;
                break;
            case 8: listScoreRules(settings); break;
            case 9:
                std::cout << "请输入新的运动员最大参赛项目数: ";
                std::cin >> tempInt;
                settings.setAthleteMaxEventsAllowed(tempInt);
                std::cout << "设置成功。" << std::endl;
                break;
            case 10:
                std::cout << "请输入新的项目最少举行人数: ";
                std::cin >> tempInt;
                settings.setMinParticipantsToHoldEvent(tempInt);
                std::cout << "设置成功。" << std::endl;
                break;
            case 0: break;
            default: std::cout << "无效选择。" << std::endl;
        }
         if (choice != 0) {
            std::cout << "\n按 Enter键 返回系统设置菜单...";
            std::cin.get();
        }
    } while (choice != 0);
}

void manageRegistration(Registration& registration, SystemSettings& settings) {
    int choice;
    do {
        std::cout << "\n--- 运动员报名与管理 ---" << std::endl;
        std::cout << "1. 运动员报名参赛项目" << std::endl;
        std::cout << "2. 运动员取消报名" << std::endl;
        std::cout << "3. 查看所有运动员及其报名情况" << std::endl;
        std::cout << "4. 查看所有项目及其报名情况" << std::endl;
        std::cout << "5. 检查并取消人数不足的项目" << std::endl;
        std::cout << "0. 返回主菜单" << std::endl;
        std::cout << "请输入您的选择: ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        int athleteId, eventId;

        switch (choice) {
            case 1:
                listAthletes(settings);
                std::cout << "请输入要报名的运动员ID: ";
                std::cin >> athleteId;
                listEvents(settings);
                std::cout << "请输入要报名的项目ID: ";
                std::cin >> eventId;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                registration.registerAthleteForEvent(athleteId, eventId);
                break;
            case 2:
                listAthletes(settings);
                std::cout << "请输入要取消报名的运动员ID: ";
                std::cin >> athleteId;
                // 可以进一步优化，只列出该运动员已报名的项目
                listEvents(settings);
                std::cout << "请输入要取消报名的项目ID: ";
                std::cin >> eventId;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                registration.unregisterAthleteFromEvent(athleteId, eventId);
                break;
            case 3:
                listAthletes(settings); // 已包含报名信息
                break;
            case 4:
                listEvents(settings); // 已包含报名信息
                break;
            case 5:
                registration.checkAndCancelEventsDueToLowParticipation();
                break;
            case 0: break;
            default: std::cout << "无效选择。" << std::endl;
        }
        if (choice != 0) {
            std::cout << "\n按 Enter键 返回报名管理菜单...";
            std::cin.get();
        }
    } while (choice != 0);
}

void manageSchedule(Schedule& schedule, SystemSettings& settings) {
    int choice;
    do {
        std::cout << "\n--- 秩序册管理 ---" << std::endl;
        std::cout << "1. 自动生成秩序册 (初步)" << std::endl;
        std::cout << "2. 查看秩序册" << std::endl;
        std::cout << "3. 验证秩序册 (占位符)" << std::endl;
        std::cout << "0. 返回主菜单" << std::endl;
        std::cout << "请输入您的选择: ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                if (schedule.generateSchedule()) {
                    std::cout << "秩序册已生成/更新。" << std::endl;
                } else {
                    std::cout << "秩序册生成失败 (可能没有有效项目)。" << std::endl;
                }
                break;
            case 2:
                schedule.printSchedule();
                break;
            case 3:
                if (schedule.validateSchedule()) {
                    std::cout << "赛程验证通过 (占位符)。" << std::endl;
                } else {
                    std::cout << "赛程验证失败 (占位符)。" << std::endl;
                }
                break;
            case 0: break;
            default: std::cout << "无效选择。" << std::endl;
        }
        if (choice != 0) {
            std::cout << "\n按 Enter键 返回秩序册管理菜单...";
            std::cin.get();
        }
    } while (choice != 0);
}

void manageResults(SystemSettings& settings) {
    std::cout << "\n--- 比赛成绩录入与查询 ---" << std::endl;
    std::cout << "此功能正在开发中..." << std::endl;
    // 后续实现：
    // 1. 选择项目录入成绩
    // 2. 为每个项目输入运动员名次和成绩 (注意田赛和径赛成绩格式和比较方法)
    // 3. 系统根据计分规则自动计算得分
    // 4. 更新运动员个人得分和单位总分
    // 5. 查询项目成绩、运动员成绩、单位成绩
}

void manageData(DataManager& dataManager) {
    int choice;
    std::string filePath;
    std::cout << "\n--- 数据管理 ---" << std::endl;
    std::cout << "1. 备份数据" << std::endl;
    std::cout << "2. 恢复数据 (功能不完整)" << std::endl;
    std::cout << "0. 返回主菜单" << std::endl;
    std::cout << "请输入您的选择: ";
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    switch (choice) {
        case 1:
            std::cout << "请输入备份文件名 (例如: backup.txt): ";
            std::getline(std::cin, filePath);
            if (dataManager.backupData(filePath)) {
                std::cout << "数据已成功备份到 " << filePath << std::endl;
            } else {
                std::cout << "数据备份失败。" << std::endl;
            }
            break;
        case 2:
            std::cout << "请输入要恢复数据的文件名: ";
            std::getline(std::cin, filePath);
            if (dataManager.restoreData(filePath)) {
                std::cout << "数据已成功从 " << filePath << " 恢复。" << std::endl;
            } else {
                std::cout << "数据恢复失败或功能未完全实现。" << std::endl;
            }
            break;
        case 0: break;
        default: std::cout << "无效选择。" << std::endl;
    }
}


void listUnits(const SystemSettings& settings) {
    std::cout << "\n--- 所有参赛单位 ---" << std::endl;
    if (settings.getAllUnits().empty()) {
        std::cout << "暂无参赛单位。" << std::endl;
        return;
    }
    for (const auto& pair : settings.getAllUnits()) {
        const Unit& unit = pair.second;
        std::cout << "ID: " << unit.getId() << ", 名称: " << unit.getName()
                  << ", 总分: " << unit.getTotalScore()
                  << ", 运动员数: " << unit.getAthleteIds().size() << std::endl;
    }
}

void listAthletes(const SystemSettings& settings) {
    std::cout << "\n--- 所有运动员 ---" << std::endl;
    if (settings.getAllAthletes().empty()) {
        std::cout << "暂无运动员。" << std::endl;
        return;
    }
    for (const auto &val: settings.getAllAthletes() | std::views::values) {
        const Athlete& athlete = val;
        auto unit = settings.getUnitConst(athlete.getUnitId()); // 注意：const_cast 可能需要，或者 getUnit 返回 const Unit*
        std::cout << "ID: " << athlete.getId() << ", 姓名: " << athlete.getName()
                  << ", 性别: " << genderToString(athlete.getGender())
                  << ", 单位: " << (unit ? unit.value().get().getName() : "未知") << " (ID:" << athlete.getUnitId() << ")"
                  << ", 已报项目数: " << athlete.getRegisteredEventsCount();
        if (athlete.getRegisteredEventsCount() > 0) {
            std::cout << " [项目ID: ";
            const auto& events = athlete.getRegisteredEventIds();
            for (size_t i = 0; i < events.size(); ++i) {
                std::cout << events[i] << (i == events.size() - 1 ? "" : ", ");
            }
            std::cout << "]";
        }
        std::cout << std::endl;
    }
}

void listEvents(const SystemSettings& settings) {
    std::cout << "\n--- 所有比赛项目 ---" << std::endl;
    if (settings.getAllCompetitionEvents().empty()) {
        std::cout << "暂无比赛项目。" << std::endl;
        return;
    }
    for (const auto& pair : settings.getAllCompetitionEvents()) {
        const CompetitionEvent& event = pair.second;
        auto rule = settings.getScoreRuleConst(event.getScoreRuleId()); // 同上，const_cast 或 const ScoreRule*
        std::cout << "ID: " << event.getId() << ", 名称: " << event.getName()
                  << ", 类型: " << eventTypeToString(event.getEventType())
                  << ", 性别要求: " << genderToString(event.getGenderRequirement())
                  << ", 参赛人数: " << event.getParticipantCount()
                  << ", 计分规则ID: " << (event.getScoreRuleId() != -1 ? std::to_string(event.getScoreRuleId()) : "未指定")
                  << (rule.has_value() ? " (" + rule.value().get().getDescription() + ")" : "")
                  << (event.getIsCancelled() ? " [已取消]" : "");
        if (event.getParticipantCount() > 0) {
            std::cout << "\n  参赛者ID: ";
            const auto& participants = event.getParticipantAthleteIds();
            for (size_t i = 0; i < participants.size(); ++i) {
                std::cout << participants[i] << (i == participants.size() - 1 ? "" : ", ");
            }
        }
        std::cout << std::endl;
    }
}

void listScoreRules(const SystemSettings& settings) {
    std::cout << "\n--- 所有计分规则 ---" << std::endl;
    if (settings.getAllScoreRules().empty()) {
        std::cout << "暂无计分规则。" << std::endl;
        return;
    }
    for (const auto& pair : settings.getAllScoreRules()) {
        const ScoreRule& rule = pair.second;
        std::cout << "ID: " << rule.getId() << ", 描述: " << rule.getDescription()
                  << ", 适用人数: " << rule.appliesTo(0) /*简化显示*/ << " (实际应显示min-max)"
                  << ", 录取名次: " << rule.getRanksToAward() << std::endl;
        std::cout << "  名次与得分: ";
        for (const auto& scorePair : rule.getAllScoresForRanks()) {
            std::cout << "第" << scorePair.first << "名=" << scorePair.second << "分; ";
        }
        std::cout << std::endl;
    }
}

