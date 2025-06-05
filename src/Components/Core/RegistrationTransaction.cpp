#include "../../../include/Components/Core/RegistrationTransaction.h"

RegistrationTransaction::RegistrationTransaction(SystemSettings& settings)
    : settings(settings), committed(false) {
}

bool RegistrationTransaction::begin() {
    // 此处可以添加锁机制或其他同步措施，如果需要的话
    // 目前简单返回true，表示事务已开始
    return true;
}

bool RegistrationTransaction::registerForEvent(int athleteId, int eventId) {
    // 获取对象引用
    auto athleteOpt = settings.athletes.get(athleteId);
    auto eventOpt = settings.events.get(eventId);
    
    if (!athleteOpt) {
        std::cerr << "错误：报名失败，运动员ID " << athleteId << " 未找到。" << std::endl;
        return false;
    }
    
    if (!eventOpt) {
        std::cerr << "错误：报名失败，项目ID " << eventId << " 未找到。" << std::endl;
        return false;
    }
    
    Athlete& athlete = athleteOpt.value().get();
    CompetitionEvent& event = eventOpt.value().get();
    
    // 检查项目是否已取消
    if (event.getIsCancelled()) {
        std::cerr << "报名失败: 项目 \"" << event.getName() << "\" 已取消。" << std::endl;
        return false;
    }
    
    // 验证性别要求
    if (!event.canAthleteRegister(athlete.getGender())) {
        std::cerr << "报名失败: 运动员 " << athlete.getName()
                << " 性别(" << genderToString(athlete.getGender())
                << ") 不符合项目 \"" << event.getName()
                << "\" 性别要求(" << genderToString(event.getGenderRequirement()) << ")." << std::endl;
        return false;
    }
    
    // 验证报名数量限制
    int maxAllowed = settings.athletes.getMaxEventsAllowed();
    if (athlete.getRegisteredEventsCount() >= maxAllowed) {
        std::cerr << "报名失败: 运动员 " << athlete.getName() << " 已达到最大报名项目数 ("
                << maxAllowed << ")." << std::endl;
        return false;
    }
    
    // 执行修改
    bool athleteSideSuccess = athlete.registerForEvent(eventId, maxAllowed);
    if (!athleteSideSuccess) {
        if (athlete.isRegisteredForEvent(eventId)) {
            std::cerr << "报名失败: 运动员 " << athlete.getName() << " 已报名项目 \"" << event.getName() << "\"。" << std::endl;
        } else {
            std::cerr << "报名失败: 运动员 " << athlete.getName() << " 无法报名项目 \"" << event.getName() << "\" (未知原因)。" << std::endl;
        }
        return false;
    }
    
    bool eventSideSuccess = event.addParticipant(athleteId);
    if (!eventSideSuccess) {
        // 回滚运动员侧的修改
        athlete.unregisterFromEvent(eventId);
        std::cerr << "报名失败: 运动员 " << athlete.getName() << " 已在项目 \"" << event.getName()
                << "\" 的参与者列表中 (理论上不应发生，已回滚运动员报名)。" << std::endl;
        return false;
    }
    
    std::cout << "报名成功: 运动员 " << athlete.getName() << " 已报名参加项目 \"" << event.getName() << "\"" << std::endl;
    return true;
}

bool RegistrationTransaction::unregisterFromEvent(int athleteId, int eventId) {
    auto athleteOpt = settings.athletes.get(athleteId);
    auto eventOpt = settings.events.get(eventId);
    
    if (!athleteOpt) {
        std::cerr << "取消报名失败: 运动员ID " << athleteId << " 不存在。" << std::endl;
        return false;
    }
    
    if (!eventOpt) {
        std::cerr << "取消报名失败: 项目ID " << eventId << " 不存在。" << std::endl;
        return false;
    }
    
    Athlete& athlete = athleteOpt.value().get();
    CompetitionEvent& event = eventOpt.value().get();
    
    // 尝试从运动员的报名列表中移除
    bool athleteSideSuccess = athlete.unregisterFromEvent(eventId);
    if (!athleteSideSuccess) {
        std::cerr << "取消报名失败: 运动员 " << athlete.getName() << " 未报名项目 \"" 
                << event.getName() << "\" (或无法从运动员记录中移除)。" << std::endl;
        return false;
    }
    
    // 尝试从项目的参与者列表中移除
    bool eventSideSuccess = event.removeParticipant(athleteId);
    if (!eventSideSuccess) {
        std::cerr << "警告: 运动员 " << athlete.getName() << " 已从个人报名列表取消项目 \"" 
                << event.getName() << "\", 但未在项目参与者列表中找到 (数据可能不一致)。" << std::endl;
        // 即使这里失败，也认为运动员的取消操作是主要的，所以继续
    }
    
    std::cout << "取消报名成功: 运动员 " << athlete.getName() << " 已取消报名项目 \"" << event.getName() << "\"" << std::endl;
    return true;
}

bool RegistrationTransaction::commit() {
    if (committed) return false;
    committed = true;
    return true;
}

bool RegistrationTransaction::rollback() {
    if (committed) return false;
    // 在更复杂的实现中，这里可以执行回滚逻辑
    return true;
} 