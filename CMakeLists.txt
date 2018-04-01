cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(ai9_logic_2018)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

include_directories(src)
include_directories(src/include)

if(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake)")
    add_definitions(/W1)
endif()

if(APPLE)
  set(CMAKE_INSTALL_NAME_DIR @executable_path)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
endif(APPLE)

aux_source_directory(src SOURCES)

add_library(lib_ai9_logic_2018 OBJECT src/json/json.h src/json/json-forwards.h src/jsoncpp.cpp src/miniz/miniz.h src/miniz/miniz.c src/include/sdk/strategy/actionmaker.h src/include/sdk/strategy/actionmaker.cpp src/include/sdk/strategy/escape_strategy.h src/include/sdk/strategy/escape_strategy.cpp src/include/sdk/strategy/focus_attack_strategy.h src/include/sdk/strategy/focus_attack_strategy.cpp src/include/sdk/strategy/patrol_strategy.h src/include/sdk/strategy/patrol_strategy.cpp src/include/sdk/strategy/randomized_attacker_strategy.h src/include/sdk/strategy/randomized_attacker_strategy.cpp src/include/sdk/strategy/stealth_strategy.h src/include/sdk/strategy/stealth_strategy.cpp src/include/sdk/strategy/strategy.h src/include/sdk/strategy/tracer_strategy.h src/include/sdk/strategy/tracer_strategy.cpp src/include/sdk/actions.cpp src/include/sdk/actions.h src/include/sdk/defs.h src/include/sdk/literals.h src/include/sdk/randomizer.h src/include/sdk/randomizer.cpp src/include/sdk/router.cpp src/include/sdk/router.h src/include/sdk/sdk.h src/include/sdk/sdk.cpp src/include/sdk/vec2.h src/include/ui/uiapi.h src/include/ui/uinterface.h src/include/dllinterface.h src/include/idmgr.h src/include/logic.h src/include/mapinfo.h src/include/idmgr.h src/idmgr.cpp src/logic.cpp src/mapinfo.cpp src/uiapi.cpp src/uinterface.cpp src/unit.cpp)
add_library(lib_logic SHARED $<TARGET_OBJECTS:lib_ai9_logic_2018>)

add_executable(api_test dlltest/dlltest_main.cpp)
add_executable(judger judger/judger.cpp)
target_link_libraries(api_test lib_logic)
target_link_libraries(judger lib_logic)

add_subdirectory(ai)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
else()
    target_link_libraries(api_test dl pthread)
    target_link_libraries(judger dl pthread)
endif()
