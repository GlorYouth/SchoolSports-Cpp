#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <iomanip>
#include "SportsMeet.h"
#include "Unit.h"
#include "Event.h"
#include "Athlete.h"
#include "Gender.h"

#ifdef _WIN32
#include <windows.h>
#endif

// Function to set console to UTF-8 for proper character display on Windows
void setConsoleUTF8() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

// UI Helper Function Declarations
void showMainMenu();
void handleUnitManagement(SportsMeet& sm);
void handleEventManagement(SportsMeet& sm);
void handleRegistrationManagement(SportsMeet& sm);
void handleResultsManagement(SportsMeet& sm);
void handleInfoQuery(SportsMeet& sm);
void handleScheduleManagement(SportsMeet& sm);
void handleBackupAndRestore(SportsMeet& sm);
void handleScoringRuleManagement(SportsMeet& sm);
void handleSystemSettings(SportsMeet& sm);
void loadSampleData(SportsMeet& sm);

Event* selectEvent(SportsMeet& sm);
Unit* selectUnit(SportsMeet& sm);

int main() {
    setConsoleUTF8();
    SportsMeet sm;
    
    std::cout << "是否加载示例数据? (y/n): ";
    char choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (choice == 'y' || choice == 'Y') {
        loadSampleData(sm);
    }

    while (true) {
        showMainMenu();
        int mainChoice;
        std::cin >> mainChoice;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "无效输入，请输入数字。\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (mainChoice) {
            case 1:
                handleUnitManagement(sm);
                break;
            case 2:
                handleEventManagement(sm);
                break;
            case 3:
                handleRegistrationManagement(sm);
                break;
            case 4:
                handleResultsManagement(sm);
                break;
            case 5:
                handleInfoQuery(sm);
                break;
            case 6:
                handleScheduleManagement(sm);
                break;
            case 7:
                handleBackupAndRestore(sm);
                break;
            case 8:
                handleScoringRuleManagement(sm);
                break;
            case 9:
                handleSystemSettings(sm);
                break;
            case 10:
                std::cout << "感谢使用，再见！\n";
                return 0;
            default:
                std::cout << "无效选项，请重试。\n";
        }
    }

    return 0;
}

void showMainMenu() {
    std::cout << "\n\n--- 学校运动会管理系统主菜单 ---\n";
    std::cout << "  1. 学院与运动员管理\n";
    std::cout << "  2. 赛事管理\n";
    std::cout << "  3. 报名管理\n";
    std::cout << "  4. 比赛成绩管理\n";
    std::cout << "  5. 信息查询\n";
    std::cout << "  6. 秩序册管理\n";
    std::cout << "  7. 数据备份与恢复\n";
    std::cout << "  8. 计分规则管理\n";
    std::cout << "  9. 系统设置\n";
    std::cout << "  10. 退出系统\n";
    std::cout << "---------------------------------\n";
    std::cout << "请输入选项: ";
}

void handleUnitManagement(SportsMeet& sm) {
    while (true) {
        std::cout << "\n--- 学院与运动员管理 ---\n";
        std::cout << "  1. 添加学院\n";
        std::cout << "  2. 添加运动员到学院\n";
        std::cout << "  3. 查看所有学院\n";
        std::cout << "  4. 查看所有运动员\n";
        std::cout << "  5. 返回上级菜单\n";
        std::cout << "------------------------\n";
        std::cout << "请输入选项: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: {
                std::string unitName;
                std::cout << "请输入学院名称: ";
                std::getline(std::cin, unitName);
                sm.addUnit(unitName);
                break;
            }
            case 2: {
                Unit* unit = selectUnit(sm);
                if (!unit) break;

                std::string athleteId, athleteName, genderStr;
                std::cout << "请输入运动员学号: ";
                std::getline(std::cin, athleteId);
                std::cout << "请输入运动员姓名: ";
                std::getline(std::cin, athleteName);
                std::cout << "请输入运动员性别 (\"男\"、\"女\"或\"混合\"): ";
                std::getline(std::cin, genderStr);
                
                // 将字符串转换为Gender枚举
                Gender gender = stringToGender(genderStr);
                
                sm.addAthleteToUnit(unit->name, athleteId, athleteName, gender);
                break;
            }
            case 3:
                sm.showAllUnits();
                break;
            case 4:
                sm.showAllAthletes();
                break;
            case 5:
                return;
            default:
                std::cout << "无效选项，请重试。\n";
        }
    }
}

void handleEventManagement(SportsMeet& sm) {
    while (true) {
        std::cout << "\n--- 赛事管理 ---\n";
        std::cout << "  1. 添加新项目\n";
        std::cout << "  2. 删除项目\n";
        std::cout << "  3. 查看所有项目\n";
        std::cout << "  4. 返回上级菜单\n";
        std::cout << "--------------------\n";
        std::cout << "请输入选项: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: {
                std::string name, genderStr;
                int isTimeBasedChoice;
                std::cout << "请输入项目名称: ";
                std::getline(std::cin, name);
                std::cout << "请输入性别要求 (\"男\"、\"女\"或\"混合\"): ";
                std::getline(std::cin, genderStr);
                
                // 将字符串转换为Gender枚举
                Gender gender = stringToGender(genderStr);
                
                std::cout << "是计时赛吗? (1 表示是, 0 表示否): ";
                std::cin >> isTimeBasedChoice;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                // 选择计分规则
                const auto& rules = sm.getScoringRules();
                if (rules.empty()) {
                    std::cout << "错误：系统中没有可用的计分规则。请先添加计分规则。\n";
                    break;
                }
                
                std::cout << "\n--- 请选择计分规则 ---\n";
                for (size_t i = 0; i < rules.size(); ++i) {
                    std::cout << "  " << i + 1 << ". " << rules[i].ruleName << "\n";
                }
                std::cout << "------------------------\n";
                std::cout << "请输入选项: ";
                int ruleChoice;
                std::cin >> ruleChoice;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if (ruleChoice < 1 || ruleChoice > rules.size()) {
                    std::cout << "无效的计分规则选项。\n";
                    break;
                }

                // 输入项目持续时间
                int durationMinutes;
                std::cout << "请输入项目持续时间(分钟): ";
                std::cin >> durationMinutes;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                
                if (durationMinutes <= 0) {
                    std::cout << "错误: 持续时间必须为正数。\n";
                    break;
                }

                sm.addEvent(name, gender, isTimeBasedChoice == 1, rules[ruleChoice - 1], durationMinutes);
                break;
            }
            case 2: {
                Event* event_to_delete = selectEvent(sm);
                if (event_to_delete) {
                    sm.deleteEvent(event_to_delete->name, event_to_delete->gender);
                }
                break;
            }
            case 3:
                sm.showAllEvents();
                break;
            case 4:
                return;
            default:
                std::cout << "无效选项，请重试。\n";
        }
    }
}

