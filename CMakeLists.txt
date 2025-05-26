cmake_minimum_required(VERSION 3.20)
project(DANDD)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Raylib configuration
set(RAYLIB_DIR "C:/DandD/libs/raylib-5.0_win64_mingw-w64")
find_library(RAYLIB_LIBRARY raylib PATHS "${RAYLIB_DIR}/lib" REQUIRED)

# Include directories
include_directories(
        ${RAYLIB_DIR}/include
        src/Entities
        src/Items
        src/Items/Types
        src/UI
        src/UI/screens
        src/UI/widgets
        src/Utils
)

# Add executable with all source files
add_executable(DANDD
        #src/Core/BattleSystem.cpp
        #src/Core/GameManager.cpp
        src/Core/MapSystem.cpp
        src/Entities/Hero.cpp
        src/Entities/Monster.cpp
        src/Entities/Treasure.cpp
        src/Items/Item.cpp
        src/Items/Types/Armor.cpp
        src/Items/Types/Weapon.cpp
        src/Items/Types/Spell.cpp
        #src/UI/managers/UIManager.cpp
        src/UI/panels/BattlePanel.cpp
        src/UI/panels/EquipmentPanel.cpp
        src/UI/renderers/MapRenderer.cpp
        src/UI/screens/GameHUD.cpp
        src/UI/screens/MainMenu.cpp
        src/UI/widgets/Button.cpp
        src/UI/widgets/ProgressBar.cpp
        src/UI/test.cpp
        src/Utils/Attack.cpp
        src/Utils/Inventory.cpp
        src/Utils/Position.cpp
        src/Utils/ProbabilitySystem.cpp
        include/UI/panels/LevelUpPanel.h
        src/UI/panels/LevelUpPanel.cpp
)

# Link libraries
target_link_libraries(DANDD PRIVATE
        ${RAYLIB_LIBRARY}
        opengl32
        gdi32
        winmm
)

# Copy DLLs and assets
add_custom_command(TARGET DANDD POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${RAYLIB_DIR}/lib/raylib.dll"
        $<TARGET_FILE_DIR:DANDD>
)

file(COPY assets/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)

# Additional debug flags
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
endif ()