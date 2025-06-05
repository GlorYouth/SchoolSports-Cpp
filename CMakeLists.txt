cmake_minimum_required(VERSION 3.31)
project(SchoolSports-Cpp)
# 为 std::views 和 std::optional 设置 C++20 标准
set(CMAKE_CXX_STANDARD 20)
# 要求必须支持所设置的 C++ 标准，否则报错
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 导出编译命令，方便使用 clangd 等工具
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# 禁用 GNU 特有的编译器扩展，提倡使用纯粹的 C++ 标准特性，这是一个好习惯
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(SchoolSports-Cpp main.cpp
        include/Components/CompetitionEvent.h
        include/Components/Athlete.h
        include/Components/Unit.h
        include/Components/ScoreRule.h
        include/Components/SystemSettings.h
        include/Components/Registration.h
        include/Components/Schedule.h
        include/Components/Result.h
        include/Components/DataManager.h
        include/Components/DataFileManager.h
        include/Components/Constants.h
        include/Components/UnitManager.h
        include/Components/AthleteManager.h
        include/Components/CompetitionEventManager.h
        src/Components/Athlete.cpp
        src/Components/CompetitionEvent.cpp
        src/Components/Unit.cpp
        src/Components/ScoreRule.cpp
        src/Components/SystemSettings.cpp
        src/Components/Registration.cpp
        src/Components/Schedule.cpp
        src/Components/Result.cpp
        src/Components/DataManager.cpp
        src/Components/DataFileManager.cpp
        src/Components/AutoTest.cpp
        src/Components/UnitManager.cpp
        src/Components/AthleteManager.cpp
        src/Components/CompetitionEventManager.cpp
        include/Components/AutoTest.h
        src/UI/UIManager.cpp
        include/UI/UIManager.h
        src/Controller/SystemSettingsController.cpp
        include/Controller/SystemSettingsController.h
        src/Controller/RegistrationController.cpp
        include/Controller/RegistrationController.h
        src/Controller/ScheduleController.cpp
        include/Controller/ScheduleController.h
        src/Controller/ResultsController.cpp
        include/Controller/ResultsController.h
        src/Controller/QueryController.cpp
        include/Controller/QueryController.h
        src/Controller/DataManagementController.cpp
        include/Controller/DataManagementController.h
        include/utils.h)