void handleRegistrationManagement(SportsMeet& sm) {
    while(true) {
        std::cout << "\n--- 报名管理 ---\n";
        std::cout << "  1. 为运动员报名项目\n";
        std::cout << "  2. 返回上级菜单\n";
        std::cout << "--------------------\n";
        std::cout << "请输入选项: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: {
                std::string athleteId;
                std::cout << "请输入要报名的运动员学号: ";
                std::getline(std::cin, athleteId);
                
                Athlete* athlete = sm.findAthlete(athleteId);
                if (!athlete) {
                    std::cout << "未找到该运动员。\n";
                    break;
                }
                
                Event* event = selectEvent(sm);
                if (!event) break;
                
                sm.registerAthleteForEvent(athleteId, event->name, event->gender);
                break;
            }
            case 2:
                return;
            default:
                std::cout << "无效选项，请重试。\n";
        }
    }
}

void handleResultsManagement(SportsMeet& sm) {
     while(true) {
        std::cout << "\n--- 比赛成绩管理 ---\n";
        std::cout << "  1. 录入比赛成绩\n";
        std::cout << "  2. 按项目查询成绩\n";
        std::cout << "  3. 按学院查询成绩\n";
        std::cout << "  4. 按运动员查询成绩\n";
        std::cout << "  5. 返回上级菜单\n";
        std::cout << "--------------------\n";
        std::cout << "请输入选项: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch(choice) {
            case 1: {
                Event* event = selectEvent(sm);
                if (!event) break;

                std::string athleteId;
                double performance;
                std::cout << "请输入要录入成绩的运动员学号: ";
                std::getline(std::cin, athleteId);
                std::cout << "请输入成绩 (径赛单位:秒, 田赛单位:米): ";
                std::cin >> performance;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                
                sm.recordResult(event->name, event->gender, athleteId, performance);
                break;
            }
            case 2: {
                Event* event = selectEvent(sm);
                if (event) {
                    sm.showEventDetails(event->name, event->gender);
                }
                break;
            }
            case 3: {
                Unit* unit = selectUnit(sm);
                if (unit) {
                    sm.showUnitResults(unit->name);
                }
                break;
            }
            case 4: {
                std::string athleteId;
                std::cout << "请输入要查询的运动员ID: ";
                std::getline(std::cin, athleteId);
                sm.showAthleteResults(athleteId);
                break;
            }
            case 5:
                return;
            default:
                std::cout << "无效选项，请重试。\n";
        }
    }
}

void handleInfoQuery(SportsMeet& sm) {
    while(true) {
        std::cout << "\n--- 信息查询 ---\n";
        std::cout << "  1. 查询所有单位\n";
        std::cout << "  2. 查询特定单位详情\n";
        std::cout << "  3. 查询所有项目\n";
        std::cout << "  4. 查询特定项目详情\n";
        std::cout << "  5. 返回上级菜单\n";
        std::cout << "--------------------\n";
        std::cout << "请输入选项: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch(choice) {
            case 1:
                sm.showAllUnits();
                break;
            case 2: {
                Unit* unit = selectUnit(sm);
                if (unit) {
                    sm.showUnitDetails(unit->name);
                }
                break;
            }
            case 3:
                sm.showAllEvents();
                break;
            case 4: {
                Event* event = selectEvent(sm);
                if (event) {
                    sm.showEventDetails(event->name, event->gender);
                }
                break;
            }
            case 5:
                return;
            default:
                std::cout << "无效选项，请重试。\n";
        }
    }
}

void handleScheduleManagement(SportsMeet& sm) {
    while (true) {
        std::cout << "\n--- 秩序册管理 ---\n";
        std::cout << "  1. 生成秩序册\n";
        std::cout << "  2. 查看秩序册\n";
        std::cout << "  3. 返回主菜单\n";
        std::cout << "--------------------\n";
        std::cout << "请输入选项: ";

        int choice;
        std::cin >> choice;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "无效输入，请输入数字。\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                sm.generateSchedule();
                break;
            case 2:
                sm.showSchedule();
                break;
            case 3:
                return;
            default:
                std::cout << "无效选项，请重试。\n";
        }
    }
}

void handleBackupAndRestore(SportsMeet& sm) {
    while(true) {
        std::cout << "\n--- 数据备份与恢复 ---\n";
        std::cout << "  1. 备份数据到文件\n";
        std::cout << "  2. 从文件恢复数据\n";
        std::cout << "  3. 返回上级菜单\n";
        std::cout << "-----------------------\n";
        std::cout << "请输入选项: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: {
                std::string filename;
                std::cout << "请输入备份文件名 (例如 sports_meet.dat): ";
                std::getline(std::cin, filename);
                sm.backupData(filename);
                break;
            }
            case 2: {
                std::string filename;
                std::cout << "请输入恢复文件名 (例如 sports_meet.dat): ";
                std::getline(std::cin, filename);
                sm.restoreData(filename);
                break;
            }
            case 3:
                return;
            default:
                std::cout << "无效选项，请重试。\n";
        }
    }
}

void handleScoringRuleManagement(SportsMeet& sm) {
    sm.manageScoringRules();
}

