#include <iostream>
#include <limits>
#include "SportsMeet.h"

// 在 Windows 平台下，包含 windows.h 头文件并设置代码页
#ifdef _WIN32
// 防止 windows.h 中的 min/max 宏与 C++ 标准库冲突
#define NOMINMAX
#include <windows.h>
#endif

// 声明新的辅助函数
Event* selectEvent(SportsMeet& sm);

void showMainMenu() {
    std::cout << "\n=====================================\n";
    std::cout << "      学校运动会管理系统\n";
    std::cout << "=====================================\n";
    std::cout << "  1. 参赛项目发布与管理\n";
    std::cout << "  2. 运动员报名登记\n";
    std::cout << "  3. 参赛信息查询\n";
    std::cout << "  4. 记录比赛成绩\n";
    std::cout << "  5. 秩序册管理\n";
    std::cout << "  6. 系统设置\n";
    std::cout << "  7. 备份与恢复\n";
    std::cout << "  8. 退出系统\n";
    std::cout << "-------------------------------------\n";
    std::cout << "请输入您的选择 [1-8]: ";
}

void handleSystemSettings(SportsMeet& sm) {
    int choice;
    std::cout << "\n--- 系统设置 ---" << std::endl;
    std::cout << "1. 添加参赛单位" << std::endl;
    std::cout << "2. 管理计分规则" << std::endl;
    std::cout << "3. 返回主菜单" << std::endl;
    std::cout << "请输入您的选择: ";
    std::cin >> choice;

    switch (choice) {
        case 1: {
            std::string unitName;
            std::cout << "\n--- 添加参赛单位 ---" << std::endl;
            std::cout << "请输入单位名称: ";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 清除输入缓冲区
            std::getline(std::cin, unitName);
            sm.addUnit(unitName);
            break;
        }
        case 2:
            sm.manageScoringRules();
            break;
        case 3:
            break;
        default:
            std::cout << "无效选择。" << std::endl;
    }
}

void handleScheduleManagement(SportsMeet& sm) {
    int choice;
    std::cout << "\n--- 秩序册管理 ---" << std::endl;
    std::cout << "1. 自动生成秩序册" << std::endl;
    std::cout << "2. 查看秩序册" << std::endl;
    std::cout << "3. 返回主菜单" << std::endl;
    std::cout << "请输入您的选择: ";
    std::cin >> choice;

    switch (choice) {
        case 1:
            sm.generateSchedule();
            break;
        case 2:
            sm.showSchedule();
            break;
        case 3:
            break;
        default:
            std::cout << "无效选择。" << std::endl;
            break;
    }
}

void handleBackupRestore(SportsMeet& sm) {
    int choice;
    std::cout << "\n--- 备份与恢复 ---" << std::endl;
    std::cout << "1. 备份数据到文件" << std::endl;
    std::cout << "2. 从文件恢复数据" << std::endl;
    std::cout << "3. 返回主菜单" << std::endl;
    std::cout << "请输入您的选择: ";
    std::cin >> choice;

    std::string filename = "sportsmeet.dat"; // 默认文件名
    switch (choice) {
        case 1:
            sm.backupData(filename);
            break;
        case 2:
            sm.restoreData(filename);
            break;
        case 3:
            break;
        default:
            std::cout << "无效选择。" << std::endl;
            break;
    }
}

