#ifndef WORKFLOWMANAGER_H
#define WORKFLOWMANAGER_H

#include "../Core/Workflow.h"

// 前向声明
class SystemSettings;

class WorkflowManager {
public:
    explicit WorkflowManager(SystemSettings& settings);
    
    // 查询工作流阶段
    WorkflowStage getCurrentStage() const;
    
    // 设置工作流阶段
    bool setStage(WorkflowStage newStage);
    
    // 进入特定阶段的便捷方法
    bool enterSetupEventsStage();
    bool enterRegistrationStage();
    bool enterCompetitionStage();
    
    // 检查是否处于特定阶段
    bool isInSetupEventsStage() const;
    bool isInRegistrationStage() const;
    bool isInCompetitionStage() const;

private:
    SystemSettings& settings;
};

#endif // WORKFLOWMANAGER_H 