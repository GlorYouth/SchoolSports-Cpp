//
// Created by GlorYouth on 2025/6/2.
//

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <vector>
#include <map>

// 性别定义
/**
 * @brief 定义运动员或比赛项目的性别属性。
 */
enum class Gender {
    MALE,         ///< 男性
    FEMALE,       ///< 女性
    UNSPECIFIED   ///< 未指定性别或混合性别项目
};

// 比赛项目类型定义
/**
 * @brief 定义比赛项目的类型。
 */
enum class EventType {
    TRACK,       ///< 径赛项目 (如跑步、跨栏)
    FIELD,       ///< 田赛项目 (如跳高、跳远、投掷)
    UNSPECIFIED  ///< 未指定项目类型
};

/**
 * @brief 将 Gender 枚举值转换为对应的中文字符串表示。
 * @param gender 要转换的 Gender 枚举值。
 * @return 表示性别的中文字符串 (例如：“男子”, “女子”, “未指定”)。
 */
inline std::string genderToString(const Gender gender) {
    switch (gender) {
        case Gender::MALE: return "男子";
        case Gender::FEMALE: return "女子";
        default: return "未指定";
    }
}

/**
 * @brief 将 EventType 枚举值转换为对应的中文字符串表示。
 * @param type 要转换的 EventType 枚举值。
 * @return 表示项目类型的中文字符串 (例如：“径赛”, “田赛”, “未指定”)。
 */
inline std::string eventTypeToString(const EventType type) {
    switch (type) {
        case EventType::TRACK: return "径赛";
        case EventType::FIELD: return "田赛";
        default: return "未指定";
    }
}

#endif //CONSTANTS_H
