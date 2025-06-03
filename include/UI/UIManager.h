//
// Created by GlorYouth on 2025/6/3.
//

#ifndef UIMANAGER_H
#define UIMANAGER_H




#include <iostream>
#include <vector>
#include <string>
#include <limits>   // 用于 std::numeric_limits
#include <iomanip>  // 用于 std::setw, std::fixed, std::setprecision
#include <functional> // 用于 std::reference_wrapper

// 前向声明核心数据结构，避免不必要的头文件依赖
// 实际项目中，如果 UIManager 需要访问这些类的成员，可能需要包含它们的完整头文件
// 或者只包含那些定义了 UIManager 所需接口的头文件。
class Unit;
class Athlete;
class CompetitionEvent;
class ScoreRule;
class EventResults;
class SystemSettings; // 包含以访问如 getMaxEventsAllowed 等设置

// 假设 Constants.h 中的枚举和转换函数在此可用或已移至更合适的位置
#include "../Components/Constants.h"
#include "../utils.h"

class UIManager {
public:
    // 清理输入缓冲区
    static void clearInputBuffer();

    // 获取整数输入
    static int getIntInput(const std::string& prompt);
    // 获取带范围校验的整数输入
    static int getIntInput(const std::string& prompt, int minVal, int maxVal);

    // 获取字符串输入
    static std::string getStringInput(const std::string& prompt);

    // 等待用户按 Enter 继续
    static void pressEnterToContinue(const std::string& message = "\n按 Enter键 返回上一级菜单...");

    // 显示通用消息
    static void showMessage(const std::string& message);
    // 显示成功消息
    static void showSuccessMessage(const std::string& message);
    // 显示错误消息
    static void showErrorMessage(const std::string& message);

    // --- 菜单显示 ---
    static void displayMainMenu();
    static void displaySystemSettingsMenu(const SystemSettings& settings);
    static void displayRegistrationMenu();
    static void displayScheduleMenu();
    static void displayResultsMenu();
    static void displayQueryMenu(); // 用于参赛信息查询
    static void displayDataManagementMenu();


    // --- 数据列表显示 ---
    // 注意：这里参数类型改为 const std::vector<utils::RefConst T>>&
    // 以便传递从 map 中获取的对象的引用集合，避免拷贝，同时保持 const 正确性。
    // 或者，可以直接传递 SystemSettings& settings 和相应的 getter 方法名或 lambda。
    // 为简化，这里假设控制器已经准备好了要显示的数据。

    static void displayUnits(const std::vector<utils::RefConst<Unit>>& units);
    static void displayAthletes(const std::vector<utils::RefConst<Athlete>>& athletes, const SystemSettings& settings);
    static void displayEvents(const std::vector<utils::RefConst<CompetitionEvent>>& events, const SystemSettings& settings);
    static void displayScoreRules(const std::vector<utils::RefConst<ScoreRule>>& rules);

    // 成绩相关显示
    static void displayEventResultsDetails(const CompetitionEvent& event,
                                           const EventResults* results, // 可能为 nullptr
                                           const SystemSettings& settings); // 用于查找运动员姓名等
    static void displayUnitStandings(const std::vector<utils::RefConst<Unit>>& sortedUnits);


    // --- 特定输入的辅助函数 ---
    static Gender getGenderInput(const std::string& prompt, bool allowUnspecified = false);
    static EventType getEventTypeInput(const std::string& prompt);
};




#endif //UIMANAGER_H
