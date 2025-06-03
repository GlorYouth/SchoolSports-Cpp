cmake_minimum_required(VERSION 3.31)
project(SchoolSports-Cpp)

set(CMAKE_CXX_STANDARD 26)

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
        include/Components/Constants.h
        src/Components/Athlete.cpp
        src/Components/CompetitionEvent.cpp
        src/Components/Unit.cpp
        src/Components/ScoreRule.cpp
        src/Components/SystemSettings.cpp
        src/Components/Registration.cpp
        src/Components/Schedule.cpp
        src/Components/Result.cpp
        src/Components/DataManager.cpp
        src/Components/AutoTest.cpp
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
        include/Controller/DataManagementController.h)
