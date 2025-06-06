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
        include/Components/Core/CompetitionEvent.h
        include/Components/Core/Athlete.h
        include/Components/Core/DataContainer.h
        include/Components/Core/Unit.h
        include/Components/Core/ScoreRule.h
        include/Components/Core/SystemSettings.h
        include/Components/Core/Registration.h
        include/Components/Core/Schedule.h
        include/Components/Core/Result.h
        include/Components/Core/RegistrationTransaction.h
        include/Components/Manager/DataManager.h
        include/Components/Manager/ScheduleManager.h
        include/Components/Manager/DataFileManager.h
        include/Components/Core/Constants.h
        include/Components/Manager/UnitManager.h
        include/Components/Manager/AthleteManager.h
        include/Components/Manager/CompetitionEventManager.h
        include/Components/Manager/ScoreRuleManager.h
        include/Components/Manager/EventResultsManager.h
        include/Components/Manager/WorkflowManager.h
        include/Components/Manager/SessionManager.h
        include/Components/Manager/VenueManager.h
        include/Components/Manager/ScheduleLockManager.h
        include/Components/Manager/SystemArgsManager.h
        src/Components/Core/Athlete.cpp
        src/Components/Core/CompetitionEvent.cpp
        src/Components/Core/Unit.cpp
        src/Components/Core/DataContainer.cpp
        src/Components/Core/ScoreRule.cpp
        src/Components/Core/SystemSettings.cpp
        src/Components/Core/Registration.cpp
        src/Components/Core/Schedule.cpp
        src/Components/Core/Result.cpp
        src/Components/Core/RegistrationTransaction.cpp
        src/Components/Manager/DataManager.cpp
        src/Components/Manager/DataFileManager.cpp
        src/Components/Core/AutoTest.cpp
        src/Components/Manager/UnitManager.cpp
        src/Components/Manager/AthleteManager.cpp
        src/Components/Manager/ScheduleManager.cpp
        src/Components/Manager/CompetitionEventManager.cpp
        src/Components/Manager/ScoreRuleManager.cpp
        src/Components/Manager/EventResultsManager.cpp
        src/Components/Manager/WorkflowManager.cpp
        src/Components/Manager/SessionManager.cpp
        src/Components/Manager/VenueManager.cpp
        src/Components/Manager/ScheduleLockManager.cpp
        src/Components/Manager/SystemArgsManager.cpp
        include/Components/Core/AutoTest.h
        src/UI/UIManager.cpp
        include/UI/UIManager.h
        src/UI/Controller/SystemSettingsController.cpp
        include/UI/Controller/SystemSettingsController.h
        src/UI/Controller/RegistrationController.cpp
        include/UI/Controller/RegistrationController.h
        src/UI/Controller/ScheduleController.cpp
        include/UI/Controller/ScheduleController.h
        src/UI/Controller/ResultsController.cpp
        include/UI/Controller/ResultsController.h
        src/UI/Controller/QueryController.cpp
        include/UI/Controller/QueryController.h
        src/UI/Controller/DataManagementController.cpp
        include/UI/Controller/DataManagementController.h
        include/utils.h)
