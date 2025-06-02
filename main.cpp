#include <iostream>
#include <limits>
#include <ranges>
#include <vector>
#include <string>
#include <algorithm> // 用于 std::sort
#include <iomanip>   // 用于 std::fixed, std::setprecision (格式化输出)

#include "include/SystemSettings.h"
#include "include/Registration.h"
#include "include/Schedule.h"
#include "include/DataManager.h"
#include "include/Constants.h" // 包含 genderToString, eventTypeToString
#include "include/Result.h"   // 确保包含 Result.h 以便在 main 中访问 EventResults
#include "include/AutoTest.h"

// 函数声明
void displayMainMenu();
void manageSystemSettings(SystemSettings& settings);
void manageRegistration(Registration& registration, SystemSettings& settings);
void manageSchedule(Schedule& schedule, SystemSettings& settings);
void manageResults(SystemSettings& settings); // 更新
void manageData(DataManager& dataManager);

void listUnits(const SystemSettings& settings);
void listAthletes(const SystemSettings& settings);
void listEvents(const SystemSettings& settings);
void listScoreRules(const SystemSettings& settings);

// 成绩管理相关的新函数声明
void recordEventResults(SystemSettings& settings); // 录入项目成绩
void viewEventResults(const SystemSettings& settings);   // 查看项目成绩
void viewUnitStandings(const SystemSettings& settings);  // 查看单位排名


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
        if (!(std::cin >> choice)) { // 基本输入验证
            std::cout << "无效输入，请输入一个数字。" << std::endl;
            std::cin.clear(); // 清除错误标志
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 忽略缓冲区中的无效输入
            choice = -1; // 设置为无效选项以重新循环
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 清理换行符
        }


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
                 if (!(std::cin >> queryChoice)) {
                    std::cout << "无效输入。" << std::endl;
                    std::cin.clear();
                }
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
            case 7: // 比赛成绩统计 (现在调用 viewUnitStandings)
                viewUnitStandings(settings);
                break;
            case 8:
                manageData(dataManager);
                break;
            case 9:
                autoTest();
            case 0:
                std::cout << "感谢使用学校运动会管理系统！正在退出..." << std::endl;
                break;
            default:
                std::cout << "无效选择，请重新输入。" << std::endl;
        }
        if (choice != 0 && choice != -1) { // 如果不是退出或无效输入错误，则暂停
            std::cout << "\n按 Enter键 返回主菜单...";
            std::cin.get(); // 等待用户按 Enter
        }
    } while (choice != 0);
}

