

#ifndef DATAFILEMANAGER_H
#define DATAFILEMANAGER_H

#include <string>
#include <optional>
#include <fstream>
#include <map>
#include <vector>
#include "../Core/SystemSettings.h"

/**
 * @brief 数据文件管理器，负责系统数据的文件读写操作
 * 
 * DataFileManager负责处理与文件系统的所有交互，包括保存数据到文件和从文件加载数据。
 * 它专注于序列化和反序列化操作，不包含业务逻辑。
 */
class DataFileManager {
public:
    /**
     * @brief 构造函数
     */
    DataFileManager();

    /**
     * @brief 将系统设置保存到文件
     * @param settings 系统设置引用
     * @param filePath 要保存的文件路径
     * @return 如果保存成功返回true，否则返回false
     */
    bool saveDataToFile(const SystemSettings& settings, const std::string& filePath) const;

    /**
     * @brief 从文件加载系统设置
     * @param settings 要被更新的系统设置引用
     * @param filePath 要加载的文件路径
     * @return 如果加载成功返回true，否则返回false
     */
    bool loadDataFromFile(SystemSettings& settings, const std::string& filePath);

    /**
     * @brief 生成默认的备份文件名（带时间戳）
     * @return 生成的文件名
     */
    std::string generateDefaultBackupFilename() const;
    
    /**
     * @brief 确保备份文件夹存在，如果不存在则创建
     * @return 如果操作成功返回true，否则返回false
     */
    bool ensureBackupDirectoryExists() const;
    
    /**
     * @brief 列出备份文件夹中的所有备份文件
     * @return 按时间降序排列的备份文件信息列表，每项包含ID和文件路径
     */
    std::vector<std::pair<int, std::string>> listBackupFiles() const;
    
    /**
     * @brief 通过ID获取备份文件路径
     * @param id 备份文件ID
     * @return 对应ID的备份文件路径，如果ID无效则返回空字符串
     */
    std::string getBackupFilePathById(int id) const;

    /**
     * @brief 获取备份文件夹路径
     * @return 备份文件夹路径
     */
    std::string getBackupDirectoryPath() const;

private:
    // ===== 文件保存辅助方法 =====

    /**
     * @brief 创建恢复前的自动备份
     * @param settings 系统设置
     * @return 备份文件路径
     */
    std::string createRestoreBackup(const SystemSettings& settings) const;
};

#endif //DATAFILEMANAGER_H 