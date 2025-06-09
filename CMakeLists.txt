cmake_minimum_required(VERSION 3.10)
project(SchoolSportsCpp)

# 添加全局编译定义
add_definitions(-DNOMINMAX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加 include 目录到头文件搜索路径
include_directories(include)

# 添加源文件
set(SOURCES
    src/main.cpp
    src/SportsMeet.cpp
    src/Unit.cpp
    src/Event.cpp
    src/Athlete.cpp
    src/Schedule.cpp
)

# 添加可执行文件
add_executable(school_sports_app ${SOURCES})

# 如果未来有需要，可以链接其他库
# target_link_libraries(school_sports_app some_library) 