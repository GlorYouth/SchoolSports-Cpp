

#ifndef REGISTRATION_TRANSACTION_H
#define REGISTRATION_TRANSACTION_H

#include "SystemSettings.h"
#include "Athlete.h"
#include "CompetitionEvent.h"
#include <iostream>

/**
 * @brief 管理运动员报名/取消报名的事务类，用于保证Athlete和CompetitionEvent之间的一致性
 * 
 * RegistrationTransaction类实现了事务性操作模式，确保在运动员报名或取消报名时，
 * Athlete对象和CompetitionEvent对象的状态保持同步。确保运动员的registeredEventIds列表
 * 和比赛项目的participantAthleteIds列表保持一致性。
 */
class RegistrationTransaction {
public:
    /**
     * @brief RegistrationTransaction类的构造函数
     * @param settings 系统设置的引用，用于访问运动员和项目数据
     */
    explicit RegistrationTransaction(SystemSettings& settings);
    
    /**
     * @brief 开始事务
     * @return 如果事务成功开始，返回true
     */
    [[nodiscard]] bool begin();
    
    /**
     * @brief 执行运动员报名操作
     * @param athleteId 运动员ID
     * @param eventId 项目ID
     * @return 如果报名成功，返回true；否则返回false
     */
    [[nodiscard]] bool registerForEvent(int athleteId, int eventId);
    
    /**
     * @brief 执行运动员取消报名操作
     * @param athleteId 运动员ID
     * @param eventId 项目ID
     * @return 如果取消报名成功，返回true；否则返回false
     */
    [[nodiscard]] bool unregisterFromEvent(int athleteId, int eventId);
    
    /**
     * @brief 提交事务
     * @return 如果事务未提交过且成功提交，返回true；否则返回false
     */
    [[nodiscard]] bool commit();
    
    /**
     * @brief 回滚事务
     * @return 如果事务未提交过且成功回滚，返回true；否则返回false
     */
    [[nodiscard]] bool rollback();

private:
    SystemSettings& settings;
    bool committed;
};

#endif // REGISTRATION_TRANSACTION_H 