void handleSystemSettings(SportsMeet& sm) {
    while (true) {
        std::cout << "\n--- 系统设置 ---\n";
        std::cout << "  1. 修改运动员参赛项目数目限制\n";
        std::cout << "  2. 修改上午比赛时间段\n";
        std::cout << "  3. 修改下午比赛时间段\n";
        std::cout << "  4. 修改比赛天数\n";
        std::cout << "  5. 查看当前系统设置\n";
        std::cout << "  6. 返回上级菜单\n";
        std::cout << "-----------------\n";
        std::cout << "请输入选项: ";
        
        int choice;
        if(!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "无效输入，请输入数字。\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        switch (choice) {
            case 1: {
                std::cout << "当前运动员参赛项目数目限制: " << sm.getMaxEventsPerAthlete() << "\n";
                std::cout << "请输入新的运动员参赛项目数目限制: ";
                int max;
                if(!(std::cin >> max)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "无效输入，请输入数字。\n";
                    break;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                sm.setMaxEventsPerAthlete(max);
                break;
            }
            case 2: {
                std::cout << "当前上午比赛时间段: " 
                          << sm.getMorningStartHour() << ":" 
                          << std::setw(2) << std::setfill('0') << sm.getMorningStartMinute()
                          << " - " 
                          << sm.getMorningEndHour() << ":" 
                          << std::setw(2) << std::setfill('0') << sm.getMorningEndMinute() << "\n";
                
                int startHour, startMinute, endHour, endMinute;
                std::cout << "请输入新的上午开始时间（小时 分钟，用空格分隔）: ";
                if(!(std::cin >> startHour >> startMinute)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "无效输入，请输入数字。\n";
                    break;
                }
                
                std::cout << "请输入新的上午结束时间（小时 分钟，用空格分隔）: ";
                if(!(std::cin >> endHour >> endMinute)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "无效输入，请输入数字。\n";
                    break;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                
                sm.setMorningTimeWindow(startHour, startMinute, endHour, endMinute);
                break;
            }
            case 3: {
                std::cout << "当前下午比赛时间段: " 
                          << sm.getAfternoonStartHour() << ":" 
                          << std::setw(2) << std::setfill('0') << sm.getAfternoonStartMinute() 
                          << " - " 
                          << sm.getAfternoonEndHour() << ":" 
                          << std::setw(2) << std::setfill('0') << sm.getAfternoonEndMinute() << "\n";
                
                int startHour, startMinute, endHour, endMinute;
                std::cout << "请输入新的下午开始时间（小时 分钟，用空格分隔）: ";
                if(!(std::cin >> startHour >> startMinute)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "无效输入，请输入数字。\n";
                    break;
                }
                
                std::cout << "请输入新的下午结束时间（小时 分钟，用空格分隔）: ";
                if(!(std::cin >> endHour >> endMinute)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "无效输入，请输入数字。\n";
                    break;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                
                sm.setAfternoonTimeWindow(startHour, startMinute, endHour, endMinute);
                break;
            }
            case 4: {
                std::cout << "当前比赛天数: " << sm.getCompetitionDays() << " 天\n";
                std::cout << "请输入新的比赛天数: ";
                int days;
                if(!(std::cin >> days) || days < 1) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "无效输入，请输入大于0的整数。\n";
                    break;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                sm.setCompetitionDays(days);
                std::cout << "比赛天数已更新为 " << days << " 天。\n";
                break;
            }
            case 5: {
                std::cout << "\n--- 当前系统设置 ---\n";
                std::cout << "运动员参赛项目数目限制: " << sm.getMaxEventsPerAthlete() << "\n";
                std::cout << "比赛天数: " << sm.getCompetitionDays() << " 天\n";
                std::cout << "上午比赛时间段: " 
                          << sm.getMorningStartHour() << ":" 
                          << std::setw(2) << std::setfill('0') << sm.getMorningStartMinute()
                          << " - " 
                          << sm.getMorningEndHour() << ":" 
                          << std::setw(2) << std::setfill('0') << sm.getMorningEndMinute() << "\n";
                std::cout << "下午比赛时间段: " 
                          << sm.getAfternoonStartHour() << ":" 
                          << std::setw(2) << std::setfill('0') << sm.getAfternoonStartMinute() 
                          << " - " 
                          << sm.getAfternoonEndHour() << ":" 
                          << std::setw(2) << std::setfill('0') << sm.getAfternoonEndMinute() << "\n";
                break;
            }
            case 6:
                return;
            default:
                std::cout << "无效选项，请重试。\n";
        }
    }
}

Event* selectEvent(SportsMeet& sm) {
    std::cout << "\n--- 当前所有项目 ---\n";
    const auto& events = sm.getAllEvents();
    if (events.empty()) {
        std::cout << "没有项目信息。\n";
        return nullptr;
    }

    for (size_t i = 0; i < events.size(); ++i) {
        std::cout << i + 1 << ". " << events[i]->name << " (" << genderToString(events[i]->gender) << ")\n";
    }

    std::cout << "\n请输入项目对应的编号进行选择 (输入 0 取消): ";
    int choice = 0;
    while (!(std::cin >> choice) || choice < 0 || choice > static_cast<int>(events.size())) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "输入无效，请重新输入: ";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (choice == 0) {
        std::cout << "操作已取消。\n";
        return nullptr;
    }
    return events[choice - 1].get();
}

Unit* selectUnit(SportsMeet& sm) {
    std::cout << "\n--- 当前所有单位 ---\n";
    const auto& units = sm.getAllUnits();
    if (units.empty()) {
        std::cout << "没有单位信息。\n";
        return nullptr;
    }

    for (size_t i = 0; i < units.size(); ++i) {
        std::cout << i + 1 << ". " << units[i]->name << "\n";
    }

    std::cout << "\n请输入单位对应的编号进行选择 (输入 0 取消): ";
    int choice = 0;
    while (!(std::cin >> choice) || choice < 0 || choice > static_cast<int>(units.size())) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "输入无效，请重新输入: ";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (choice == 0) {
        std::cout << "操作已取消。\n";
        return nullptr;
    }
    return units[choice - 1].get();
}

void loadSampleData(SportsMeet& sm) {
    std::cout << "正在加载示例数据...\n";

    // 1. 添加单位
    sm.addUnit("计算机学院");
    sm.addUnit("外国语学院");
    sm.addUnit("机械工程学院");
    sm.addUnit("理学院");

    // 2. 添加项目 (名称, 性别, 是否为计时赛)
    const auto& rules = sm.getScoringRules();
    if (rules.empty()) {
        std::cout << "无法加载示例项目，因为没有计分规则！\n";
        return;
    }
    
    // 使用第一个可用的规则
    const ScoringRule& defaultRule = rules[0];
    
    // 已有项目，使用Gender枚举
    sm.addEvent("男子100米", Gender::MALE, true, defaultRule, 30);     // 短跑项目大约需要30分钟
    sm.addEvent("男子1500米", Gender::MALE, true, defaultRule, 45);    // 中长跑需要更多时间
    sm.addEvent("男子跳高", Gender::MALE, false, defaultRule, 60);     // 田赛项目通常需要更长时间
    sm.addEvent("男子铅球", Gender::MALE, false, defaultRule, 60);
    sm.addEvent("女子100米", Gender::FEMALE, true, defaultRule, 30);
    sm.addEvent("女子800米", Gender::FEMALE, true, defaultRule, 40);
    sm.addEvent("女子跳远", Gender::FEMALE, false, defaultRule, 60);
    sm.addEvent("女子400米", Gender::FEMALE, true, defaultRule, 35);
    
    // 添加新项目
    sm.addEvent("男子200米", Gender::MALE, true, defaultRule, 30);     // 短跑
    sm.addEvent("男子400米", Gender::MALE, true, defaultRule, 35);     // 中短跑
    sm.addEvent("男子跳远", Gender::MALE, false, defaultRule, 60);     // 田赛
    sm.addEvent("男子三级跳远", Gender::MALE, false, defaultRule, 70); // 田赛
    sm.addEvent("男子标枪", Gender::MALE, false, defaultRule, 65);     // 田赛
    sm.addEvent("男子4x100米接力", Gender::MALE, true, defaultRule, 40); // 接力赛
    
    sm.addEvent("女子200米", Gender::FEMALE, true, defaultRule, 30);     // 短跑
    sm.addEvent("女子1500米", Gender::FEMALE, true, defaultRule, 45);    // 中长跑
    sm.addEvent("女子铅球", Gender::FEMALE, false, defaultRule, 60);     // 田赛
    sm.addEvent("女子三级跳远", Gender::FEMALE, false, defaultRule, 70); // 田赛
    sm.addEvent("女子标枪", Gender::FEMALE, false, defaultRule, 65);     // 田赛
    sm.addEvent("女子4x100米接力", Gender::FEMALE, true, defaultRule, 40); // 接力赛
    
    // 添加混合项目
    sm.addEvent("混合4x400米接力", Gender::MIXED, true, defaultRule, 50);  // 混合接力赛
    sm.addEvent("混合团体跳远", Gender::MIXED, false, defaultRule, 75);    // 混合团体田赛
    sm.addEvent("混合马拉松", Gender::MIXED, true, defaultRule, 180);      // 混合长跑

    // 3. 注册运动员并报名
    // 计算机学院
    sm.addAthleteToUnit("计算机学院", "CS001", "张三", Gender::MALE);
    sm.registerAthleteForEvent("CS001", "男子100米", Gender::MALE);
    sm.registerAthleteForEvent("CS001", "男子跳高", Gender::MALE);

    sm.addAthleteToUnit("计算机学院", "CS002", "李四", Gender::MALE);
    sm.registerAthleteForEvent("CS002", "男子100米", Gender::MALE);
    sm.registerAthleteForEvent("CS002", "男子1500米", Gender::MALE);

    sm.addAthleteToUnit("计算机学院", "CS003", "王五", Gender::MALE);
    sm.registerAthleteForEvent("CS003", "男子100米", Gender::MALE);
    sm.registerAthleteForEvent("CS003", "男子铅球", Gender::MALE);

    sm.addAthleteToUnit("计算机学院", "CS004", "赵六", Gender::MALE);
    sm.registerAthleteForEvent("CS004", "男子1500米", Gender::MALE);

    sm.addAthleteToUnit("计算机学院", "CS005", "孙七", Gender::MALE);
    sm.registerAthleteForEvent("CS005", "男子1500米", Gender::MALE);

    sm.addAthleteToUnit("计算机学院", "CS006", "周八", Gender::MALE);
    sm.registerAthleteForEvent("CS006", "男子跳高", Gender::MALE);

    sm.addAthleteToUnit("计算机学院", "CS007", "吴九", Gender::MALE);
    sm.registerAthleteForEvent("CS007", "男子100米", Gender::MALE);

    sm.addAthleteToUnit("计算机学院", "CS008", "郑十", Gender::MALE);
    sm.registerAthleteForEvent("CS008", "男子100米", Gender::MALE);

    sm.addAthleteToUnit("计算机学院", "CS101", "陈一一", Gender::FEMALE);
    sm.registerAthleteForEvent("CS101", "女子100米", Gender::FEMALE);
    sm.registerAthleteForEvent("CS101", "女子800米", Gender::FEMALE);

    sm.addAthleteToUnit("计算机学院", "CS102", "钱一二", Gender::FEMALE);
    sm.registerAthleteForEvent("CS102", "女子100米", Gender::FEMALE);
    sm.registerAthleteForEvent("CS102", "女子跳远", Gender::FEMALE);

    sm.addAthleteToUnit("计算机学院", "CS103", "卫一三", Gender::FEMALE);
    sm.registerAthleteForEvent("CS103", "女子800米", Gender::FEMALE);

    sm.addAthleteToUnit("计算机学院", "CS104", "蒋一四", Gender::FEMALE);
    sm.registerAthleteForEvent("CS104", "女子400米", Gender::FEMALE);

    // 计算机学院 - 添加新的运动员及其报名项目
    sm.addAthleteToUnit("计算机学院", "CS009", "徐一五", Gender::MALE);
    sm.registerAthleteForEvent("CS009", "男子200米", Gender::MALE);
    sm.registerAthleteForEvent("CS009", "男子跳远", Gender::MALE);

    sm.addAthleteToUnit("计算机学院", "CS010", "丁一六", Gender::MALE);
    sm.registerAthleteForEvent("CS010", "男子400米", Gender::MALE);
    sm.registerAthleteForEvent("CS010", "男子4x100米接力", Gender::MALE);

    sm.addAthleteToUnit("计算机学院", "CS011", "方一七", Gender::MALE);
    sm.registerAthleteForEvent("CS011", "男子三级跳远", Gender::MALE);
    sm.registerAthleteForEvent("CS011", "男子4x100米接力", Gender::MALE);

    sm.addAthleteToUnit("计算机学院", "CS012", "倪一八", Gender::MALE);
    sm.registerAthleteForEvent("CS012", "男子标枪", Gender::MALE);
    sm.registerAthleteForEvent("CS012", "男子4x100米接力", Gender::MALE);

    sm.addAthleteToUnit("计算机学院", "CS105", "江一九", Gender::FEMALE);
    sm.registerAthleteForEvent("CS105", "女子200米", Gender::FEMALE);
    sm.registerAthleteForEvent("CS105", "女子4x100米接力", Gender::FEMALE);

    sm.addAthleteToUnit("计算机学院", "CS106", "董二十", Gender::FEMALE);
    sm.registerAthleteForEvent("CS106", "女子1500米", Gender::FEMALE);
    sm.registerAthleteForEvent("CS106", "女子4x100米接力", Gender::FEMALE);

    sm.addAthleteToUnit("计算机学院", "CS107", "程二一", Gender::FEMALE);
    sm.registerAthleteForEvent("CS107", "女子铅球", Gender::FEMALE);
    sm.registerAthleteForEvent("CS107", "女子标枪", Gender::FEMALE);

    sm.addAthleteToUnit("计算机学院", "CS108", "牛二二", Gender::FEMALE);
    sm.registerAthleteForEvent("CS108", "女子三级跳远", Gender::FEMALE);
    sm.registerAthleteForEvent("CS108", "女子4x100米接力", Gender::FEMALE);

    // 外国语学院
    sm.addAthleteToUnit("外国语学院", "FL001", "冯A", Gender::MALE);
    sm.registerAthleteForEvent("FL001", "男子100米", Gender::MALE);
    sm.registerAthleteForEvent("FL001", "男子跳高", Gender::MALE);

    sm.addAthleteToUnit("外国语学院", "FL002", "陈B", Gender::MALE);
    sm.registerAthleteForEvent("FL002", "男子1500米", Gender::MALE);
    
    sm.addAthleteToUnit("外国语学院", "FL101", "褚C", Gender::FEMALE);
    sm.registerAthleteForEvent("FL101", "女子100米", Gender::FEMALE);
    sm.registerAthleteForEvent("FL101", "女子跳远", Gender::FEMALE);

    sm.addAthleteToUnit("外国语学院", "FL102", "卫D", Gender::FEMALE);
    sm.registerAthleteForEvent("FL102", "女子800米", Gender::FEMALE);
    sm.registerAthleteForEvent("FL102", "女子跳远", Gender::FEMALE);

    sm.addAthleteToUnit("外国语学院", "FL103", "蒋E", Gender::FEMALE);
    sm.registerAthleteForEvent("FL103", "女子100米", Gender::FEMALE);

    sm.addAthleteToUnit("外国语学院", "FL104", "沈F", Gender::FEMALE);
    sm.registerAthleteForEvent("FL104", "女子800米", Gender::FEMALE);
    
    // 外国语学院 - 添加新的运动员及其报名项目
    sm.addAthleteToUnit("外国语学院", "FL003", "马T", Gender::MALE);
    sm.registerAthleteForEvent("FL003", "男子200米", Gender::MALE);
    sm.registerAthleteForEvent("FL003", "男子4x100米接力", Gender::MALE);

    sm.addAthleteToUnit("外国语学院", "FL004", "孙U", Gender::MALE);
    sm.registerAthleteForEvent("FL004", "男子400米", Gender::MALE);
    sm.registerAthleteForEvent("FL004", "男子4x100米接力", Gender::MALE);

    sm.addAthleteToUnit("外国语学院", "FL005", "黄V", Gender::MALE);
    sm.registerAthleteForEvent("FL005", "男子跳远", Gender::MALE);
    sm.registerAthleteForEvent("FL005", "男子三级跳远", Gender::MALE);

    sm.addAthleteToUnit("外国语学院", "FL006", "石W", Gender::MALE);
    sm.registerAthleteForEvent("FL006", "男子标枪", Gender::MALE);
    sm.registerAthleteForEvent("FL006", "男子4x100米接力", Gender::MALE);

    sm.addAthleteToUnit("外国语学院", "FL105", "章X", Gender::FEMALE);
    sm.registerAthleteForEvent("FL105", "女子200米", Gender::FEMALE);
    sm.registerAthleteForEvent("FL105", "女子4x100米接力", Gender::FEMALE);

    sm.addAthleteToUnit("外国语学院", "FL106", "郝Y", Gender::FEMALE);
    sm.registerAthleteForEvent("FL106", "女子1500米", Gender::FEMALE);
    sm.registerAthleteForEvent("FL106", "女子铅球", Gender::FEMALE);

    sm.addAthleteToUnit("外国语学院", "FL107", "邓Z", Gender::FEMALE);
    sm.registerAthleteForEvent("FL107", "女子三级跳远", Gender::FEMALE);
    sm.registerAthleteForEvent("FL107", "女子4x100米接力", Gender::FEMALE);

    sm.addAthleteToUnit("外国语学院", "FL108", "袁AA", Gender::FEMALE);
    sm.registerAthleteForEvent("FL108", "女子标枪", Gender::FEMALE);
    sm.registerAthleteForEvent("FL108", "女子4x100米接力", Gender::FEMALE);

    // 机械工程学院
    sm.addAthleteToUnit("机械工程学院", "ME001", "韩G", Gender::MALE);
    sm.registerAthleteForEvent("ME001", "男子1500米", Gender::MALE);
    sm.registerAthleteForEvent("ME001", "男子铅球", Gender::MALE);

    sm.addAthleteToUnit("机械工程学院", "ME002", "杨H", Gender::MALE);
    sm.registerAthleteForEvent("ME002", "男子铅球", Gender::MALE);

    sm.addAthleteToUnit("机械工程学院", "ME003", "朱I", Gender::MALE);
    sm.registerAthleteForEvent("ME003", "男子铅球", Gender::MALE);

    sm.addAthleteToUnit("机械工程学院", "ME004", "秦J", Gender::MALE);
    sm.registerAthleteForEvent("ME004", "男子铅球", Gender::MALE);

    sm.addAthleteToUnit("机械工程学院", "ME005", "许AA", Gender::MALE);
    sm.registerAthleteForEvent("ME005", "男子100米", Gender::MALE);

    sm.addAthleteToUnit("机械工程学院", "ME101", "尤K", Gender::FEMALE);
    sm.registerAthleteForEvent("ME101", "女子跳远", Gender::FEMALE);

    sm.addAthleteToUnit("机械工程学院", "ME102", "许L", Gender::FEMALE);
    sm.registerAthleteForEvent("ME102", "女子跳远", Gender::FEMALE);
    
    // 机械工程学院 - 添加新的运动员及其报名项目
    sm.addAthleteToUnit("机械工程学院", "ME006", "高BB", Gender::MALE);
    sm.registerAthleteForEvent("ME006", "男子200米", Gender::MALE);
    sm.registerAthleteForEvent("ME006", "男子4x100米接力", Gender::MALE);

    sm.addAthleteToUnit("机械工程学院", "ME007", "谢CC", Gender::MALE);
    sm.registerAthleteForEvent("ME007", "男子400米", Gender::MALE);
    sm.registerAthleteForEvent("ME007", "男子4x100米接力", Gender::MALE);

    sm.addAthleteToUnit("机械工程学院", "ME008", "赖DD", Gender::MALE);
    sm.registerAthleteForEvent("ME008", "男子跳远", Gender::MALE);
    sm.registerAthleteForEvent("ME008", "男子4x100米接力", Gender::MALE);

    sm.addAthleteToUnit("机械工程学院", "ME009", "曾EE", Gender::MALE);
    sm.registerAthleteForEvent("ME009", "男子三级跳远", Gender::MALE);
    sm.registerAthleteForEvent("ME009", "男子标枪", Gender::MALE);

    sm.addAthleteToUnit("机械工程学院", "ME103", "毛FF", Gender::FEMALE);
    sm.registerAthleteForEvent("ME103", "女子200米", Gender::FEMALE);
    sm.registerAthleteForEvent("ME103", "女子4x100米接力", Gender::FEMALE);

    sm.addAthleteToUnit("机械工程学院", "ME104", "韦GG", Gender::FEMALE);
    sm.registerAthleteForEvent("ME104", "女子1500米", Gender::FEMALE);
    sm.registerAthleteForEvent("ME104", "女子4x100米接力", Gender::FEMALE);

    sm.addAthleteToUnit("机械工程学院", "ME105", "傅HH", Gender::FEMALE);
    sm.registerAthleteForEvent("ME105", "女子铅球", Gender::FEMALE);
    sm.registerAthleteForEvent("ME105", "女子标枪", Gender::FEMALE);

    sm.addAthleteToUnit("机械工程学院", "ME106", "常II", Gender::FEMALE);
    sm.registerAthleteForEvent("ME106", "女子三级跳远", Gender::FEMALE);
    sm.registerAthleteForEvent("ME106", "女子4x100米接力", Gender::FEMALE);

    // 理学院
    sm.addAthleteToUnit("理学院", "SCI001", "何M", Gender::MALE);
    sm.registerAthleteForEvent("SCI001", "男子跳高", Gender::MALE);
    
    sm.addAthleteToUnit("理学院", "SCI002", "吕N", Gender::MALE);
    sm.registerAthleteForEvent("SCI002", "男子跳高", Gender::MALE);

    sm.addAthleteToUnit("理学院", "SCI003", "施O", Gender::MALE);
    sm.registerAthleteForEvent("SCI003", "男子跳高", Gender::MALE);

    sm.addAthleteToUnit("理学院", "SCI004", "施BB", Gender::MALE);
    sm.registerAthleteForEvent("SCI004", "男子100米", Gender::MALE);
    
    sm.addAthleteToUnit("理学院", "SCI101", "张P", Gender::FEMALE);
    sm.registerAthleteForEvent("SCI101", "女子800米", Gender::FEMALE);
    
    sm.addAthleteToUnit("理学院", "SCI102", "孔Q", Gender::FEMALE);
    sm.registerAthleteForEvent("SCI102", "女子800米", Gender::FEMALE);
    
    sm.addAthleteToUnit("理学院", "SCI103", "曹R", Gender::FEMALE);
    sm.registerAthleteForEvent("SCI103", "女子400米", Gender::FEMALE);
    
    sm.addAthleteToUnit("理学院", "SCI104", "严S", Gender::FEMALE);
    sm.registerAthleteForEvent("SCI104", "女子400米", Gender::FEMALE);
    
    // 理学院 - 添加新的运动员及其报名项目
    sm.addAthleteToUnit("理学院", "SCI005", "乐JJ", Gender::MALE);
    sm.registerAthleteForEvent("SCI005", "男子200米", Gender::MALE);
    sm.registerAthleteForEvent("SCI005", "男子4x100米接力", Gender::MALE);

    sm.addAthleteToUnit("理学院", "SCI006", "熊KK", Gender::MALE);
    sm.registerAthleteForEvent("SCI006", "男子400米", Gender::MALE);
    sm.registerAthleteForEvent("SCI006", "男子4x100米接力", Gender::MALE);

    sm.addAthleteToUnit("理学院", "SCI007", "贺LL", Gender::MALE);
    sm.registerAthleteForEvent("SCI007", "男子跳远", Gender::MALE);
    sm.registerAthleteForEvent("SCI007", "男子三级跳远", Gender::MALE);

    sm.addAthleteToUnit("理学院", "SCI008", "龙MM", Gender::MALE);
    sm.registerAthleteForEvent("SCI008", "男子标枪", Gender::MALE);
    sm.registerAthleteForEvent("SCI008", "男子4x100米接力", Gender::MALE);

    sm.addAthleteToUnit("理学院", "SCI105", "阮NN", Gender::FEMALE);
    sm.registerAthleteForEvent("SCI105", "女子200米", Gender::FEMALE);
    sm.registerAthleteForEvent("SCI105", "女子4x100米接力", Gender::FEMALE);

    sm.addAthleteToUnit("理学院", "SCI106", "雷OO", Gender::FEMALE);
    sm.registerAthleteForEvent("SCI106", "女子1500米", Gender::FEMALE);
    sm.registerAthleteForEvent("SCI106", "女子4x100米接力", Gender::FEMALE);

    sm.addAthleteToUnit("理学院", "SCI107", "夏PP", Gender::FEMALE);
    sm.registerAthleteForEvent("SCI107", "女子铅球", Gender::FEMALE);
    sm.registerAthleteForEvent("SCI107", "女子标枪", Gender::FEMALE);

    sm.addAthleteToUnit("理学院", "SCI108", "侯QQ", Gender::FEMALE);
    sm.registerAthleteForEvent("SCI108", "女子三级跳远", Gender::FEMALE);
    sm.registerAthleteForEvent("SCI108", "女子4x100米接力", Gender::FEMALE);

    // 混合项目报名 - 计算机学院
    sm.registerAthleteForEvent("CS002", "混合4x400米接力", Gender::MIXED); // 男生
    sm.registerAthleteForEvent("CS010", "混合4x400米接力", Gender::MIXED); // 男生
    sm.registerAthleteForEvent("CS101", "混合4x400米接力", Gender::MIXED); // 女生
    sm.registerAthleteForEvent("CS105", "混合4x400米接力", Gender::MIXED); // 女生
    
    sm.registerAthleteForEvent("CS006", "混合团体跳远", Gender::MIXED); // 男生
    sm.registerAthleteForEvent("CS009", "混合团体跳远", Gender::MIXED); // 男生
    sm.registerAthleteForEvent("CS102", "混合团体跳远", Gender::MIXED); // 女生
    sm.registerAthleteForEvent("CS108", "混合团体跳远", Gender::MIXED); // 女生
    
    sm.registerAthleteForEvent("CS004", "混合马拉松", Gender::MIXED); // 男生
    sm.registerAthleteForEvent("CS103", "混合马拉松", Gender::MIXED); // 女生
    
    // 混合项目报名 - 外国语学院
    sm.registerAthleteForEvent("FL002", "混合4x400米接力", Gender::MIXED); // 男生
    sm.registerAthleteForEvent("FL004", "混合4x400米接力", Gender::MIXED); // 男生
    sm.registerAthleteForEvent("FL101", "混合4x400米接力", Gender::MIXED); // 女生
    sm.registerAthleteForEvent("FL105", "混合4x400米接力", Gender::MIXED); // 女生
    
    sm.registerAthleteForEvent("FL001", "混合团体跳远", Gender::MIXED); // 男生
    sm.registerAthleteForEvent("FL005", "混合团体跳远", Gender::MIXED); // 男生
    sm.registerAthleteForEvent("FL101", "混合团体跳远", Gender::MIXED); // 女生
    sm.registerAthleteForEvent("FL107", "混合团体跳远", Gender::MIXED); // 女生
    
    sm.registerAthleteForEvent("FL003", "混合马拉松", Gender::MIXED); // 男生
    sm.registerAthleteForEvent("FL106", "混合马拉松", Gender::MIXED); // 女生
    
    // 混合项目报名 - 机械工程学院
    sm.registerAthleteForEvent("ME006", "混合4x400米接力", Gender::MIXED); // 男生
    sm.registerAthleteForEvent("ME007", "混合4x400米接力", Gender::MIXED); // 男生
    sm.registerAthleteForEvent("ME103", "混合4x400米接力", Gender::MIXED); // 女生
    sm.registerAthleteForEvent("ME104", "混合4x400米接力", Gender::MIXED); // 女生
    
    sm.registerAthleteForEvent("ME008", "混合团体跳远", Gender::MIXED); // 男生
    sm.registerAthleteForEvent("ME009", "混合团体跳远", Gender::MIXED); // 男生
    sm.registerAthleteForEvent("ME101", "混合团体跳远", Gender::MIXED); // 女生
    sm.registerAthleteForEvent("ME106", "混合团体跳远", Gender::MIXED); // 女生
    
    sm.registerAthleteForEvent("ME001", "混合马拉松", Gender::MIXED); // 男生
    sm.registerAthleteForEvent("ME104", "混合马拉松", Gender::MIXED); // 女生
    
    // 混合项目报名 - 理学院
    sm.registerAthleteForEvent("SCI005", "混合4x400米接力", Gender::MIXED); // 男生
    sm.registerAthleteForEvent("SCI006", "混合4x400米接力", Gender::MIXED); // 男生
    sm.registerAthleteForEvent("SCI105", "混合4x400米接力", Gender::MIXED); // 女生
    sm.registerAthleteForEvent("SCI106", "混合4x400米接力", Gender::MIXED); // 女生
    
    sm.registerAthleteForEvent("SCI001", "混合团体跳远", Gender::MIXED); // 男生
    sm.registerAthleteForEvent("SCI007", "混合团体跳远", Gender::MIXED); // 男生
    sm.registerAthleteForEvent("SCI107", "混合团体跳远", Gender::MIXED); // 女生
    sm.registerAthleteForEvent("SCI108", "混合团体跳远", Gender::MIXED); // 女生
    
    sm.registerAthleteForEvent("SCI004", "混合马拉松", Gender::MIXED); // 男生
    sm.registerAthleteForEvent("SCI101", "混合马拉松", Gender::MIXED); // 女生

    // 4. 为所有项目记录成绩并计分
    std::cout << "\n正在录入和处理成绩...\n";

    // --- 男子100米 ---
    sm.recordResult("男子100米", Gender::MALE, "CS001", 11.2);
    sm.recordResult("男子100米", Gender::MALE, "CS002", 11.5);
    sm.recordResult("男子100米", Gender::MALE, "CS003", 11.1);
    sm.recordResult("男子100米", Gender::MALE, "CS007", 11.8);
    sm.recordResult("男子100米", Gender::MALE, "CS008", 11.3);
    sm.recordResult("男子100米", Gender::MALE, "FL001", 11.0);
    sm.recordResult("男子100米", Gender::MALE, "ME005", 11.4);
    sm.recordResult("男子100米", Gender::MALE, "SCI004", 11.6);
    sm.processScoresForEvent("男子100米", Gender::MALE);

    // --- 男子1500米 ---
    sm.recordResult("男子1500米", Gender::MALE, "CS002", 245.2); // 4:05.2
    sm.recordResult("男子1500米", Gender::MALE, "CS004", 250.1); // 4:10.1
    sm.recordResult("男子1500米", Gender::MALE, "CS005", 243.8); // 4:03.8
    sm.recordResult("男子1500米", Gender::MALE, "FL002", 248.5); // 4:08.5
    sm.recordResult("男子1500米", Gender::MALE, "ME001", 240.5); // 4:00.5
    sm.processScoresForEvent("男子1500米", Gender::MALE);

    // --- 男子跳高 ---
    sm.recordResult("男子跳高", Gender::MALE, "CS001", 1.95);
    sm.recordResult("男子跳高", Gender::MALE, "CS006", 1.98);
    sm.recordResult("男子跳高", Gender::MALE, "FL001", 2.01);
    sm.recordResult("男子跳高", Gender::MALE, "SCI001", 1.92);
    sm.recordResult("男子跳高", Gender::MALE, "SCI002", 1.85);
    sm.recordResult("男子跳高", Gender::MALE, "SCI003", 1.88);
    sm.processScoresForEvent("男子跳高", Gender::MALE);

    // --- 男子铅球 ---
    sm.recordResult("男子铅球", Gender::MALE, "CS003", 12.5);
    sm.recordResult("男子铅球", Gender::MALE, "ME001", 13.1);
    sm.recordResult("男子铅球", Gender::MALE, "ME002", 13.5);
    sm.recordResult("男子铅球", Gender::MALE, "ME003", 12.8);
    sm.recordResult("男子铅球", Gender::MALE, "ME004", 14.0);
    sm.processScoresForEvent("男子铅球", Gender::MALE);

    // --- 女子100米 ---
    sm.recordResult("女子100米", Gender::FEMALE, "CS101", 12.5);
    sm.recordResult("女子100米", Gender::FEMALE, "CS102", 12.8);
    sm.recordResult("女子100米", Gender::FEMALE, "FL101", 12.3);
    sm.recordResult("女子100米", Gender::FEMALE, "FL103", 12.6);
    sm.processScoresForEvent("女子100米", Gender::FEMALE);

    // --- 女子800米 ---
    sm.recordResult("女子800米", Gender::FEMALE, "CS101", 150.5); // 2:30.5
    sm.recordResult("女子800米", Gender::FEMALE, "CS103", 148.2); // 2:28.2
    sm.recordResult("女子800米", Gender::FEMALE, "FL102", 152.1); // 2:32.1
    sm.recordResult("女子800米", Gender::FEMALE, "FL104", 147.9); // 2:27.9
    sm.recordResult("女子800米", Gender::FEMALE, "SCI101", 155.0); // 2:35.0
    sm.recordResult("女子800米", Gender::FEMALE, "SCI102", 153.3); // 2:33.3
    sm.processScoresForEvent("女子800米", Gender::FEMALE);

    // --- 女子跳远 ---
    sm.recordResult("女子跳远", Gender::FEMALE, "CS102", 5.80);
    sm.recordResult("女子跳远", Gender::FEMALE, "FL101", 6.01);
    sm.recordResult("女子跳远", Gender::FEMALE, "FL102", 5.95);
    sm.recordResult("女子跳远", Gender::FEMALE, "ME101", 5.77);
    sm.recordResult("女子跳远", Gender::FEMALE, "ME102", 5.89);
    sm.processScoresForEvent("女子跳远", Gender::FEMALE);

    // --- 女子400米 ---
    sm.recordResult("女子400米", Gender::FEMALE, "CS104", 62.5);
    sm.recordResult("女子400米", Gender::FEMALE, "SCI103", 61.2);
    sm.recordResult("女子400米", Gender::FEMALE, "SCI104", 60.9);
    sm.processScoresForEvent("女子400米", Gender::FEMALE);
    
    // --- 新增项目成绩 ---
    
    // --- 男子200米 ---
    sm.recordResult("男子200米", Gender::MALE, "CS009", 22.3);
    sm.recordResult("男子200米", Gender::MALE, "FL003", 22.1);
    sm.recordResult("男子200米", Gender::MALE, "ME006", 22.5);
    sm.recordResult("男子200米", Gender::MALE, "SCI005", 22.0);
    sm.processScoresForEvent("男子200米", Gender::MALE);
    
    // --- 男子400米 ---
    sm.recordResult("男子400米", Gender::MALE, "CS010", 50.2);
    sm.recordResult("男子400米", Gender::MALE, "FL004", 49.8);
    sm.recordResult("男子400米", Gender::MALE, "ME007", 50.5);
    sm.recordResult("男子400米", Gender::MALE, "SCI006", 49.4);
    sm.processScoresForEvent("男子400米", Gender::MALE);
    
    // --- 男子跳远 ---
    sm.recordResult("男子跳远", Gender::MALE, "CS009", 7.10);
    sm.recordResult("男子跳远", Gender::MALE, "FL005", 7.25);
    sm.recordResult("男子跳远", Gender::MALE, "ME008", 7.15);
    sm.recordResult("男子跳远", Gender::MALE, "SCI007", 7.20);
    sm.processScoresForEvent("男子跳远", Gender::MALE);
    
    // --- 男子三级跳远 ---
    sm.recordResult("男子三级跳远", Gender::MALE, "CS011", 15.30);
    sm.recordResult("男子三级跳远", Gender::MALE, "FL005", 15.45);
    sm.recordResult("男子三级跳远", Gender::MALE, "ME009", 15.20);
    sm.recordResult("男子三级跳远", Gender::MALE, "SCI007", 15.35);
    sm.processScoresForEvent("男子三级跳远", Gender::MALE);
    
    // --- 男子标枪 ---
    sm.recordResult("男子标枪", Gender::MALE, "CS012", 65.8);
    sm.recordResult("男子标枪", Gender::MALE, "FL006", 67.2);
    sm.recordResult("男子标枪", Gender::MALE, "ME009", 64.5);
    sm.recordResult("男子标枪", Gender::MALE, "SCI008", 66.3);
    sm.processScoresForEvent("男子标枪", Gender::MALE);
    
    // --- 男子4x100米接力 ---
    sm.recordResult("男子4x100米接力", Gender::MALE, "CS010", 42.3);  // 使用一个队员ID代表整个团队
    sm.recordResult("男子4x100米接力", Gender::MALE, "FL003", 42.1);
    sm.recordResult("男子4x100米接力", Gender::MALE, "ME006", 42.7);
    sm.recordResult("男子4x100米接力", Gender::MALE, "SCI005", 41.9);
    sm.processScoresForEvent("男子4x100米接力", Gender::MALE);
    
    // --- 女子200米 ---
    sm.recordResult("女子200米", Gender::FEMALE, "CS105", 24.8);
    sm.recordResult("女子200米", Gender::FEMALE, "FL105", 24.5);
    sm.recordResult("女子200米", Gender::FEMALE, "ME103", 24.9);
    sm.recordResult("女子200米", Gender::FEMALE, "SCI105", 24.4);
    sm.processScoresForEvent("女子200米", Gender::FEMALE);
    
    // --- 女子1500米 ---
    sm.recordResult("女子1500米", Gender::FEMALE, "CS106", 285.3);  // 4:45.3
    sm.recordResult("女子1500米", Gender::FEMALE, "FL106", 283.1);  // 4:43.1
    sm.recordResult("女子1500米", Gender::FEMALE, "ME104", 284.5);  // 4:44.5
    sm.recordResult("女子1500米", Gender::FEMALE, "SCI106", 282.8); // 4:42.8
    sm.processScoresForEvent("女子1500米", Gender::FEMALE);
    
    // --- 女子铅球 ---
    sm.recordResult("女子铅球", Gender::FEMALE, "CS107", 14.2);
    sm.recordResult("女子铅球", Gender::FEMALE, "FL106", 14.5);
    sm.recordResult("女子铅球", Gender::FEMALE, "ME105", 14.8);
    sm.recordResult("女子铅球", Gender::FEMALE, "SCI107", 13.9);
    sm.processScoresForEvent("女子铅球", Gender::FEMALE);
    
    // --- 女子三级跳远 ---
    sm.recordResult("女子三级跳远", Gender::FEMALE, "CS108", 13.5);
    sm.recordResult("女子三级跳远", Gender::FEMALE, "FL107", 13.8);
    sm.recordResult("女子三级跳远", Gender::FEMALE, "ME106", 13.3);
    sm.recordResult("女子三级跳远", Gender::FEMALE, "SCI108", 13.6);
    sm.processScoresForEvent("女子三级跳远", Gender::FEMALE);
    
    // --- 女子标枪 ---
    sm.recordResult("女子标枪", Gender::FEMALE, "CS107", 52.3);
    sm.recordResult("女子标枪", Gender::FEMALE, "FL108", 53.1);
    sm.recordResult("女子标枪", Gender::FEMALE, "ME105", 51.8);
    sm.recordResult("女子标枪", Gender::FEMALE, "SCI107", 52.7);
    sm.processScoresForEvent("女子标枪", Gender::FEMALE);
    
    // --- 女子4x100米接力 ---
    sm.recordResult("女子4x100米接力", Gender::FEMALE, "CS105", 48.2);  // 使用一个队员ID代表整个团队
    sm.recordResult("女子4x100米接力", Gender::FEMALE, "FL105", 47.8);
    sm.recordResult("女子4x100米接力", Gender::FEMALE, "ME103", 48.5);
    sm.recordResult("女子4x100米接力", Gender::FEMALE, "SCI105", 48.0);
    sm.processScoresForEvent("女子4x100米接力", Gender::FEMALE);
    
    // --- 混合项目成绩 ---
    
    // --- 混合4x400米接力 ---
    // 使用其中一名队员的ID来代表整个团队
    sm.recordResult("混合4x400米接力", Gender::MIXED, "CS002", 215.8);  // 3:35.8
    sm.recordResult("混合4x400米接力", Gender::MIXED, "FL002", 213.5);  // 3:33.5
    sm.recordResult("混合4x400米接力", Gender::MIXED, "ME006", 214.2);  // 3:34.2
    sm.recordResult("混合4x400米接力", Gender::MIXED, "SCI005", 212.9); // 3:32.9
    sm.processScoresForEvent("混合4x400米接力", Gender::MIXED);
    
    // --- 混合团体跳远 ---
    // 使用团队平均成绩
    sm.recordResult("混合团体跳远", Gender::MIXED, "CS006", 6.55);  // 团队平均跳远成绩
    sm.recordResult("混合团体跳远", Gender::MIXED, "FL001", 6.70);
    sm.recordResult("混合团体跳远", Gender::MIXED, "ME008", 6.45);
    sm.recordResult("混合团体跳远", Gender::MIXED, "SCI001", 6.60);
    sm.processScoresForEvent("混合团体跳远", Gender::MIXED);
    
    // --- 混合马拉松 ---
    sm.recordResult("混合马拉松", Gender::MIXED, "CS004", 9000.5);  // 2:30:00.5
    sm.recordResult("混合马拉松", Gender::MIXED, "FL003", 9250.3);  // 2:34:10.3
    sm.recordResult("混合马拉松", Gender::MIXED, "ME001", 9180.7);  // 2:33:00.7
    sm.recordResult("混合马拉松", Gender::MIXED, "SCI004", 9120.2); // 2:32:00.2
    sm.recordResult("混合马拉松", Gender::MIXED, "CS103", 9450.8);  // 2:37:30.8
    sm.recordResult("混合马拉松", Gender::MIXED, "FL106", 9540.1);  // 2:39:00.1
    sm.recordResult("混合马拉松", Gender::MIXED, "ME104", 9380.4);  // 2:36:20.4
    sm.recordResult("混合马拉松", Gender::MIXED, "SCI101", 9600.6); // 2:40:00.6
    sm.processScoresForEvent("混合马拉松", Gender::MIXED);
    
    std::cout << "\n示例数据加载完毕。\n";
}