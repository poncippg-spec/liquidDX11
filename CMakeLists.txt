cmake_minimum_required(VERSION 3.20)

project(liquidDX11 LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "liquidDX11 is Windows-only. It uses Win32, DXGI Desktop Duplication, Direct3D 11, and HLSL.")
endif()

if(NOT CMAKE_SIZEOF_VOID_P STREQUAL "8")
    message(FATAL_ERROR "liquidDX11 is x64-only because the bundled FreeType import library is x64.")
endif()

set(LIQUIDDX11_APP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/examples/example_win32_directx11")

set(LIQUIDDX11_SOURCES
    imgui.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    misc/freetype/imgui_freetype.cpp
    backends/imgui_impl_dx11.cpp
    backends/imgui_impl_win32.cpp
    examples/example_win32_directx11/src/glass/backdrop.cpp
    examples/example_win32_directx11/src/glass/glass.cpp
    examples/example_win32_directx11/src/glass/surfaces.cpp
    examples/example_win32_directx11/src/main.cpp
)

set(LIQUIDDX11_HEADERS
    imconfig.h
    imgui.h
    imgui_internal.h
    backends/imgui_impl_dx11.h
    backends/imgui_impl_win32.h
    misc/freetype/imgui_freetype.h
    examples/example_win32_directx11/src/glass/backdrop.h
    examples/example_win32_directx11/src/glass/glass.h
    examples/example_win32_directx11/src/glass/shaders.h
    examples/example_win32_directx11/src/glass/spring.h
    examples/example_win32_directx11/src/glass/surfaces.h
)

set(LIQUIDDX11_INLINE_FILES
    examples/example_win32_directx11/src/app/config.inc
    examples/example_win32_directx11/src/app/config_apply.inc
    examples/example_win32_directx11/src/app/megasettings.inc
    examples/example_win32_directx11/src/app/pages.inc
    examples/example_win32_directx11/src/app/render_capture.inc
    examples/example_win32_directx11/src/app/ui_state.inc
    examples/example_win32_directx11/src/app/config/config_io.inc
    examples/example_win32_directx11/src/app/config/config_parse.inc
    examples/example_win32_directx11/src/app/config/glass_config.inc
    examples/example_win32_directx11/src/app/pages/account.inc
    examples/example_win32_directx11/src/app/pages/battery.inc
    examples/example_win32_directx11/src/app/pages/customize.inc
    examples/example_win32_directx11/src/app/pages/display.inc
    examples/example_win32_directx11/src/app/pages/general.inc
    examples/example_win32_directx11/src/app/pages/launchpad.inc
    examples/example_win32_directx11/src/app/pages/profile.inc
    examples/example_win32_directx11/src/app/pages/storage.inc
    examples/example_win32_directx11/src/app/pages/surface.inc
    examples/example_win32_directx11/src/app/render/device_d3d11.inc
    examples/example_win32_directx11/src/app/render/recorder_mp4.inc
    examples/example_win32_directx11/src/app/render/ssaa_resolve.inc
    examples/example_win32_directx11/src/glass/surfaces/part1.inc
    examples/example_win32_directx11/src/glass/surfaces/part2.inc
    examples/example_win32_directx11/src/glass/surfaces/part3.inc
    examples/example_win32_directx11/src/glass/surfaces/part4.inc
    examples/example_win32_directx11/src/glass/surfaces/part5.inc
    examples/example_win32_directx11/src/glass/surfaces/part6.inc
    examples/example_win32_directx11/src/glass/widgets/audio.inc
    examples/example_win32_directx11/src/glass/widgets/chrome.inc
    examples/example_win32_directx11/src/glass/widgets/fields_color.inc
    examples/example_win32_directx11/src/glass/widgets/modals.inc
    examples/example_win32_directx11/src/glass/widgets/widgets_core.inc
    examples/example_win32_directx11/src/glass/widgets/widgets_extra.inc
    examples/example_win32_directx11/src/glass/widgets/widgets_input.inc
    examples/example_win32_directx11/src/glass/widgets/widgets_misc.inc
)

set_source_files_properties(${LIQUIDDX11_INLINE_FILES} PROPERTIES HEADER_FILE_ONLY ON)

add_executable(liquidDX11
    ${LIQUIDDX11_SOURCES}
    ${LIQUIDDX11_HEADERS}
    ${LIQUIDDX11_INLINE_FILES}
)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT liquidDX11)

target_compile_features(liquidDX11 PRIVATE cxx_std_17)

target_include_directories(liquidDX11 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/backends"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/freetype/include"
    "${LIQUIDDX11_APP_DIR}/src"
)

target_compile_definitions(liquidDX11 PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    UNICODE
    _UNICODE
)

if(MSVC)
    target_compile_options(liquidDX11 PRIVATE
        /W4
        /utf-8
        $<$<CONFIG:Release>:/GS->
    )
endif()

target_link_libraries(liquidDX11 PRIVATE
    d3d11
    d3dcompiler
    dxgi
    dwmapi
    windowscodecs
    ole32
    winmm
    user32
    gdi32
    shell32
    comdlg32
    advapi32
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/freetype/win64/freetype.lib"
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES
    ${LIQUIDDX11_SOURCES}
    ${LIQUIDDX11_HEADERS}
    ${LIQUIDDX11_INLINE_FILES}
)
