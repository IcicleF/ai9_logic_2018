add_library(sdk OBJECT ../src/include/sdk/actions.h ../src/include/sdk/actions.cpp ../src/include/sdk/defs.h ../src/include/sdk/literals.h ../src/include/sdk/randomizer.h ../src/include/sdk/randomizer.cpp ../src/include/sdk/router.h ../src/include/sdk/router.cpp ../src/include/sdk/sdk.h ../src/include/sdk/sdk.cpp ../src/include/sdk/vec2.h)
add_library(emptyAI SHARED emptyAI.cpp $<TARGET_OBJECTS:sdk>)
add_library(routerSpeedAI SHARED routerSpeedAI.cpp $<TARGET_OBJECTS:sdk>)
add_dependencies(api_test emptyAI routerSpeedAI)