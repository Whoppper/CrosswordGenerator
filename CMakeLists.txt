cmake_minimum_required(VERSION 3.10)
project(crossword VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building in Debug mode")
    add_compile_options(
        -Wall 
        -Wextra
        -Wpedantic
        -g
        -O0 
        -D_GLIBCXX_DEBUG
    )
    add_link_options(
        -fno-omit-frame-pointer 
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Building in Release mode")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -O3
        -DNDEBUG
    )
    add_link_options(
        -s 
    )
else()
    message(STATUS "Building in default mode (Release-like)")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -O2
    )
endif()


enable_testing()

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

find_package(Qt5 COMPONENTS Core Gui Widgets Sql REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


set(SOURCE_FILES
    
    resources.qrc
    src/databasemanager.cpp 
    src/logger.cpp
    src/crosswordmanager.cpp
    src/crosswordcell.cpp
    src/wordtree.cpp
    src/gridworker.cpp
    src/gridgenerator.cpp
    src/backtrackingalgorithm.cpp
    src/leastwordcountselection.cpp
    src/gameinterface.cpp
    src/gridgenerationinterface.cpp
    src/mainwindow.cpp
    src/solvingalgorithmfactory.cpp
    src/wordselectionfactory.cpp

    include/logger.hpp 
    include/databasemanager.hpp
    include/crosswordmanager.hpp
    include/crosswordcell.hpp
    include/wordtree.hpp
    include/gridworker.hpp
    include/gridgenerator.hpp
    include/cell.hpp
    include/iwordselectionstrategy.hpp
    include/isolvingalgorithmstrategy.hpp
    include/backtrackingalgorithm.hpp
    include/leastwordcountselection.hpp
    include/gameinterface.hpp
    include/gridgenerationinterface.hpp
    include/mainwindow.hpp
    include/solvingalgorithmfactory.hpp
    include/wordselectionfactory.hpp
)

add_executable(crossword src/main.cpp ${SOURCE_FILES})
target_include_directories(crossword PRIVATE include)
target_link_libraries(crossword Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Sql)


#set(TEST_FILES test/crossword.cpp src/crossword.cpp )

#add_executable(crosswordTests ${SOURCE_FILES})
#target_include_directories(crosswordTests PRIVATE include)
#target_link_libraries(crosswordTests PRIVATE GTest::GTest GTest::Main pthread Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Sql

#add_test(NAME crosswordTests COMMAND crosswordTests)
