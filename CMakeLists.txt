CMAKE_MINIMUM_REQUIRED(VERSION 3.20)
SET(CMAKE_CXX_STANDARD 20)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

PROJECT(SuperChip8)

SET(SuperChip8_SRC_FILES
    src/main.cpp
    src/schip8_error.cpp
    src/emulator/schip8_emulator_vm.cpp
    src/emulator/memory/schip8_emulator_memory_ram.cpp
    src/emulator/memory/schip8_emulator_memory_registers.cpp
    src/system/audio/schip8_system_audio_audiodevice.cpp
    src/system/graphics/schip8_system_graphics_display.cpp
    src/system/input/schip8_system_input_keyboard.cpp
)

SET(SuperChip8_INCLUDE_DIRS
    src/
    src/emulator/
    src/emulator/memory/
    src/system/audio/
    src/system/graphics/
    src/system/input/
)

# header only external libraries
SET(EXTERNAL_INCLUDE_DIRS
    external/
)

INCLUDE_DIRECTORIES(
    ${SuperChip8_INCLUDE_DIRS}
    ${EXTERNAL_INCLUDE_DIRS}
)

ADD_EXECUTABLE(${PROJECT_NAME} ${SuperChip8_SRC_FILES})

# raylib dependencies
FIND_PACKAGE(raylib REQUIRED)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} raylib)

# Make the CMAKE INSTALL prefix accessible in the cpp file
# cmake -DDEV_MODE=ON .. => to unable dev mode
# cmake -DDEV_MODE=OFF .. => to disable it
option(DEV_MODE "Build in development mode" ON)
if(DEV_MODE)
    set(CMAKE_BUILD_TYPE Debug) # To enable debugging symbols (gdb)
    ADD_COMPILE_DEFINITIONS(SOUND_FILE_PATH="${CMAKE_SOURCE_DIR}/resources")
else()
    set(CMAKE_BUILD_TYPE Release) # To enable optimizations
    ADD_COMPILE_DEFINITIONS(SOUND_FILE_PATH="${CMAKE_INSTALL_PREFIX}/share/SuperChip8/resources")
endif()

# install rules
# run: cmake -DDEV_MODE=OFF ..
# then run: make && make install
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/resources/beep.wav DESTINATION share/SuperChip8/resources)

