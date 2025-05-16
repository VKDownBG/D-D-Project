cmake_minimum_required(VERSION 3.20)
project(DANDD)

set(RAYLIB_DIR "C:/DandD/libs/raylib-5.0_win64_mingw-w64")
include_directories(${RAYLIB_DIR}/include)
link_directories(${RAYLIB_DIR}/lib)

add_executable(DANDD
        include/UI/managers/UIManager.h
        include/UI/screens/MainMenu.h
        include/UI/screens/GameHUD.h
        include/UI/screens/InventoryUI.h
        include/UI/widgets/Button.h
        include/UI/widgets/ProgressBar.h
        src/UI/managers/UIManager.cpp
        src/UI/screens/GameHUD.cpp
        src/UI/screens/InventoryUI.cpp
        src/UI/screens/MainMenu.cpp
        src/UI/widgets/Button.cpp
        src/UI/widgets/ProgressBar.cpp
        src/UI/test.cpp
        include/UI/widgets/Icon.h
        include/UI/panels/EquipmentPanel.h
        src/UI/panels/EquipmentPanel.cpp
        src/UI/widgets/Icon.cpp
        #        src/Entities/Hero/Hero.cpp
        #        src/Entities/Items/Item.cpp
        #        src/Entities/Hero/Inventory.cpp
        #        src/Entities/Items/Armor.cpp
        #        src/Entities/Items/Weapon.cpp
        #        src/Entities/Items/Spell.cpp
        #        src/Entities/Monster.cpp
        #        include/Core/MapSystem.h
        #        src/Core/MapSystem.cpp
        #        include/Core/GameManager.h
        #        include/Core/BattleSystem.h
        #        src/Core/GameManager.cpp
        #        "include/Utils/Position.h"
        #        src/Utils/Position.cpp
        #        include/Entities/Treasure.h
        #        src/Entities/Treasure.cpp
        #        include/Utils/ProbabilitySystem.h
        #        src/Utils/ProbabilitySystem.cpp
        #        src/Core/BattleSystem.cpp
        #        include/Utils/Attack.h
)

target_link_libraries(DANDD PRIVATE
        raylib
        opengl32
        gdi32
        winmm
)

add_custom_command(TARGET DANDD POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${RAYLIB_DIR}/lib/raylib.dll"
        $<TARGET_FILE_DIR:DANDD>
)

file(COPY assets/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

file(GLOB DLL_FILES "${RAYLIB_DIR}/lib/*.dll")
foreach (DLL_FILE ${DLL_FILES})
    add_custom_command(TARGET DANDD POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${DLL_FILE}"
            $<TARGET_FILE_DIR:DANDD>
    )
endforeach ()