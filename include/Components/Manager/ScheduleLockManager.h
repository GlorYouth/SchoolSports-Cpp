#ifndef SCHEDULELOCKMANAGER_H
#define SCHEDULELOCKMANAGER_H

// 前向声明
class SystemSettings;

class ScheduleLockManager {
public:
    explicit ScheduleLockManager(SystemSettings& settings);
    
    // 锁定和解锁赛程
    void lock();
    void unlock();
    
    // 检查赛程状态
    bool isLocked() const;

private:
    SystemSettings& settings;
};

#endif // SCHEDULELOCKMANAGER_H 