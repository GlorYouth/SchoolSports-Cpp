

#ifndef SCORERULE_H
#define SCORERULE_H

#include <map>
#include <string>
#include <atomic> // 用于生成唯一ID
#include <optional>
#include <vector>
#include <vector> // 用于存储子规则

#include "../../utils.h"

/**
 * @brief 处理比赛项目的计分规则
 *
 * ScoreRule 封装了一个计分规则，包含适用条件（参赛人数范围）、
 * 录取名次数，以及每个名次对应的具体得分。
 * ID 由静态原子计数器自动生成，保证唯一性。
 * 
 * 实现了复合模式，一个规则ID可以包含多个子规则，
 * 系统会根据参赛人数自动选择最适用的子规则。
 */
class ScoreRule {
private:
    static std::atomic<int> nextId;         // 用于生成唯一的规则ID
    int id;                                 // 规则ID
    std::string description;                // 规则描述 (例如: "超过6人取前5名")
    int minParticipants;                    // 适用此规则的最小参赛人数 (包含)
    int maxParticipants;                    // 适用此规则的最大参赛人数 (包含, 使用 -1 表示无上限)
    int ranksToAward;                       // 录取名次数
    std::map<int, double> scoresForRanks;   // 名次对应的分数 <名次, 分数> (例如: {1:7, 2:5, ...})
    
    // 子规则列表（对于复合规则）
    std::vector<utils::Ref<ScoreRule>> subRules;       // 存储子规则引用
    bool isCompositeRule;                   // 是否为复合规则标志

public:
    /**
     * @brief ScoreRule 类的构造函数
     * @param desc 规则的文字描述
     * @param minP 适用此规则的最小参赛人数（含）
     * @param maxP 适用此规则的最大参赛人数（含）。若为-1，表示无人数上限
     * @param ranks 计划奖励的名次数
     * @param scores 一个map，键为名次（int），值为对应的分数（double）
     * @param isComposite 是否创建为复合规则，默认为false
     */
    ScoreRule(std::string desc, int minP, int maxP, int ranks, 
              const std::map<int, double>& scores, bool isComposite = false);
    
    /**
     * @brief 析构函数，负责清理子规则占用的内存
     */
    ~ScoreRule();

    /**
     * @brief 获取计分规则的唯一ID
     * @return 返回规则ID
     */
    [[nodiscard]] int getId() const;
    /**
     * @brief 获取计分规则的文字描述
     * @return 返回规则描述字符串
     */
    [[nodiscard]] std::string getDescription() const;

    /**
     * @brief 检测计分规则是否适用于给定的参赛人数
     * @param participantCount 实际参赛人数
     * @return 如果适用，返回 true；否则返回 false
     */
    [[nodiscard]] bool appliesTo(int participantCount) const;
    
    /**
     * @brief 根据参赛人数获取适用的子规则或自身
     * @param participantCount 实际参赛人数
     * @return 返回适用的规则指针，不会返回nullptr（如无匹配子规则则返回this）
     */
    [[nodiscard]] std::optional<utils::RefConst<ScoreRule>> getApplicableRule(int participantCount) const;

    /**
     * @brief 获取此规则计划录取的名次数量
     * @return 返回录取名次数
     */
    [[nodiscard]] int getRanksToAward() const;
    /**
     * @brief 根据指定名次获取对应的分数值
     * @param rank 要查询的名次
     * @return 如果名次在奖励范围内，则返回对应的分数；否则通常返回0
     */
    [[nodiscard]] double getScoreForRank(int rank) const;
    /**
     * @brief 获取此规则所有名次和对应分数的映射表
     * @return 返回一个名次到分数映射的常量map引用
     */
    [[nodiscard]] const std::map<int, double>& getAllScoresForRanks() const;
    /**
     * @brief 获取适用此规则的最小参赛人数
     * @return 返回最小参赛人数
     */
    [[nodiscard]] int getMinParticipants() const { return minParticipants; }
    /**
     * @brief 获取适用此规则的最大参赛人数
     * @return 返回最大参赛人数（-1表示无上限）
     */
    [[nodiscard]] int getMaxParticipants() const { return maxParticipants; }
    
    /**
     * @brief 添加一个子规则
     * @param subRule 要添加的子规则指针（由本类负责释放内存）
     */
    void addSubRule(ScoreRule& subRule);
    
    /**
     * @brief 检查是否为复合规则（包含子规则）
     * @return 如果是复合规则，返回true；否则返回false
     */
    [[nodiscard]] bool isComposite() const { return isCompositeRule; }
    
    /**
     * @brief 获取所有子规则
     * @return 返回子规则指针向量的常量引用
     */
    [[nodiscard]] std::vector<utils::RefConst<ScoreRule>> getSubRules() const {
        auto result = std::vector<utils::RefConst<ScoreRule>>();
        result.reserve(subRules.size());
        for (const auto& subRule : subRules){
            result.emplace_back(subRule);
        }
        return result;
    }

    /**
     * @brief 重置计数器，生成新的一组计分规则ID的静态方法
     * @param startId 可选，指定从指定ID重新开始的初始值，默认为1
     * @note 主要用于测试目的，确保每次测试时ID序列是可预测的
     */
    static void resetNextId(int startId = 1);
    
    /**
     * @brief 创建复合规则的静态工厂方法
     * @param desc 复合规则的描述
     * @param minP 最小参赛人数
     * @param maxP 最大参赛人数
     * @return 返回创建的复合规则指针（由调用者负责管理内存）
     */
    static ScoreRule createCompositeRule(const std::string& desc, int minP, int maxP);
};
#endif //SCORERULE_H
