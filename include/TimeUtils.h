#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <string>
#include <iomanip>
#include <sstream>

namespace TimeUtils {
    /**
     * @brief 将分钟数（从午夜开始）转换为 HH:MM 格式的字符串
     */
    inline std::string toHHMM(int minutes) {
        int hours = minutes / 60;
        int mins = minutes % 60;
        std::stringstream ss;
        ss << std::setw(2) << std::setfill('0') << hours << ":"
           << std::setw(2) << std::setfill('0') << mins;
        return ss.str();
    }
}

#endif // TIME_UTILS_H 