void displayMainMenu() {
    std::cout << "\n========== 学校运动会管理系统 ==========" << std::endl;
    std::cout << "1. 系统设置管理 (单位、项目、运动员、计分规则)" << std::endl;
    std::cout << "2. 查看参赛项目" << std::endl;
    std::cout << "3. 运动员报名登记与管理" << std::endl;
    std::cout << "4. 参赛信息查询" << std::endl;
    std::cout << "5. 秩序册生成与查看" << std::endl;
    std::cout << "6. 比赛成绩录入与查询" << std::endl; // 更新
    std::cout << "7. 比赛成绩统计 (单位排名)" << std::endl;   // 更新
    std::cout << "8. 数据备份与恢复 (部分实现)" << std::endl;
    std::cout << "9. 自动测试" << std::endl;
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
        if (!(std::cin >> choice)) {
            std::cout << "无效输入，请输入一个数字。" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1; // 标记为无效输入
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }


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
                else std::cout << "单位添加失败 (可能名称已存在或输入无效)。" << std::endl;
                break;
            case 2: listUnits(settings); break;
            case 3: // 添加项目
                std::cout << "请输入项目名称: ";
                std::getline(std::cin, name);
                std::cout << "请选择项目类型 (0: 径赛, 1: 田赛): ";
                std::cin >> tempInt;  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 读取后清空缓冲区
                eventType = (tempInt == 0) ? EventType::TRACK : ((tempInt == 1) ? EventType::FIELD : EventType::UNSPECIFIED);
                if (eventType == EventType::UNSPECIFIED) { std::cout << "无效的项目类型选择。" << std::endl; break;}

                std::cout << "请选择性别要求 (0: 男子, 1: 女子, 2: 不限): ";
                std::cin >> tempInt; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                gender = (tempInt == 0) ? Gender::MALE : ((tempInt == 1) ? Gender::FEMALE : Gender::UNSPECIFIED);
                // UNSPECIFIED 性别要求是允许的

                if(settings.addCompetitionEvent(name, eventType, gender)) std::cout << "项目 '" << name << "' 添加成功。" << std::endl;
                else std::cout << "项目添加失败。" << std::endl;
                break;
            case 4: listEvents(settings); break;
            case 5: // 添加运动员
                std::cout << "请输入运动员姓名: ";
                std::getline(std::cin, name);
                std::cout << "请选择运动员性别 (0: 男, 1: 女): ";
                std::cin >> tempInt; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                gender = (tempInt == 0) ? Gender::MALE : ((tempInt == 1) ? Gender::FEMALE : Gender::UNSPECIFIED); // 运动员性别不能是UNSPECIFIED
                 if (gender == Gender::UNSPECIFIED) { std::cout << "无效的性别选择。" << std::endl; break;}

                listUnits(settings);
                if (settings.getAllUnits().empty()) {std::cout << "请先添加参赛单位。" <<std::endl; break;}
                std::cout << "请输入运动员所属单位ID: ";
                std::cin >> unitId; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if(settings.addAthlete(name, gender, unitId)) std::cout << "运动员 '" << name << "' 添加成功。" << std::endl;
                else std::cout << "运动员添加失败 (单位ID可能无效或输入错误)。" << std::endl;
                break;
            case 6: listAthletes(settings); break;
            case 7: // 添加计分规则
                std::cout << "请输入规则描述 (例如: 超过6人取前5名): ";
                std::getline(std::cin, desc);
                std::cout << "请输入适用此规则的最小参赛人数: ";
                std::cin >> minP; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "请输入适用此规则的最大参赛人数 (-1表示无上限): ";
                std::cin >> maxP; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "请输入录取名次数: ";
                std::cin >> ranks; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (ranks <=0) {std::cout << "录取名次数必须大于0。" << std::endl; break;}
                scoresMap.clear();
                for (int i = 1; i <= ranks; ++i) {
                    std::cout << "请输入第 " << i << " 名的分数: ";
                    std::cin >> scoreVal; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    scoresMap[i] = scoreVal;
                }
                if(settings.addScoreRule(desc, minP, maxP, ranks, scoresMap)) std::cout << "计分规则添加成功。" << std::endl;
                else std::cout << "计分规则添加失败。" << std::endl;
                break;
            case 8: listScoreRules(settings); break;
            case 9:
                std::cout << "请输入新的运动员最大参赛项目数: ";
                std::cin >> tempInt; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (tempInt > 0) {
                    settings.setAthleteMaxEventsAllowed(tempInt);
                    std::cout << "设置成功。" << std::endl;
                } else {
                    std::cout << "无效的数值。" << std::endl;
                }
                break;
            case 10:
                std::cout << "请输入新的项目最少举行人数: ";
                std::cin >> tempInt; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                 if (tempInt > 0) {
                    settings.setMinParticipantsToHoldEvent(tempInt);
                    std::cout << "设置成功。" << std::endl;
                } else {
                    std::cout << "无效的数值。" << std::endl;
                }
                break;
            case 0: break;
            default: std::cout << "无效选择。" << std::endl;
        }
         if (choice != 0 && choice != -1) { // 如果不是退出或无效输入错误，则暂停
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
        if (!(std::cin >> choice)) {
            std::cout << "无效输入，请输入一个数字。" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1;
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }


        int athleteId, eventId;

        switch (choice) {
            case 1:
                if (settings.getAllAthletes().empty()) {std::cout << "请先添加运动员。" << std::endl; break;}
                listAthletes(settings);
                std::cout << "请输入要报名的运动员ID: ";
                std::cin >> athleteId; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if (settings.getAllCompetitionEvents().empty()) {std::cout << "请先添加比赛项目。" << std::endl; break;}
                listEvents(settings);
                std::cout << "请输入要报名的项目ID: ";
                std::cin >> eventId; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                registration.registerAthleteForEvent(athleteId, eventId);
                break;
            case 2:
                if (settings.getAllAthletes().empty()) {std::cout << "系统中没有运动员。" << std::endl; break;}
                listAthletes(settings);
                std::cout << "请输入要取消报名的运动员ID: ";
                std::cin >> athleteId; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                // 可以考虑只列出该运动员已报名的项目
                // 目前，列出所有项目
                if (settings.getAllCompetitionEvents().empty()) {std::cout << "系统中没有比赛项目。" << std::endl; break;}
                listEvents(settings);
                std::cout << "请输入要取消报名的项目ID: ";
                std::cin >> eventId; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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
        if (choice != 0 && choice != -1) {
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
        if (!(std::cin >> choice)) {
            std::cout << "无效输入，请输入一个数字。" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1;
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }


        switch (choice) {
            case 1:
                if (schedule.generateSchedule()) {
                    std::cout << "秩序册已生成/更新。" << std::endl;
                } else {
                    std::cout << "秩序册生成失败 (可能没有有效项目或未取消项目)。" << std::endl;
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
        if (choice != 0 && choice != -1) {
            std::cout << "\n按 Enter键 返回秩序册管理菜单...";
            std::cin.get();
        }
    } while (choice != 0);
}

// 更新后的 manageResults 函数
void manageResults(SystemSettings& settings) {
    int choice;
    do {
        std::cout << "\n--- 比赛成绩录入与查询 ---" << std::endl;
        std::cout << "1. 录入/修改项目成绩" << std::endl;
        std::cout << "2. 查看指定项目成绩" << std::endl;
        std::cout << "3. 查看所有单位得分排名" << std::endl;
        std::cout << "0. 返回主菜单" << std::endl;
        std::cout << "请输入您的选择: ";
        if (!(std::cin >> choice)) {
            std::cout << "无效输入，请输入一个数字。" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1;
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }


        switch (choice) {
            case 1:
                recordEventResults(settings);
                break;
            case 2:
                viewEventResults(settings);
                break;
            case 3:
                viewUnitStandings(settings);
                break;
            case 0:
                break;
            default:
                std::cout << "无效选择。" << std::endl;
        }
        if (choice != 0 && choice != -1) {
            std::cout << "\n按 Enter键 返回成绩管理菜单...";
            std::cin.get();
        }
    } while (choice != 0);
}


void manageData(DataManager& dataManager) {
    int choice;
    std::string filePath;
    std::cout << "\n--- 数据管理 ---" << std::endl;
    std::cout << "1. 备份数据" << std::endl;
    std::cout << "2. 恢复数据 (功能不完整)" << std::endl;
    std::cout << "0. 返回主菜单" << std::endl;
    std::cout << "请输入您的选择: ";
     if (!(std::cin >> choice)) {
            std::cout << "无效输入，请输入一个数字。" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1;
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }


    switch (choice) {
        case 1:
            std::cout << "请输入备份文件名 (例如: backup.dat): "; // 为了清晰，更改了扩展名
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
                 std::cout << "注意：恢复后，请检查所有数据是否正确，特别是单位总分需要基于恢复的成绩重新计算或通过备份恢复。" << std::endl;
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
    // 表头
    std::cout << std::left << std::setw(5) << "ID" << "| "
              << std::setw(20) << "名称" << "| "
              << std::setw(10) << "总分" << "| "
              << "运动员数" << std::endl;
    std::cout << "-----|----------------------|------------|------------" << std::endl;
    // 数据行
    for (const auto& pair : settings.getAllUnits()) {
        const Unit& unit = pair.second;
        std::cout << std::left << std::setw(5) << unit.getId() << "| "
                  << std::setw(20) << unit.getName() << "| "
                  << std::fixed << std::setprecision(1) << std::setw(10) << unit.getTotalScore() << "| " // 总分保留一位小数
                  << unit.getAthleteIds().size() << std::endl;
    }
}

void listAthletes(const SystemSettings& settings) {
    std::cout << "\n--- 所有运动员 ---" << std::endl;
    if (settings.getAllAthletes().empty()) {
        std::cout << "暂无运动员。" << std::endl;
        return;
    }
    // 表头
    std::cout << std::left
              << std::setw(5) << "ID" << "| "
              << std::setw(15) << "姓名" << "| "
              << std::setw(8) << "性别" << "| "
              << std::setw(20) << "单位 (ID)" << "| "
              << "已报项目数/项目ID" << std::endl;
    std::cout << "-----|-----------------|----------|----------------------|--------------------" << std::endl;
    // 数据行
    for (const auto &val: settings.getAllAthletes() | std::views::values) {
        const Athlete& athlete = val;
        auto unitOpt = settings.getUnitConst(athlete.getUnitId()); // 使用 const 版本获取单位
        std::string unitNameAndId = (unitOpt ? unitOpt.value().get().getName() : "未知") + " (" + std::to_string(athlete.getUnitId()) + ")";

        std::cout << std::left
                  << std::setw(5) << athlete.getId() << "| "
                  << std::setw(15) << athlete.getName() << "| "
                  << std::setw(8) << genderToString(athlete.getGender()) << "| "
                  << std::setw(20) << unitNameAndId << "| "
                  << std::setw(2) << athlete.getRegisteredEventsCount(); // 已报项目数
        if (athlete.getRegisteredEventsCount() > 0) {
            std::cout << " [IDs: ";
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
    // 表头
     std::cout << std::left
              << std::setw(5) << "ID" << "| "
              << std::setw(25) << "项目名称" << "| "
              << std::setw(8) << "类型" << "| "
              << std::setw(10) << "性别要求" << "| "
              << std::setw(6) << "人数" << "| "
              << std::setw(12) << "计分规则ID" << "| " // 调整宽度
              << "状态"
              << std::endl;
    std::cout << "-----|---------------------------|----------|------------|--------|--------------|--------" << std::endl;
    // 数据行
    for (const auto& pair : settings.getAllCompetitionEvents()) {
        const CompetitionEvent& event = pair.second;
        auto ruleOpt = settings.getScoreRuleConst(event.getScoreRuleId()); // 使用 const 版本
        std::string ruleDescPart = (ruleOpt.has_value() ? " (" + ruleOpt.value().get().getDescription() + ")" : ""); // 规则描述部分

        std::cout << std::left
                  << std::setw(5) << event.getId() << "| "
                  << std::setw(25) << event.getName() << "| "
                  << std::setw(8) << eventTypeToString(event.getEventType()) << "| "
                  << std::setw(10) << genderToString(event.getGenderRequirement()) << "| "
                  << std::setw(6) << event.getParticipantCount() << "| "
                  << std::setw(12) << (event.getScoreRuleId() != -1 ? std::to_string(event.getScoreRuleId()) : "未指定") << "| " // 显示计分规则ID
                  << (event.getIsCancelled() ? "已取消" : "正常");

        // 可选：在下方或按需显示规则描述或参赛者ID
        // if (!event.getIsCancelled() && ruleOpt.has_value()) {
        //     // std::cout << "\n  规则: " << ruleOpt.value().get().getDescription();
        // }
        // if (event.getParticipantCount() > 0) {
        //      // std::cout << "\n  参赛者ID: ";
        //      // const auto& participants = event.getParticipantAthleteIds();
        //      // for (size_t i = 0; i < participants.size(); ++i) {
        //      //     std::cout << participants[i] << (i == participants.size() - 1 ? "" : ", ");
        //      // }
        // }
        std::cout << std::endl;
    }
}

void listScoreRules(const SystemSettings& settings) {
    std::cout << "\n--- 所有计分规则 ---" << std::endl;
    if (settings.getAllScoreRules().empty()) {
        std::cout << "暂无计分规则。" << std::endl;
        return;
    }
    // 表头
    std::cout << std::left << std::setw(5) << "ID" << "| "
              << std::setw(30) << "描述" << "| "
              << std::setw(15) << "适用人数(Min)" << "| " // 简化显示
              << std::setw(15) << "适用人数(Max)" << "| "
              << "录取名次" << std::endl;
    std::cout << "-----|--------------------------------|-----------------|-----------------|------------" << std::endl;
    // 数据行
    for (const auto& pair : settings.getAllScoreRules()) {
        const ScoreRule& rule = pair.second;
        // ScoreRule 类应提供 getMinParticipants() 和 getMaxParticipants() 方法以便在此处显示。
        // 当前假设这些方法已添加（此处不显示添加过程以保持差异简洁）。
        // 临时方案：如果 ScoreRule 没有这些 getter，则显示 " (见描述)"。

        std::cout << std::left << std::setw(5) << rule.getId() << "| "
                  << std::setw(30) << rule.getDescription() << "| "
                //   << std::setw(15) << rule.getMinParticipants() << "| " // 假设有 getter
                //   << std::setw(15) << (rule.getMaxParticipants() == -1 ? "无上限" : std::to_string(rule.getMaxParticipants())) << "| " // 假设有 getter
                  << std::setw(15) << " (见描述)" << "| "  // 临时显示方式
                  << std::setw(15) << " (见描述)" << "| "  // 临时显示方式
                  << rule.getRanksToAward() << std::endl;
        std::cout << "  名次与得分: ";
        for (const auto& scorePair : rule.getAllScoresForRanks()) {
            std::cout << "第" << scorePair.first << "名=" << scorePair.second << "分; ";
        }
        std::cout << std::endl;
    }
}


// --- 新的成绩管理函数 ---

void recordEventResults(SystemSettings& settings) {
    std::cout << "\n--- 录入/修改项目成绩 ---" << std::endl;
    if (settings.getAllCompetitionEvents().empty()) {
        std::cout << "系统中没有比赛项目，无法录入成绩。" << std::endl;
        return;
    }
    listEvents(settings); // 列出所有项目供选择
    std::cout << "请输入要录入成绩的项目ID: ";
    int eventId;
    if (!(std::cin >> eventId)) { // 输入验证
        std::cout << "无效的项目ID输入。" << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 清理换行符

    auto eventOpt = settings.getCompetitionEvent(eventId); // 获取项目对象
    if (!eventOpt) {
        std::cout << "错误: 项目ID " << eventId << " 不存在。" << std::endl;
        return;
    }
    CompetitionEvent& competitionEvent = eventOpt.value().get(); // 获取项目引用

    if (competitionEvent.getIsCancelled()) {
        std::cout << "项目 \"" << competitionEvent.getName() << "\" 已被取消，无法录入成绩。" << std::endl;
        return;
    }

    // 防御性检查：如果项目因人数不足而应取消但未被标记（例如报名后有人取消），则在此处理
    if (competitionEvent.getParticipantCount() < settings.getMinParticipantsToHoldEvent()) {
         std::cout << "项目 \"" << competitionEvent.getName() << "\" 参赛人数 (" << competitionEvent.getParticipantCount()
                  << ") 不足最少要求 (" << settings.getMinParticipantsToHoldEvent()
                  << ")。" << std::endl;
        if (!competitionEvent.getIsCancelled()){ // 如果尚未标记为取消
             competitionEvent.setCancelled(true); // 标记为取消
             // 此处无需从运动员的已报名列表中移除，因为他们确实报名了。
             // 只是该项目本身不会产生积分/成绩。
             std::cout << "项目 \"" << competitionEvent.getName() << "\" 现已自动标记为取消，无法录入成绩。" << std::endl;
        }
        return;
    }

    // 清除该项目的旧成绩（这将从单位总分中扣除旧分数）
    settings.clearResultsForEvent(eventId);

    // 查找适用的计分规则
    auto scoreRuleOpt = settings.findAppropriateScoreRule(competitionEvent.getParticipantCount());
    if (!scoreRuleOpt) {
        std::cout << "错误: 未找到适用于项目 \"" << competitionEvent.getName()
                  << "\" (参赛人数: " << competitionEvent.getParticipantCount()
                  << ") 的计分规则。请先定义相关计分规则。" << std::endl;
        return;
    }
    ScoreRule& scoreRule = scoreRuleOpt.value().get(); // 获取计分规则引用
    std::cout << "应用计分规则: " << scoreRule.getDescription() << std::endl;

    EventResults newEventResults(eventId); // 为本次录入创建新的 EventResults 对象
    int ranksToAward = scoreRule.getRanksToAward(); // 获取应录取名次数
    std::cout << "请为项目 \"" << competitionEvent.getName() << "\" 录入前 " << ranksToAward << " 名的成绩。" << std::endl;

    // 列出参与此项目的运动员，方便裁判查看
    std::cout << "以下是参与此项目的运动员:" << std::endl;
    bool participantsExist = false;
    for(int athleteId_participant : competitionEvent.getParticipantAthleteIds()){
        auto ath = settings.getAthlete(athleteId_participant);
        if(ath) {
            std::cout << "  ID: " << ath.value().get().getId() << ", 姓名: " << ath.value().get().getName()
                      << ", 单位ID: " << ath.value().get().getUnitId() << std::endl;
            participantsExist = true;
        }
    }
    if (!participantsExist) {
        std::cout << "此项目没有有效的报名运动员。" << std::endl;
        return;
    }


    std::vector<int> awardedAthleteIdsThisSession; // 用于防止同一运动员在本次录入中获得多个名次

    for (int i = 1; i <= ranksToAward; ++i) {
        int rank = i; // 当前录入的名次
        int athleteId_input;
        std::string scoreRecordStr;

        std::cout << "\n请输入第 " << rank << " 名的运动员ID (输入0跳过此名次或结束录入): ";
        if (!(std::cin >> athleteId_input)) {
            std::cout << "无效的运动员ID输入。此名次跳过。" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue; // 跳过当前名次
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (athleteId_input == 0) {
            std::cout << "已跳过第 " << rank << " 名及之后名次的录入。" << std::endl;
            break; // 结束录入循环
        }

        auto athleteOpt = settings.getAthlete(athleteId_input); // 获取运动员对象
        if (!athleteOpt) {
            std::cout << "错误: 运动员ID " << athleteId_input << " 不存在。此名次作废。" << std::endl;
            i--; // 重试当前名次
            continue;
        }
        Athlete& rankedAthlete = athleteOpt.value().get(); // 获取运动员引用

        // 验证运动员是否确实报名了此项目
        bool participated = false;
        for(int pId : competitionEvent.getParticipantAthleteIds()){
            if(pId == athleteId_input) {
                participated = true;
                break;
            }
        }
        if(!participated){
            std::cout << "错误: 运动员 " << rankedAthlete.getName() << " (ID: " << athleteId_input
                      << ") 未报名参加此项目 \"" << competitionEvent.getName() << "\"。此名次作废。" << std::endl;
            i--; // 重试当前名次
            continue;
        }

        // 检查运动员是否已在本次录入中获得过名次
        if (std::find(awardedAthleteIdsThisSession.begin(), awardedAthleteIdsThisSession.end(), athleteId_input) != awardedAthleteIdsThisSession.end()) {
            std::cout << "错误: 运动员 " << rankedAthlete.getName() << " (ID: " << athleteId_input
                      << ") 已在此次录入中获得名次。此名次作废。" << std::endl;
            i--; // 重试当前名次
            continue;
        }

        std::cout << "请输入第 " << rank << " 名 (运动员 " << rankedAthlete.getName() << ") 的成绩记录 (如 10.5s, 2.10m): ";
        std::getline(std::cin, scoreRecordStr); // 读取成绩字符串

        double points = scoreRule.getScoreForRank(rank); // 根据名次获取分数

        Result result(eventId, athleteId_input, rank, scoreRecordStr, points); // 创建成绩对象
        newEventResults.addResult(result); // 添加到当前项目的成绩列表中
        awardedAthleteIdsThisSession.push_back(athleteId_input); // 记录已获奖的运动员ID

        // 更新运动员所属单位的总分
        auto unitOpt = settings.getUnit(rankedAthlete.getUnitId());
        if (unitOpt) {
            unitOpt.value().get().addScore(points); // 增加新的分数
            std::cout << "运动员 " << rankedAthlete.getName() << " (单位: " << unitOpt.value().get().getName()
                      << ") 获得 " << points << " 分。单位总分更新。" << std::endl;
        } else {
            std::cout << "警告: 运动员 " << rankedAthlete.getName() << " 的单位ID (" << rankedAthlete.getUnitId() << ") 无效，分数未计入单位。" << std::endl;
        }
    }

    newEventResults.finalizeResults(); // 最终确定此项目的成绩（目前仅标记）
    settings.addOrUpdateEventResults(newEventResults); // 将新的成绩记录保存到系统中
    competitionEvent.setScoreRuleId(scoreRule.getId()); // 关联所使用的计分规则ID到项目

    std::cout << "\n项目 \"" << competitionEvent.getName() << "\" 的成绩已录入/更新完毕。" << std::endl;
}


void viewEventResults(const SystemSettings& settings) {
    std::cout << "\n--- 查看项目成绩 ---" << std::endl;
    if (settings.getAllCompetitionEvents().empty()) {
        std::cout << "系统中没有比赛项目。" << std::endl;
        return;
    }
    listEvents(settings);
    std::cout << "请输入要查看成绩的项目ID: ";
    int eventId;
    if (!(std::cin >> eventId)) {
        std::cout << "无效的项目ID输入。" << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    auto eventOpt = settings.getCompetitionEventConst(eventId); // 使用 const 版本获取项目
    if (!eventOpt) {
        std::cout << "错误: 项目ID " << eventId << " 不存在。" << std::endl;
        return;
    }
    const CompetitionEvent& competitionEvent = eventOpt.value().get();

    std::cout << "\n项目: " << competitionEvent.getName();
    if (competitionEvent.getIsCancelled()) {
        std::cout << " [状态: 已取消]" << std::endl;
        // 如果在取消前已录入成绩，仍然显示，但注明已取消
    }
     auto ruleOpt = settings.getScoreRuleConst(competitionEvent.getScoreRuleId());
    if (ruleOpt) {
        std::cout << " (计分规则: " << ruleOpt.value().get().getDescription() << ")" << std::endl;
    } else if (competitionEvent.getScoreRuleId() != -1) {
        std::cout << " (计分规则ID: " << competitionEvent.getScoreRuleId() << " - 未找到详细信息)" << std::endl;
    } else {
        std::cout << " (未指定计分规则)" << std::endl;
    }


    auto eventResultsOpt = settings.getEventResultsConst(eventId); // 使用 const 版本获取成绩
    if (!eventResultsOpt || eventResultsOpt.value().get().getResultsList().empty()) {
        std::cout << "项目 \"" << competitionEvent.getName() << "\" 尚无成绩记录。" << std::endl;
        return;
    }
    const EventResults& eventResults = eventResultsOpt.value().get();

    // 获取成绩列表并按名次排序显示
    auto resultsToDisplay = eventResults.getResultsList();
    std::sort(resultsToDisplay.begin(), resultsToDisplay.end(), [](const Result& a, const Result& b) {
        return a.getRank() < b.getRank(); // 按名次升序排序
    });

    // 表头
    std::cout << "\n" << std::left
              << std::setw(6) << "名次" << "| "
              << std::setw(18) << "运动员姓名" << "| "
              << std::setw(10) << "运动员ID" << "| "
              << std::setw(15) << "成绩记录" << "| "
              << "所获分数" << std::endl;
    std::cout << "------|--------------------|------------|-----------------|----------" << std::endl;
    // 数据行
    for (const auto& result : resultsToDisplay) {
        auto athleteOpt = settings.getAthleteConst(result.getAthleteId()); // 使用 const 版本获取运动员
        std::string athleteName = athleteOpt ? athleteOpt.value().get().getName() : "未知运动员";

        std::cout << std::left << std::setw(6) << result.getRank() << "| "
                  << std::setw(18) << athleteName << "| "
                  << std::setw(10) << result.getAthleteId() << "| "
                  << std::setw(15) << result.getScoreRecord() << "| "
                  << std::fixed << std::setprecision(1) << result.getPointsAwarded() << std::endl; // 分数保留一位小数
    }
}

void viewUnitStandings(const SystemSettings& settings) {
    std::cout << "\n--- 单位得分排名 ---" << std::endl;
    const auto& allUnitsMap = settings.getAllUnits();
    if (allUnitsMap.empty()) {
        std::cout << "系统中没有单位。" << std::endl;
        return;
    }

    // 将 map 中的单位复制到 vector 以便排序
    std::vector<std::reference_wrapper<const Unit>> unitsVec;
    for (const auto &val: allUnitsMap | std::views::values) {
        unitsVec.push_back(std::cref(val));
    }

    // 按总分降序排序，分数相同则按单位名称升序
    std::sort(unitsVec.begin(), unitsVec.end(), [](const Unit& a, const Unit& b) {
        if (a.getTotalScore() != b.getTotalScore()) {
            return a.getTotalScore() > b.getTotalScore(); // 按分数降序
        }
        return a.getName() < b.getName(); // 分数相同则按名称升序
    });

    // 表头
    std::cout << "\n" << std::left
              << std::setw(6) << "排名" << "| "
              << std::setw(25) << "单位名称" << "| "
              << "总得分" << std::endl;
    std::cout << "------|---------------------------|----------" << std::endl;
    // 数据行
    int rank = 1;
    double lastScore = -1.0; // 用于处理并列排名
    int displayRank = 1;
    for (const auto& unitRef : unitsVec) {
        const Unit& unit = unitRef.get();
        if (unit.getTotalScore() != lastScore && rank > 1) { // 如果分数不同且不是第一名，更新显示排名
            displayRank = rank;
        }
        std::cout << std::left << std::setw(6) << displayRank << "| "
                  << std::setw(25) << unit.getName() << "| "
                  << std::fixed << std::setprecision(1) << unit.getTotalScore() << std::endl; // 总分保留一位小数
        lastScore = unit.getTotalScore();
        rank++;
    }
}