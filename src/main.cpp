#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include "SportsMeet.h"
#include "Unit.h"
#include "Event.h"
#include "Athlete.h"

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
                std::cout << "该功能待实现。\n";
                break;
            case 7:
                 std::cout << "该功能待实现。\n";
                break;
            case 8:
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
    std::cout << "  6. 秩序册管理 (待实现)\n";
    std::cout << "  7. 数据备份与恢复 (待实现)\n";
    std::cout << "  8. 退出系统\n";
    std::cout << "---------------------------------\n";
    std::cout << "请输入选项: ";
}

void handleUnitManagement(SportsMeet& sm) {
    while (true) {
        std::cout << "\n--- 学院与运动员管理 ---\n";
        std::cout << "  1. 添加学院\n";
        std::cout << "  2. 添加运动员到学院\n";
        std::cout << "  3. 查看所有学院\n";
        std::cout << "  4. 返回上级菜单\n";
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

                std::string athleteId, athleteName, gender;
                std::cout << "请输入运动员学号: ";
                std::getline(std::cin, athleteId);
                std::cout << "请输入运动员姓名: ";
                std::getline(std::cin, athleteName);
                std::cout << "请输入运动员性别 (\"男\"或\"女\"): ";
                std::getline(std::cin, gender);
                sm.addAthleteToUnit(unit->name, athleteId, athleteName, gender);
                break;
            }
            case 3:
                sm.showAllUnits();
                break;
            case 4:
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
                std::string name, gender;
                int isTimeBasedChoice;
                std::cout << "请输入项目名称: ";
                std::getline(std::cin, name);
                std::cout << "请输入性别要求 (\"男\"或\"女\"): ";
                std::getline(std::cin, gender);
                std::cout << "是计时赛吗? (1 表示是, 0 表示否): ";
                std::cin >> isTimeBasedChoice;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                sm.addEvent(name, gender, isTimeBasedChoice == 1);
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

Event* selectEvent(SportsMeet& sm) {
    std::cout << "\n--- 当前所有项目 ---\n";
    const auto& events = sm.getAllEvents();
    if (events.empty()) {
        std::cout << "没有项目信息。\n";
        return nullptr;
    }

    for (size_t i = 0; i < events.size(); ++i) {
        std::cout << i + 1 << ". " << events[i]->name << " (" << events[i]->gender << ")\n";
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
    sm.addUnit("计算机学院");
    sm.addUnit("外国语学院");
    sm.addUnit("机械工程学院");

    sm.addAthleteToUnit("计算机学院", "20220101", "张三", "男");
    sm.addAthleteToUnit("计算机学院", "20220102", "李四", "男");
    sm.addAthleteToUnit("计算机学院", "20220103", "王五", "女");
    sm.addAthleteToUnit("外国语学院", "20220201", "赵六", "女");
    sm.addAthleteToUnit("外国语学院", "20220202", "孙七", "女");
    sm.addAthleteToUnit("机械工程学院", "20220301", "周八", "男");

    sm.addEvent("男子100米", "男", true);
    sm.addEvent("女子800米", "女", true);
    sm.addEvent("男子跳远", "男", false);
    sm.addEvent("女子铅球", "女", false);

    sm.registerAthleteForEvent("20220101", "男子100米", "男");
    sm.registerAthleteForEvent("20220101", "男子跳远", "男");
    sm.registerAthleteForEvent("20220102", "男子100米", "男");
    sm.registerAthleteForEvent("20220301", "男子跳远", "男");
    sm.registerAthleteForEvent("20220103", "女子800米", "女");
    sm.registerAthleteForEvent("20220201", "女子800米", "女");
    sm.registerAthleteForEvent("20220202", "女子铅球", "女");

    std::cout << "示例数据加载完毕。\n";
}