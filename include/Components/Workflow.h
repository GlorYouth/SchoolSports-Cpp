#ifndef WORKFLOW_H
#define WORKFLOW_H

/**
 * @brief 定义运动会进行的不同工作流程阶段
 */
enum class WorkflowStage {
    SETUP_EVENTS,        ///< 项目设置阶段：管理员预设项目信息，配置场地、时间等。
    REGISTRATION_OPEN,   ///< 运动员报名阶段：赛程核心信息锁定后，运动员进行报名。
    COMPETITION_RUNNING  ///< 比赛管理阶段：报名结束后，进行成绩录入、排名统计等。
};

#endif //WORKFLOW_H
