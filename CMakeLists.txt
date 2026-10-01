cmake_minimum_required(VERSION 3.24)

project(KaosRestoration VERSION 0.1.0 LANGUAGES CXX)

if(WIN32 AND CMAKE_SIZEOF_VOID_P LESS 8)
    message(FATAL_ERROR
        "KAOS requires a 64-bit Windows toolchain (x64). "
        "Select the x64 architecture, e.g. cmake -A x64.")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL3 CONFIG REQUIRED)
find_package(Threads REQUIRED)

set(KAOS_VIDEO_SOURCES
    crcio.cpp
    errorstr.cpp
    fastmem.cpp
    fixed.cpp
    kaos_sdl.cpp
    lfont2.cpp
    lgraph.cpp
    lmouse.cpp
    mm4.cpp
    std.cpp
    vgatool3.cpp
    xms.cpp
)

set(KAOS_GAME_SOURCES
    ${KAOS_VIDEO_SOURCES}
    doors.cpp
    events.cpp
    infobar.cpp
    kaos.cpp
    platform_main.cpp
    key.cpp
    objects.cpp
    objects2.cpp
    player2.cpp
    strlist.cpp
    3dengin5.cpp
    3dsdriv.cpp
    3dsound.cpp
)

function(kaos_configure_target target)
    target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${target} PRIVATE SDL3::SDL3)
    target_compile_definitions(${target} PRIVATE SDL_MAIN_HANDLED=1)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive- /EHsc /utf-8)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

if(WIN32)
    add_executable(kaos WIN32 ${KAOS_GAME_SOURCES})
else()
    add_executable(kaos ${KAOS_GAME_SOURCES})
endif()
set_property(TARGET kaos PROPERTY FOLDER "Game")
kaos_configure_target(kaos)
target_link_libraries(kaos PRIVATE Threads::Threads)
target_compile_definitions(kaos PRIVATE __DATAFILE__ KAOS_GAME=1)

set(KAOS_EDITOR_SOURCES
    editor.cpp
    platform_editor_main.cpp
    win32_editor_platform.cpp
    crcio.cpp
    errorstr.cpp
    fastmem.cpp
    fixed.cpp
    lfont2.cpp
    lgraph.cpp
    lmouse.cpp
    mm4.cpp
    std.cpp
    vgatool3.cpp
)
if(WIN32)
    add_executable(editor WIN32 ${KAOS_EDITOR_SOURCES})
else()
    add_executable(editor ${KAOS_EDITOR_SOURCES})
endif()
set_property(TARGET editor PROPERTY FOLDER "Editor")
target_include_directories(editor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(editor PRIVATE KAOS_EDITOR=1 NOMINMAX WIN32_LEAN_AND_MEAN)
if(WIN32)
    set_target_properties(editor PROPERTIES
        WIN32_EXECUTABLE TRUE
        VS_GLOBAL_UseDebugLibraries "$<IF:$<CONFIG:Debug>,true,false>"
    )
endif()
if(MSVC)
    target_compile_options(editor PRIVATE /W4 /permissive- /EHsc /utf-8)
else()
    target_compile_options(editor PRIVATE -Wall -Wextra -Wpedantic)
endif()
if(WIN32)
    target_link_libraries(editor PRIVATE user32 gdi32)
endif()

add_executable(ftest ftest.cpp)
set_property(TARGET ftest PROPERTY FOLDER "Tools")
target_include_directories(ftest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
if(MSVC)
    target_compile_options(ftest PRIVATE /W4 /permissive- /EHsc /utf-8)
else()
    target_compile_options(ftest PRIVATE -Wall -Wextra -Wpedantic)
endif()

foreach(tool IN ITEMS makefont makebdf setfirm)
    if(tool STREQUAL "makefont")
        set(tool_sources makefont.cpp tool_video_page.cpp ${KAOS_VIDEO_SOURCES})
    elseif(tool STREQUAL "makebdf")
        set(tool_sources makebdf.cpp strlist.cpp ${KAOS_VIDEO_SOURCES})
    else()
        set(tool_sources setfirm.cpp crcio.cpp errorstr.cpp fastmem.cpp mm4.cpp std.cpp xms.cpp kaos_sdl.cpp)
    endif()
    add_executable(${tool} ${tool_sources})
    set_property(TARGET ${tool} PROPERTY FOLDER "Tools")
    kaos_configure_target(${tool})
    if(tool STREQUAL "makebdf")
        target_compile_definitions(${tool} PRIVATE __MAKEDATA__)
    endif()
endforeach()

add_executable(testmid testmid.cpp std.cpp kaos_sdl.cpp)
set_property(TARGET testmid PROPERTY FOLDER "Tools")
kaos_configure_target(testmid)