void handleEventManager(SportsMeet& sm) {
    int choice;
    do {
        std::cout << "\n--- 参赛项目发布与管理 ---" << std::endl;
        std::cout << "1. 添加新项目" << std::endl;
        std::cout << "2. 修改项目信息" << std::endl;
        std::cout << "3. 删除项目" << std::endl;
        std::cout << "4. 查看所有项目" << std::endl;
        std::cout << "5. 返回主菜单" << std::endl;
        std::cout << "请输入您的选择: ";
        std::cin >> choice;

        // 处理无效输入
        while (std::cin.fail()) {
            std::cout << "输入无效，请输入一个数字。" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "请输入您的选择: ";
            std::cin >> choice;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 清除换行符

        switch (choice) {
            case 1: {
                std::string name;
                int typeChoice, genderChoice, duration, min_participants, max_participants;

                std::cout << "请输入新项目名称: ";
                std::getline(std::cin, name);
                
                std::cout << "请选择项目类型 (1: 径赛, 2: 田赛): ";
                std::cin >> typeChoice;
                EventType eventType = (typeChoice == 1) ? EventType::TRACK : EventType::FIELD;

                std::cout << "请选择性别组别 (1: 男子, 2: 女子): ";
                std::cin >> genderChoice;
                GenderCategory gender = (genderChoice == 1) ? GenderCategory::MALE : GenderCategory::FEMALE;

                std::cout << "请输入项目预计时长（分钟）: ";
                std::cin >> duration;

                std::cout << "请输入最少参赛人数: ";
                std::cin >> min_participants;
                std::cout << "请输入最大参赛人数: ";
                std::cin >> max_participants;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                sm.addEvent(name, eventType, gender, duration, min_participants, max_participants);
                break;
            }
            case 2: { // Modify Event
                std::cout << "\n--- 请选择要修改的项目 ---" << std::endl;
                Event* event = selectEvent(sm);
                if (!event) break; // User cancelled or no events

                int editChoice;
                do {
                    std::cout << "\n--- 修改项目: " << event->getName() << " ---" << std::endl;
                    std::cout << "1. 修改时长 (当前: " << event->getDuration() << "分钟)" << std::endl;
                    std::cout << "2. 修改最少人数 (当前: " << event->getMinParticipants() << ")" << std::endl;
                    std::cout << "3. 修改最大人数 (当前: " << event->getMaxParticipants() << ")" << std::endl;
                    std::cout << "4. 完成修改" << std::endl;
                    std::cout << "请选择要修改的属性: ";
                    std::cin >> editChoice;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    int newValue;
                    switch (editChoice) {
                        case 1:
                            std::cout << "请输入新的时长(分钟): ";
                            std::cin >> newValue;
                            event->setDuration(newValue);
                            std::cout << "时长已更新。" << std::endl;
                            break;
                        case 2:
                            std::cout << "请输入新的最少人数: ";
                            std::cin >> newValue;
                            event->setMinParticipants(newValue);
                             std::cout << "最少人数已更新。" << std::endl;
                            break;
                        case 3:
                            std::cout << "请输入新的最大人数: ";
                            std::cin >> newValue;
                            event->setMaxParticipants(newValue);
                            std::cout << "最大人数已更新。" << std::endl;
                            break;
                        case 4:
                            break;
                        default:
                            std::cout << "无效选择。" << std::endl;
                    }
                } while (editChoice != 4);
                break;
            }
            case 3: { // Delete Event
                std::cout << "\n--- 请选择要删除的项目 ---" << std::endl;
                Event* event_to_delete = selectEvent(sm);
                if (!event_to_delete) break;

                sm.deleteEvent(event_to_delete->getName(), event_to_delete->getGender());
                break;
            }
            case 4:
                sm.showAllEvents();
                break;
            case 5:
                std::cout << "返回主菜单..." << std::endl;
                break;
            default:
                std::cout << "无效的选择，请输入1-5之间的数字。" << std::endl;
        }
    } while (choice != 5);
}

void handleInfoQuery(SportsMeet& sm) {
    int choice;
    std::cout << "\n--- 参赛信息查询 ---" << std::endl;
    std::cout << "1. 查询所有单位和项目" << std::endl;
    std::cout << "2. 查询特定单位详情" << std::endl;
    std::cout << "3. 查询特定项目详情" << std::endl;
    std::cout << "4. 返回主菜单" << std::endl;
    std::cout << "请输入您的选择: ";
    std::cin >> choice;

    std::string name;
    switch(choice) {
        case 1:
            sm.showAllUnits();
            sm.showAllEvents();
            break;
        case 2:
            std::cout << "请输入要查询的单位名称: ";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::getline(std::cin, name);
            sm.showUnitDetails(name);
            break;
        case 3:
            int genderChoice;
            std::cout << "请输入要查询的项目名称: ";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::getline(std::cin, name);
            std::cout << "请选择项目性别组别 (1: 男, 2: 女): ";
            std::cin >> genderChoice;
            sm.showEventDetails(name, (genderChoice == 1) ? GenderCategory::MALE : GenderCategory::FEMALE);
            break;
        case 4:
            break;
        default:
            std::cout << "无效选择。" << std::endl;
    }
}

void loadSampleData(SportsMeet& sm) {
    std::cout << "正在加载示例数据..." << std::endl;

    // 1. 添加单位
    sm.addUnit("计算机学院");
    sm.addUnit("外国语学院");
    sm.addUnit("机械工程学院");
    sm.addUnit("理学院");

    // 2. 添加项目 (名称, 类型, 性别, 持续时间(分钟), 最少/最多人数)
    sm.addEvent("男子100米", EventType::TRACK, GenderCategory::MALE, 60, 5, 40);
    sm.addEvent("男子1500米", EventType::TRACK, GenderCategory::MALE, 90, 5, 30);
    sm.addEvent("男子跳高", EventType::FIELD, GenderCategory::MALE, 120, 4, 20);
    sm.addEvent("男子铅球", EventType::FIELD, GenderCategory::MALE, 120, 4, 25);
    sm.addEvent("女子100米", EventType::TRACK, GenderCategory::FEMALE, 60, 5, 40);
    sm.addEvent("女子800米", EventType::TRACK, GenderCategory::FEMALE, 90, 5, 30);
    sm.addEvent("女子跳远", EventType::FIELD, GenderCategory::FEMALE, 120, 4, 20);
    sm.addEvent("女子400米", EventType::TRACK, GenderCategory::FEMALE, 45, 5, 30);

    // 3. 注册运动员并报名
    // 为了方便，直接获取单位和项目指针
    Unit* cs = sm.findUnit("计算机学院");
    Unit* fl = sm.findUnit("外国语学院");
    Unit* me = sm.findUnit("机械工程学院");
    Unit* sci = sm.findUnit("理学院");

    Event* m100 = sm.findEvent("男子100米", GenderCategory::MALE);
    Event* m1500 = sm.findEvent("男子1500米", GenderCategory::MALE);
    Event* mHighJump = sm.findEvent("男子跳高", GenderCategory::MALE);
    Event* mShotPut = sm.findEvent("男子铅球", GenderCategory::MALE);
    Event* w100 = sm.findEvent("女子100米", GenderCategory::FEMALE);
    Event* w800 = sm.findEvent("女子800米", GenderCategory::FEMALE);
    Event* wLongJump = sm.findEvent("女子跳远", GenderCategory::FEMALE);
    Event* w400 = sm.findEvent("女子400米", GenderCategory::FEMALE);
    
    // 计算机学院
    cs->addAthlete("张三", "CS001", true)->registerForEvent(m100);
    cs->findAthlete("CS001")->registerForEvent(mHighJump);
    cs->addAthlete("李四", "CS002", true)->registerForEvent(m100);
    cs->findAthlete("CS002")->registerForEvent(m1500);
    cs->addAthlete("王五", "CS003", true)->registerForEvent(m100);
    cs->findAthlete("CS003")->registerForEvent(mShotPut);
    cs->addAthlete("赵六", "CS004", true)->registerForEvent(m1500);
    cs->addAthlete("孙七", "CS005", true)->registerForEvent(m1500);
    cs->addAthlete("周八", "CS006", true)->registerForEvent(mHighJump);
    cs->addAthlete("吴九", "CS007", true)->registerForEvent(m100);
    cs->addAthlete("郑十", "CS008", true)->registerForEvent(m100);
    cs->addAthlete("陈一一", "CS101", false)->registerForEvent(w100);
    cs->findAthlete("CS101")->registerForEvent(w800);
    cs->addAthlete("钱一二", "CS102", false)->registerForEvent(w100);
    cs->findAthlete("CS102")->registerForEvent(wLongJump);
    cs->addAthlete("卫一三", "CS103", false)->registerForEvent(w800);
    cs->addAthlete("蒋一四", "CS104", false)->registerForEvent(w400);

    // 外国语学院
    fl->addAthlete("冯A", "FL001", true)->registerForEvent(m100);
    fl->findAthlete("FL001")->registerForEvent(mHighJump);
    fl->addAthlete("陈B", "FL002", true)->registerForEvent(m1500);
    fl->addAthlete("褚C", "FL101", false)->registerForEvent(w100);
    fl->findAthlete("FL101")->registerForEvent(wLongJump);
    fl->addAthlete("卫D", "FL102", false)->registerForEvent(w800);
    fl->findAthlete("FL102")->registerForEvent(wLongJump);
    fl->addAthlete("蒋E", "FL103", false)->registerForEvent(w100);
    fl->addAthlete("沈F", "FL104", false)->registerForEvent(w800);

    // 机械工程学院
    me->addAthlete("韩G", "ME001", true)->registerForEvent(m1500);
    me->findAthlete("ME001")->registerForEvent(mShotPut);
    me->addAthlete("杨H", "ME002", true)->registerForEvent(mShotPut);
    me->addAthlete("朱I", "ME003", true)->registerForEvent(mShotPut);
    me->addAthlete("秦J", "ME004", true)->registerForEvent(mShotPut);
    me->addAthlete("许AA", "ME005", true)->registerForEvent(m100);
    me->addAthlete("尤K", "ME101", false)->registerForEvent(wLongJump);
    me->addAthlete("许L", "ME102", false)->registerForEvent(wLongJump);
    
    // 理学院
    sci->addAthlete("何M", "SCI001", true)->registerForEvent(mHighJump);
    sci->addAthlete("吕N", "SCI002", true)->registerForEvent(mHighJump);
    sci->addAthlete("施O", "SCI003", true)->registerForEvent(mHighJump);
    sci->addAthlete("施BB", "SCI004", true)->registerForEvent(m100);
    sci->addAthlete("张P", "SCI101", false)->registerForEvent(w800);
    sci->addAthlete("孔Q", "SCI102", false)->registerForEvent(w800);
    sci->addAthlete("曹R", "SCI103", false)->registerForEvent(w400);
    sci->addAthlete("严S", "SCI104", false)->registerForEvent(w400);

    // 更新 Event 对象中的参赛者列表 (重要!)
    auto all_units = {cs, fl, me, sci};
    auto all_events = {m100, m1500, mHighJump, mShotPut, w100, w800, wLongJump, w400};
    for(auto unit : all_units) {
        for(const auto& athlete : unit->getAthletes()) {
            for (auto event : athlete->getRegisteredEvents()) {
                event->addParticipant(athlete.get());
            }
        }
    }
    std::cout << "示例数据加载完毕。\n" << std::endl;
}

// 实现新的辅助函数
Event* selectEvent(SportsMeet& sm) {
    sm.showAllEvents();
    
    // 获取所有事件的扁平列表以进行编号
    std::vector<Event*> all_events;
    for (const auto& event_ptr : sm.getAllEvents()) {
        all_events.push_back(event_ptr.get());
    }

    if (all_events.empty()) {
        std::cout << "当前没有可供选择的项目。" << std::endl;
        return nullptr;
    }
    
    std::cout << "\n请输入项目对应的编号进行选择 (输入 0 取消): ";
    int selection;
    std::cin >> selection;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (selection > 0 && selection <= all_events.size()) {
        return all_events[selection - 1];
    } else {
        if (selection != 0) {
            std::cout << "无效的编号。" << std::endl;
        }
        return nullptr;
    }
}

int main() {
    // 设置控制台代码页为 UTF-8 (仅在 Windows 上)
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    SportsMeet sportsMeet;

    // 询问用户是否加载示例数据
    char choice;
    std::cout << "是否加载示例数据? (y/n): ";
    std::cin >> choice;
    if (choice == 'y' || choice == 'Y') {
        loadSampleData(sportsMeet);
    }
    // 清除输入缓冲区，防止影响后续的菜单选择
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    int menuChoice;
    do {
        showMainMenu();
        std::cin >> menuChoice;

        // 检查输入是否有效
        while (std::cin.fail()) {
            std::cout << "输入无效，请输入一个数字。" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            showMainMenu();
            std::cin >> menuChoice;
        }

        switch (menuChoice) {
            case 1:
                handleEventManager(sportsMeet);
                break;
            case 2:
                sportsMeet.registerAthlete();
                break;
            case 3:
                handleInfoQuery(sportsMeet);
                break;
            case 4: {
                std::cout << "\n--- 请选择要记录成绩的项目 ---" << std::endl;
                Event* event = selectEvent(sportsMeet);
                if (event) {
                    sportsMeet.recordAndScoreEvent(event);
                }
                break;
            }
            case 5:
                handleScheduleManagement(sportsMeet);
                break;
            case 6:
                handleSystemSettings(sportsMeet);
                break;
            case 7:
                handleBackupRestore(sportsMeet);
                break;
            case 8:
                std::cout << "感谢使用，系统退出。" << std::endl;
                break;
            default:
                std::cout << "无效选择，请输入1-8之间的数字。" << std::endl;
                break;
        }
    } while (menuChoice != 8);

    return 0;
} 