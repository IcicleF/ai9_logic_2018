cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(ai9_logic_2018_playerai)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

include_directories(sdk)

if(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake)")
    add_definitions(/W1)
endif()

if(APPLE)
  set(CMAKE_INSTALL_NAME_DIR @executable_path)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
endif(APPLE)

add_library(sdk OBJECT sdk/strategy/actionmaker.h sdk/strategy/actionmaker.cpp sdk/strategy/escape_strategy.h sdk/strategy/escape_strategy.cpp sdk/strategy/focus_attack_strategy.h sdk/strategy/focus_attack_strategy.cpp sdk/strategy/patrol_strategy.h sdk/strategy/patrol_strategy.cpp sdk/strategy/randomized_attacker_strategy.h sdk/strategy/randomized_attacker_strategy.cpp sdk/strategy/stealth_strategy.h sdk/strategy/stealth_strategy.cpp sdk/strategy/strategy.h sdk/strategy/tracer_strategy.h sdk/strategy/tracer_strategy.cpp sdk/actions.h sdk/actions.cpp sdk/defs.h sdk/literals.h sdk/randomizer.h sdk/randomizer.cpp sdk/router.h sdk/router.cpp sdk/sdk.h sdk/sdk.cpp sdk/vec2.h sdk/strategy/blind_item_strategy.h sdk/strategy/blind_item_strategy.cpp)

add_library(playerAI SHARED playerAI.cpp $<TARGET_OBJECTS:sdk>)