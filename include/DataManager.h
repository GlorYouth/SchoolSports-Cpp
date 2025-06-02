//
// Created by GlorYouth on 2025/6/2.
//

#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <string>
#include "SystemSettings.h" // 需要备份和恢复系统中的所有数据

// 数据管理类 (负责数据持久化，如备份和恢复)
class DataManager {
private:
    SystemSettings& settings; // 引用系统设置以访问数据

public:
    explicit DataManager(SystemSettings& sysSettings);

    // 将当前系统数据备份到文件
    // filePath: 文件路径
    [[nodiscard]] bool backupData(const std::string& filePath) const;

    // 从文件恢复数据到系统
    // filePath: 文件路径
    // 返回true表示恢复成功，false表示失败
    bool restoreData(const std::string& filePath);

    // 其他数据管理功能，如导入导出特定格式数据等
};


#endif //DATAMANAGER_H
