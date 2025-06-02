//
// Created by GlorYouth on 2025/6/2.
//

#ifndef REGISTRATION_H
#define REGISTRATION_H
#include "SystemSettings.h"

// 报名管理类
class Registration {
private:
    SystemSettings& settings; // 引用系统设置，以便访问运动员、项目和规则

public:
    explicit Registration(SystemSettings& sysSettings);

    // 运动员报名参赛项目
    // 返回true表示报名成功，false表示失败 (例如超过项目限制、项目不存在、性别不符等)
    bool registerAthleteForEvent(int athleteId, int eventId) const;

    // 运动员取消报名
    bool unregisterAthleteFromEvent(int athleteId, int eventId) const;

    // 检查项目是否因人数不足而需要取消
    void checkAndCancelEventsDueToLowParticipation() const;

    // 其他报名相关功能...
};

#endif //REGISTRATION_H
