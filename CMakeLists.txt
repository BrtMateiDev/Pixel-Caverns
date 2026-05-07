cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(PRODUCTION_BUILD OFF CACHE BOOL "Make this a production build")

if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build Raylib as static libraries" FORCE)

project("Pixel Caverns")

set(EXE_NAME "PixelCaverns")

add_subdirectory(thirdparty/raylib-6.0)
# add_subdirectory(thirdparty/imgui-docking)
# add_subdirectory(thirdparty/rlImGui)

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${EXE_NAME}
        src/gameLayer/player.h
        src/gameLayer/player.cpp
        src/gameLayer/storage.h
        src/gameLayer/blocks.cpp
        src/gameLayer/pickaxe.h
        src/gameLayer/pickaxe.cpp
        src/gameLayer/dialogue.h
)

# This is the magic line that makes the actual .exe have a space in its name!
set_target_properties(${EXE_NAME} PROPERTIES OUTPUT_NAME "Pixel Caverns")

set_property(TARGET ${EXE_NAME} PROPERTY CXX_STANDARD 17)

if (PRODUCTION_BUILD)
    target_compile_definitions(${EXE_NAME} PUBLIC RESOURCES_PATH="./resources/")
    target_compile_definitions(${EXE_NAME} PUBLIC PRODUCTION_BUILD=1)
else ()
    target_compile_definitions(${EXE_NAME} PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/")
    target_compile_definitions(${EXE_NAME} PUBLIC PRODUCTION_BUILD=0)
endif ()

target_sources(${EXE_NAME} PRIVATE ${MY_SOURCES})

if (MSVC) # Visual Studio
    target_compile_definitions(${EXE_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)
    target_compile_options(${EXE_NAME} PRIVATE /arch:AVX2)

    if (PRODUCTION_BUILD)
        set_target_properties(${EXE_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
    endif ()

elseif (MINGW) #MinGW to Windows
    target_compile_options(${EXE_NAME} PRIVATE -mno-avx -mstackrealign)

    #Including dlls in the exe to avoid dependencies
    target_link_options(${EXE_NAME} PRIVATE
            -static
            -static-libgcc
            -static-libstdc++
    )

    if (PRODUCTION_BUILD)
        target_link_options(${EXE_NAME} PRIVATE -mwindows)
    endif ()

elseif (APPLE OR UNIX) # Linux / macOS
    target_compile_options(${EXE_NAME} PRIVATE -mavx2 -mfma)
    if (PRODUCTION_BUILD)
        target_link_options(${EXE_NAME} PRIVATE -s)
    endif ()
endif ()

target_include_directories(${EXE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/platform")
target_include_directories(${EXE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/gameLayer")
target_include_directories(${EXE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/FastNoiseLite")

target_link_libraries(${EXE_NAME} PRIVATE raylib) # don't forget to add imgui and rlimgui back when needed
