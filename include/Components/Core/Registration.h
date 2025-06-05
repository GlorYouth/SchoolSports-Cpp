#ifndef REGISTRATION_H
#define REGISTRATION_H
#include "SystemSettings.h"

// 报名管理类
/**
 * @brief 负责处理运动员报名参赛和取消报名的逻辑。
 *
 * Registration 类依赖于 SystemSettings 来访问运动员、比赛项目、报名规则
 * （如每人最多报名项目数、项目性别要求、赛程时间冲突检测等）。
 * 它还负责检查并标记那些因报名人数不足而可能需要取消的项目。
 */
class Registration {
private:
    SystemSettings& settings; ///< 对系统设置的引用，用于访问运动员、项目、规则和赛程信息。

public:
    /**
     * @brief Registration 类的构造函数。
     * @param sysSettings 对 SystemSettings 对象的引用，报名管理将依赖此设置。
     */
    explicit Registration(SystemSettings& sysSettings);

    /**
     * @brief 为指定运动员报名参加指定的比赛项目。
     *
     * 此方法会进行多项检查：
     * 1. 运动员和项目是否存在。
     * 2. 运动员是否已达到最大报名项目数限制。
     * 3. 运动员性别是否符合项目要求。
     * 4. （如果赛程已锁定）报名项目是否与该运动员已报名的其他项目在时间上冲突。
     * 5. （如果赛程已锁定）报名项目与已报其他项目时间间隔是否过短（如小于30分钟，则警告）。
     * 6. 项目是否已被取消。
     *
     * @param athleteId 要报名的运动员ID。
     * @param eventId 目标比赛项目ID。
     * @return 如果报名成功，返回 true。如果因任何检查失败导致无法报名，则返回 false。
     *         相关的错误或警告信息会通过 UIManager 显示。
     */
    [[nodiscard]] bool registerAthleteForEvent(int athleteId, int eventId);

    /**
     * @brief 为指定运动员取消报名指定的比赛项目。
     *
     * 此方法会检查：
     * 1. 运动员和项目是否存在。
     * 2. 运动员是否确实报名了该项目。
     * 3. （如果赛程已锁定且不允许修改）报名是否已锁定。
     *
     * @param athleteId 要取消报名的运动员ID。
     * @param eventId 要取消的比赛项目ID。
     * @return 如果取消报名成功，返回 true。如果因任何原因无法取消，则返回 false。
     *         相关的错误信息会通过 UIManager 显示。
     */
    [[nodiscard]] bool unregisterAthleteFromEvent(int athleteId, int eventId);

    /**
     * @brief 检查所有比赛项目，对于报名人数不足的项目，将其标记为"已取消"。
     *
     * 此方法会遍历所有项目，根据 SystemSettings 中定义的计分规则（特别是人数下限）
     * 来判断项目是否应该取消。
     * @return 返回因此次检查而被取消的项目数量。
     * @note 此操作通常在报名截止后，赛程生成前执行。
     */
    int checkAndCancelEventsDueToLowParticipation();

    // 其他报名相关功能...
};

#endif //REGISTRATION_H
