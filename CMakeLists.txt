cmake_minimum_required(VERSION 3.10)
project(DANDD)

# Set SFML paths
set(SFML_DIR "C:/SFML/lib/cmake/SFML")
set(SFML_INCLUDE_DIR "C:/SFML/include")

# Find SFML with components (new 3.0 syntax)
find_package(SFML 3.0 COMPONENTS Graphics Window System REQUIRED)

# Add executable
add_executable(DANDD
        src/Entities/Hero.cpp
        src/Entities/Items/Item.cpp
        src/Entities/Items/Inventory.cpp
        src/Entities/Items/Armor.cpp
        src/Entities/Items/Weapon.cpp
        src/Entities/Items/Spell.cpp
        src/Entities/Monsters.cpp
)

# Link SFML (new 3.0 target names)
target_include_directories(DANDD PRIVATE include ${SFML_INCLUDE_DIR})
target_link_libraries(DANDD PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
)

# Copy DLLs and assets
add_custom_command(TARGET DANDD POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "C:/SFML/SFML-3.0.0/bin/"  # Updated path
        $<TARGET_FILE_DIR:DANDD>
)

file(COPY assets/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)