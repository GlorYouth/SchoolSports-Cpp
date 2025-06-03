//
// Created by GlorYouth on 2025/6/2.
//

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <vector>
#include <map>

// 性别定义
enum class Gender {
    MALE,    // 男
    FEMALE,  // 女
    UNSPECIFIED // 未指定
};

// 比赛项目类型定义
enum class EventType {
    TRACK,       // 径赛
    FIELD,       // 田赛
    UNSPECIFIED  // 未指定
};

// 将性别枚举转换为字符串
inline std::string genderToString(const Gender gender) {
    switch (gender) {
        case Gender::MALE: return "男子";
        case Gender::FEMALE: return "女子";
        default: return "未指定";
    }
}

// 将项目类型枚举转换为字符串
inline std::string eventTypeToString(const EventType type) {
    switch (type) {
        case EventType::TRACK: return "径赛";
        case EventType::FIELD: return "田赛";
        default: return "未指定";
    }
}

#endif //CONSTANTS_H
