

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
     * @brief 写入元数据段
     * @param outFile 输出文件流
     * @param settings 系统设置
     */
    void saveMetadata(std::ofstream& outFile, const SystemSettings& settings) const;
    
    /**
     * @brief 写入单位数据段
     * @param outFile 输出文件流
     * @param settings 系统设置
     */
    void saveUnits(std::ofstream& outFile, const SystemSettings& settings) const;
    
    /**
     * @brief 写入计分规则段
     * @param outFile 输出文件流
     * @param settings 系统设置
     */
    void saveScoreRules(std::ofstream& outFile, const SystemSettings& settings) const;
    
    /**
     * @brief 写入比赛项目段
     * @param outFile 输出文件流
     * @param settings 系统设置
     */
    void saveEvents(std::ofstream& outFile, const SystemSettings& settings) const;
    
    /**
     * @brief 写入运动员段
     * @param outFile 输出文件流
     * @param settings 系统设置
     */
    void saveAthletes(std::ofstream& outFile, const SystemSettings& settings) const;
    
    /**
     * @brief 写入成绩段
     * @param outFile 输出文件流
     * @param settings 系统设置
     */
    void saveResults(std::ofstream& outFile, const SystemSettings& settings) const;
    
    /**
     * @brief 写入场馆信息段
     * @param outFile 输出文件流
     * @param settings 系统设置
     */
    void saveVenues(std::ofstream& outFile, const SystemSettings& settings) const;
    
    // ===== 文件加载辅助方法 =====
    
    /**
     * @brief 处理元数据段
     * @param line 当前读取的行
     * @param settings 系统设置
     */
    void processMetadata(const std::string& line, SystemSettings& settings);
    
    /**
     * @brief 处理单位数据行
     * @param line 当前读取的行
     * @param settings 系统设置
     */
    void processUnit(const std::string& line, SystemSettings& settings);
    
    /**
     * @brief 处理计分规则数据行
     * @param line 当前读取的行
     * @param settings 系统设置
     * @return 是否为ID为1的默认规则
     */
    bool processScoreRule(const std::string& line, SystemSettings& settings);
    
    /**
     * @brief 处理比赛项目数据行
     * @param line 当前读取的行
     * @param settings 系统设置
     * @param athleteEventMap 用于存储运动员与项目的关联，加载运动员后使用
     */
    void processEvent(const std::string& line, SystemSettings& settings, std::map<int, std::vector<int>>& athleteEventMap);
    
    /**
     * @brief 处理运动员数据行
     * @param line 当前读取的行
     * @param settings 系统设置
     */
    void processAthlete(const std::string& line, SystemSettings& settings);
    
    /**
     * @brief 处理成绩数据行
     * @param line 当前读取的行
     * @param settings 系统设置
     */
    void processResult(const std::string& line, SystemSettings& settings);
    
    /**
     * @brief 创建恢复前的自动备份
     * @param settings 系统设置
     * @return 备份文件路径
     */
    std::string createRestoreBackup(const SystemSettings& settings) const;
};

#endif //DATAFILEMANAGER_H 