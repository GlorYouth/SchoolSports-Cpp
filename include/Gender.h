#ifndef GENDER_H
#define GENDER_H

#include <string>

// 定义性别枚举类型
enum class Gender {
    MALE,   // 男性
    FEMALE, // 女性
    MIXED   // 混合
};

// 将Gender枚举转换为字符串
inline std::string genderToString(Gender gender) {
    switch (gender) {
        case Gender::MALE: return "男";
        case Gender::FEMALE: return "女";
        case Gender::MIXED: return "混合";
        default: return "未知";
    }
}

// 将字符串转换为Gender枚举
inline Gender stringToGender(const std::string& str) {
    if (str == "男") return Gender::MALE;
    if (str == "女") return Gender::FEMALE;
    if (str == "混合") return Gender::MIXED;
    return Gender::MIXED; // 默认为混合类型，以便向后兼容
}

// 检查一个性别是否可以参加指定要求的项目
inline bool genderAllowed(Gender athleteGender, Gender eventGender) {
    // 如果是混合项目，所有性别都可以参加
    if (eventGender == Gender::MIXED) {
        return true;
    }
    // 否则性别必须匹配
    return athleteGender == eventGender;
}

#endif // GENDER_H 