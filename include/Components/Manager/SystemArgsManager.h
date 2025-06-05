#ifndef SYSTEMARGSMANAGER_H
#define SYSTEMARGSMANAGER_H

// 前向声明
class SystemSettings;

class SystemArgsManager {
public:
    explicit SystemArgsManager(SystemSettings& settings);
    
    // 运动员允许参加的最多项目数
    void setAthleteMaxEventsAllowed(int maxEvents);
    int getAthleteMaxEventsAllowed() const;
    
    // 未来可能添加的其他系统参数
    // void setMinimumParticipantsForEvent(int min);
    // int getMinimumParticipantsForEvent() const;
    
    // 重置所有系统参数为默认值
    void resetToDefaults();

private:
    SystemSettings& settings;
};

#endif // SYSTEMARGSMANAGER_H 