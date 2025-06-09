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
void handleScheduleManagement(SportsMeet& sm);
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
    std::cout << "  6. 秩序册管理\n";
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

    // 1. 添加单位
    sm.addUnit("计算机学院");
    sm.addUnit("外国语学院");
    sm.addUnit("机械工程学院");
    sm.addUnit("理学院");

    // 2. 添加项目 (名称, 性别, 是否为计时赛)
    sm.addEvent("男子100米", "男", true);
    sm.addEvent("男子1500米", "男", true);
    sm.addEvent("男子跳高", "男", false);
    sm.addEvent("男子铅球", "男", false);
    sm.addEvent("女子100米", "女", true);
    sm.addEvent("女子800米", "女", true);
    sm.addEvent("女子跳远", "女", false);
    sm.addEvent("女子400米", "女", true);

    // 3. 注册运动员并报名
    // 计算机学院
    sm.addAthleteToUnit("计算机学院", "CS001", "张三", "男");
    sm.registerAthleteForEvent("CS001", "男子100米", "男");
    sm.registerAthleteForEvent("CS001", "男子跳高", "男");

    sm.addAthleteToUnit("计算机学院", "CS002", "李四", "男");
    sm.registerAthleteForEvent("CS002", "男子100米", "男");
    sm.registerAthleteForEvent("CS002", "男子1500米", "男");

    sm.addAthleteToUnit("计算机学院", "CS003", "王五", "男");
    sm.registerAthleteForEvent("CS003", "男子100米", "男");
    sm.registerAthleteForEvent("CS003", "男子铅球", "男");

    sm.addAthleteToUnit("计算机学院", "CS004", "赵六", "男");
    sm.registerAthleteForEvent("CS004", "男子1500米", "男");

    sm.addAthleteToUnit("计算机学院", "CS005", "孙七", "男");
    sm.registerAthleteForEvent("CS005", "男子1500米", "男");

    sm.addAthleteToUnit("计算机学院", "CS006", "周八", "男");
    sm.registerAthleteForEvent("CS006", "男子跳高", "男");

    sm.addAthleteToUnit("计算机学院", "CS007", "吴九", "男");
    sm.registerAthleteForEvent("CS007", "男子100米", "男");

    sm.addAthleteToUnit("计算机学院", "CS008", "郑十", "男");
    sm.registerAthleteForEvent("CS008", "男子100米", "男");

    sm.addAthleteToUnit("计算机学院", "CS101", "陈一一", "女");
    sm.registerAthleteForEvent("CS101", "女子100米", "女");
    sm.registerAthleteForEvent("CS101", "女子800米", "女");

    sm.addAthleteToUnit("计算机学院", "CS102", "钱一二", "女");
    sm.registerAthleteForEvent("CS102", "女子100米", "女");
    sm.registerAthleteForEvent("CS102", "女子跳远", "女");

    sm.addAthleteToUnit("计算机学院", "CS103", "卫一三", "女");
    sm.registerAthleteForEvent("CS103", "女子800米", "女");

    sm.addAthleteToUnit("计算机学院", "CS104", "蒋一四", "女");
    sm.registerAthleteForEvent("CS104", "女子400米", "女");

    // 外国语学院
    sm.addAthleteToUnit("外国语学院", "FL001", "冯A", "男");
    sm.registerAthleteForEvent("FL001", "男子100米", "男");
    sm.registerAthleteForEvent("FL001", "男子跳高", "男");

    sm.addAthleteToUnit("外国语学院", "FL002", "陈B", "男");
    sm.registerAthleteForEvent("FL002", "男子1500米", "男");
    
    sm.addAthleteToUnit("外国语学院", "FL101", "褚C", "女");
    sm.registerAthleteForEvent("FL101", "女子100米", "女");
    sm.registerAthleteForEvent("FL101", "女子跳远", "女");

    sm.addAthleteToUnit("外国语学院", "FL102", "卫D", "女");
    sm.registerAthleteForEvent("FL102", "女子800米", "女");
    sm.registerAthleteForEvent("FL102", "女子跳远", "女");

    sm.addAthleteToUnit("外国语学院", "FL103", "蒋E", "女");
    sm.registerAthleteForEvent("FL103", "女子100米", "女");

    sm.addAthleteToUnit("外国语学院", "FL104", "沈F", "女");
    sm.registerAthleteForEvent("FL104", "女子800米", "女");
    
    // 机械工程学院
    sm.addAthleteToUnit("机械工程学院", "ME001", "韩G", "男");
    sm.registerAthleteForEvent("ME001", "男子1500米", "男");
    sm.registerAthleteForEvent("ME001", "男子铅球", "男");

    sm.addAthleteToUnit("机械工程学院", "ME002", "杨H", "男");
    sm.registerAthleteForEvent("ME002", "男子铅球", "男");

    sm.addAthleteToUnit("机械工程学院", "ME003", "朱I", "男");
    sm.registerAthleteForEvent("ME003", "男子铅球", "男");

    sm.addAthleteToUnit("机械工程学院", "ME004", "秦J", "男");
    sm.registerAthleteForEvent("ME004", "男子铅球", "男");

    sm.addAthleteToUnit("机械工程学院", "ME005", "许AA", "男");
    sm.registerAthleteForEvent("ME005", "男子100米", "男");

    sm.addAthleteToUnit("机械工程学院", "ME101", "尤K", "女");
    sm.registerAthleteForEvent("ME101", "女子跳远", "女");

    sm.addAthleteToUnit("机械工程学院", "ME102", "许L", "女");
    sm.registerAthleteForEvent("ME102", "女子跳远", "女");
    
    // 理学院
    sm.addAthleteToUnit("理学院", "SCI001", "何M", "男");
    sm.registerAthleteForEvent("SCI001", "男子跳高", "男");
    
    sm.addAthleteToUnit("理学院", "SCI002", "吕N", "男");
    sm.registerAthleteForEvent("SCI002", "男子跳高", "男");

    sm.addAthleteToUnit("理学院", "SCI003", "施O", "男");
    sm.registerAthleteForEvent("SCI003", "男子跳高", "男");

    sm.addAthleteToUnit("理学院", "SCI004", "施BB", "男");
    sm.registerAthleteForEvent("SCI004", "男子100米", "男");
    
    sm.addAthleteToUnit("理学院", "SCI101", "张P", "女");
    sm.registerAthleteForEvent("SCI101", "女子800米", "女");
    
    sm.addAthleteToUnit("理学院", "SCI102", "孔Q", "女");
    sm.registerAthleteForEvent("SCI102", "女子800米", "女");
    
    sm.addAthleteToUnit("理学院", "SCI103", "曹R", "女");
    sm.registerAthleteForEvent("SCI103", "女子400米", "女");
    
    sm.addAthleteToUnit("理学院", "SCI104", "严S", "女");
    sm.registerAthleteForEvent("SCI104", "女子400米", "女");
    
    // 4. 为所有项目记录成绩并计分
    std::cout << "\n正在录入和处理成绩...\n";

    // --- 男子100米 ---
    sm.recordResult("男子100米", "男", "CS001", 11.2);
    sm.recordResult("男子100米", "男", "CS002", 11.5);
    sm.recordResult("男子100米", "男", "CS003", 11.1);
    sm.recordResult("男子100米", "男", "CS007", 11.8);
    sm.recordResult("男子100米", "男", "CS008", 11.3);
    sm.recordResult("男子100米", "男", "FL001", 11.0);
    sm.recordResult("男子100米", "男", "ME005", 11.4);
    sm.recordResult("男子100米", "男", "SCI004", 11.6);
    sm.processScoresForEvent("男子100米", "男");

    // --- 男子1500米 ---
    sm.recordResult("男子1500米", "男", "CS002", 245.2); // 4:05.2
    sm.recordResult("男子1500米", "男", "CS004", 250.1); // 4:10.1
    sm.recordResult("男子1500米", "男", "CS005", 243.8); // 4:03.8
    sm.recordResult("男子1500米", "男", "FL002", 248.5); // 4:08.5
    sm.recordResult("男子1500米", "男", "ME001", 240.5); // 4:00.5
    sm.processScoresForEvent("男子1500米", "男");

    // --- 男子跳高 ---
    sm.recordResult("男子跳高", "男", "CS001", 1.95);
    sm.recordResult("男子跳高", "男", "CS006", 1.98);
    sm.recordResult("男子跳高", "男", "FL001", 2.01);
    sm.recordResult("男子跳高", "男", "SCI001", 1.92);
    sm.recordResult("男子跳高", "男", "SCI002", 1.85);
    sm.recordResult("男子跳高", "男", "SCI003", 1.88);
    sm.processScoresForEvent("男子跳高", "男");

    // --- 男子铅球 ---
    sm.recordResult("男子铅球", "男", "CS003", 12.5);
    sm.recordResult("男子铅球", "男", "ME001", 13.1);
    sm.recordResult("男子铅球", "男", "ME002", 13.5);
    sm.recordResult("男子铅球", "男", "ME003", 12.8);
    sm.recordResult("男子铅球", "男", "ME004", 14.0);
    sm.processScoresForEvent("男子铅球", "男");

    // --- 女子100米 ---
    sm.recordResult("女子100米", "女", "CS101", 12.5);
    sm.recordResult("女子100米", "女", "CS102", 12.8);
    sm.recordResult("女子100米", "女", "FL101", 12.3);
    sm.recordResult("女子100米", "女", "FL103", 12.6);
    sm.processScoresForEvent("女子100米", "女");

    // --- 女子800米 ---
    sm.recordResult("女子800米", "女", "CS101", 150.5); // 2:30.5
    sm.recordResult("女子800米", "女", "CS103", 148.2); // 2:28.2
    sm.recordResult("女子800米", "女", "FL102", 152.1); // 2:32.1
    sm.recordResult("女子800米", "女", "FL104", 147.9); // 2:27.9
    sm.recordResult("女子800米", "女", "SCI101", 155.0); // 2:35.0
    sm.recordResult("女子800米", "女", "SCI102", 153.3); // 2:33.3
    sm.processScoresForEvent("女子800米", "女");

    // --- 女子跳远 ---
    sm.recordResult("女子跳远", "女", "CS102", 5.80);
    sm.recordResult("女子跳远", "女", "FL101", 6.01);
    sm.recordResult("女子跳远", "女", "FL102", 5.95);
    sm.recordResult("女子跳远", "女", "ME101", 5.77);
    sm.recordResult("女子跳远", "女", "ME102", 5.89);
    sm.processScoresForEvent("女子跳远", "女");

    // --- 女子400米 ---
    sm.recordResult("女子400米", "女", "CS104", 62.5);
    sm.recordResult("女子400米", "女", "SCI103", 61.2);
    sm.recordResult("女子400米", "女", "SCI104", 60.9);
    sm.processScoresForEvent("女子400米", "女");
    
    std::cout << "\n示例数据加载完毕。\n";
}