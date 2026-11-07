cmake_minimum_required(VERSION 3.21)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(ConcreteMixDesign_Mocks LANGUAGES CXX)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

if(WIN32)
    enable_language(RC)
endif()

find_package(Qt6 QUIET COMPONENTS LinguistTools)

if(WIN32)
    if(MSVC)
        set(MOCKS_DETECTED_TOOLCHAIN_SUFFIX "msvc")
    elseif(MINGW)
        set(MOCKS_DETECTED_TOOLCHAIN_SUFFIX "mingw")
    elseif(DEFINED CONCRETE_MIX_DESIGN_BUILD_TOOLCHAIN_SUFFIX)
        set(MOCKS_DETECTED_TOOLCHAIN_SUFFIX "${CONCRETE_MIX_DESIGN_BUILD_TOOLCHAIN_SUFFIX}")
    else()
        set(MOCKS_DETECTED_TOOLCHAIN_SUFFIX "windows")
    endif()

    set(MOCKS_DEFAULT_EXPORT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../build/build_Mocks_${MOCKS_DETECTED_TOOLCHAIN_SUFFIX}/export")
else()
    set(MOCKS_DEFAULT_EXPORT_DIR "")
endif()

if(WIN32)
    set(MOCKS_WINDOWS_ICON_NAME "concrete_mix_design.ico")
    set(MOCKS_WINDOWS_ICON "${CMAKE_CURRENT_SOURCE_DIR}/icons/${MOCKS_WINDOWS_ICON_NAME}")
    if(NOT EXISTS "${MOCKS_WINDOWS_ICON}")
        set(MOCKS_WINDOWS_ICON "${CMAKE_CURRENT_SOURCE_DIR}/../source/assets/icons/${MOCKS_WINDOWS_ICON_NAME}")
    endif()
endif()

set(
    MOCKS_EXPORT_DIR
    "${MOCKS_DEFAULT_EXPORT_DIR}"
    CACHE PATH
    "Diretorio que recebe o runtime implantado dos mocks"
)

if(MOCKS_EXPORT_DIR)
    file(TO_CMAKE_PATH "${MOCKS_EXPORT_DIR}" MOCKS_EXPORT_DIR_NORMALIZED)
else()
    set(MOCKS_EXPORT_DIR_NORMALIZED "")
endif()

if(WIN32 AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/icons")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/icons" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
endif()

function(mocks_add_windows_icon TARGET_NAME)
    if(NOT WIN32)
        return()
    endif()

    if(NOT EXISTS "${MOCKS_WINDOWS_ICON}")
        message(WARNING "Icone do mock nao encontrado em ${MOCKS_WINDOWS_ICON}. ${TARGET_NAME} sera compilado sem icone.")
        return()
    endif()

    file(TO_NATIVE_PATH "${MOCKS_WINDOWS_ICON}" MOCKS_WINDOWS_ICON_NATIVE)
    string(REPLACE "\\" "\\\\" MOCKS_WINDOWS_ICON_ESCAPED "${MOCKS_WINDOWS_ICON_NATIVE}")
    set(RC_FILE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}_app_icon.rc")
    file(WRITE "${RC_FILE}" "IDI_ICON1 ICON DISCARDABLE \"${MOCKS_WINDOWS_ICON_ESCAPED}\"\n")
    target_sources(
        "${TARGET_NAME}"
        PRIVATE
            "${RC_FILE}"
    )
endfunction()

function(mocks_apply_common_options TARGET_NAME)
    set_target_properties(
        "${TARGET_NAME}"
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    )

    target_include_directories(
        "${TARGET_NAME}"
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}"
            "${CMAKE_CURRENT_SOURCE_DIR}/.."
    )

    target_compile_definitions(
        "${TARGET_NAME}"
        PRIVATE
            $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
            $<$<CONFIG:Release>:QT_NO_INFO_OUTPUT>
            $<$<CONFIG:RelWithDebInfo>:QT_NO_DEBUG_OUTPUT>
            $<$<CONFIG:RelWithDebInfo>:QT_NO_INFO_OUTPUT>
    )

    if(MSVC)
        set_property(TARGET "${TARGET_NAME}" PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
        target_compile_options(
            "${TARGET_NAME}"
            PRIVATE
                $<$<CONFIG:Release>:/O2>
                $<$<CONFIG:Release>:/Oi>
                $<$<CONFIG:Release>:/Ot>
                $<$<CONFIG:Release>:/fp:fast>
                $<$<CONFIG:RelWithDebInfo>:/O2>
                $<$<CONFIG:RelWithDebInfo>:/Oi>
                $<$<CONFIG:RelWithDebInfo>:/Ot>
                $<$<CONFIG:RelWithDebInfo>:/fp:fast>
        )
    elseif(MINGW)
        target_compile_options(
            "${TARGET_NAME}"
            PRIVATE
                $<$<CONFIG:Release>:-O3>
                $<$<CONFIG:Release>:-ffunction-sections>
                $<$<CONFIG:Release>:-fdata-sections>
                $<$<CONFIG:Release>:-fomit-frame-pointer>
                $<$<CONFIG:RelWithDebInfo>:-O3>
                $<$<CONFIG:RelWithDebInfo>:-ffunction-sections>
                $<$<CONFIG:RelWithDebInfo>:-fdata-sections>
                $<$<CONFIG:RelWithDebInfo>:-fomit-frame-pointer>
        )
        target_link_options(
            "${TARGET_NAME}"
            PRIVATE
                $<$<CONFIG:Release>:-Wl,--gc-sections>
                $<$<CONFIG:Release>:-Wl,-O1>
                $<$<CONFIG:RelWithDebInfo>:-Wl,--gc-sections>
                $<$<CONFIG:RelWithDebInfo>:-Wl,-O1>
        )
    endif()
endfunction()

function(mocks_add_deploy_target TARGET_NAME)
    if(NOT WIN32)
        return()
    endif()

    get_filename_component(MOCKS_QT_PREFIX_DIR "${Qt6_DIR}/../../.." ABSOLUTE)
    set(MOCKS_WINDEPLOYQT_EXECUTABLE "${MOCKS_QT_PREFIX_DIR}/bin/windeployqt.exe")
    if(NOT EXISTS "${MOCKS_WINDEPLOYQT_EXECUTABLE}")
        message(WARNING "windeployqt.exe nao encontrado. ${TARGET_NAME} nao sera implantado automaticamente.")
        return()
    endif()

    set(MOCKS_EXTRA_DLLS
        "Qt6Charts.dll"
        "Qt6OpenGL.dll"
        "Qt6OpenGLWidgets.dll"
        "Qt6Sql.dll"
    )
    if(MINGW)
        list(APPEND MOCKS_EXTRA_DLLS
            "libgcc_s_seh-1.dll"
            "libstdc++-6.dll"
            "libwinpthread-1.dll"
        )
    endif()

    set(DEPLOY_COMMANDS
        COMMAND "${MOCKS_WINDEPLOYQT_EXECUTABLE}"
            --verbose 0
            --compiler-runtime
            --no-translations
            --no-system-d3d-compiler
            --no-opengl-sw
            "$<TARGET_FILE:${TARGET_NAME}>"
    )

    foreach(EXTRA_DLL IN LISTS MOCKS_EXTRA_DLLS)
        if(EXISTS "${MOCKS_QT_PREFIX_DIR}/bin/${EXTRA_DLL}")
            list(APPEND DEPLOY_COMMANDS
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                    "${MOCKS_QT_PREFIX_DIR}/bin/${EXTRA_DLL}"
                    "$<TARGET_FILE_DIR:${TARGET_NAME}>/"
            )
        endif()
    endforeach()

    if(MOCKS_EXPORT_DIR_NORMALIZED)
        list(APPEND DEPLOY_COMMANDS
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${MOCKS_EXPORT_DIR_NORMALIZED}"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${MOCKS_EXPORT_DIR_NORMALIZED}/icons"
            COMMAND "${CMAKE_COMMAND}" -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/icons"
                "${MOCKS_EXPORT_DIR_NORMALIZED}/icons"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "$<TARGET_FILE:${TARGET_NAME}>"
                "${MOCKS_EXPORT_DIR_NORMALIZED}/"
            COMMAND "${MOCKS_WINDEPLOYQT_EXECUTABLE}"
                --verbose 0
                --compiler-runtime
                --no-translations
                --no-system-d3d-compiler
                --no-opengl-sw
                "${MOCKS_EXPORT_DIR_NORMALIZED}/$<TARGET_FILE_NAME:${TARGET_NAME}>"
        )
        foreach(EXTRA_DLL IN LISTS MOCKS_EXTRA_DLLS)
            if(EXISTS "${MOCKS_QT_PREFIX_DIR}/bin/${EXTRA_DLL}")
                list(APPEND DEPLOY_COMMANDS
                    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                        "${MOCKS_QT_PREFIX_DIR}/bin/${EXTRA_DLL}"
                        "${MOCKS_EXPORT_DIR_NORMALIZED}/"
                )
            endif()
        endforeach()
    endif()

    add_custom_target(
        "${TARGET_NAME}Deploy"
        ALL
        DEPENDS "${TARGET_NAME}"
        ${DEPLOY_COMMANDS}
        COMMENT "Deploying ${TARGET_NAME} runtime"
        VERBATIM
    )

    if(MOCKS_LAST_DEPLOY_TARGET)
        add_dependencies("${TARGET_NAME}Deploy" "${MOCKS_LAST_DEPLOY_TARGET}")
    endif()
    set(MOCKS_LAST_DEPLOY_TARGET "${TARGET_NAME}Deploy" PARENT_SCOPE)
endfunction()

function(mocks_add_gui_run_target TARGET_NAME TOOL_TARGET)
    if(NOT WIN32)
        return()
    endif()

    add_custom_target(
        "${TARGET_NAME}"
        DEPENDS "${TOOL_TARGET}Deploy"
        COMMAND "${CMAKE_COMMAND}" -E env
            "QT_PLUGIN_PATH=$<TARGET_FILE_DIR:${TOOL_TARGET}>"
            "QT_QPA_PLATFORM_PLUGIN_PATH=$<TARGET_FILE_DIR:${TOOL_TARGET}>/platforms"
            cmd /c start "" /b "$<TARGET_FILE:${TOOL_TARGET}>" ${ARGN}
        VERBATIM
        USES_TERMINAL
    )
endfunction()

function(mocks_add_terminal_run_target TARGET_NAME TOOL_TARGET)
    if(NOT WIN32)
        return()
    endif()

    add_custom_target(
        "${TARGET_NAME}"
        DEPENDS "${TOOL_TARGET}Deploy"
        COMMAND "${CMAKE_COMMAND}" -E chdir "$<TARGET_FILE_DIR:${TOOL_TARGET}>"
            "${CMAKE_COMMAND}" -E env
            "QT_PLUGIN_PATH=$<TARGET_FILE_DIR:${TOOL_TARGET}>"
            "QT_QPA_PLATFORM_PLUGIN_PATH=$<TARGET_FILE_DIR:${TOOL_TARGET}>/platforms"
            "$<TARGET_FILE:${TOOL_TARGET}>"
            ${ARGN}
        VERBATIM
        USES_TERMINAL
    )
endfunction()

# ----------------------------------------------------------------------------
# 1. CompileTranslationsGUI
# ----------------------------------------------------------------------------
add_executable(
    CompileTranslationsGUI
    WIN32
    CompileTranslationsGUI.cpp
    CompileTranslationsGUI.hpp
)
target_link_libraries(CompileTranslationsGUI PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
mocks_add_windows_icon(CompileTranslationsGUI)
mocks_apply_common_options(CompileTranslationsGUI)
mocks_add_deploy_target(CompileTranslationsGUI)

# ----------------------------------------------------------------------------
# 2. CompileTranslationsTerminal
# ----------------------------------------------------------------------------
add_executable(
    CompileTranslationsTerminal
    CompileTranslationsTerminal.cpp
    CompileTranslationsTerminal.hpp
)
target_link_libraries(CompileTranslationsTerminal PRIVATE Qt6::Core)
if(WIN32)
    target_link_libraries(CompileTranslationsTerminal PRIVATE User32)
endif()
mocks_add_windows_icon(CompileTranslationsTerminal)
mocks_apply_common_options(CompileTranslationsTerminal)
mocks_add_deploy_target(CompileTranslationsTerminal)

# ----------------------------------------------------------------------------
# 3. find_VersionEditor
# ----------------------------------------------------------------------------
add_executable(
    find_VersionEditor
    WIN32
    find_VersionEditor.cpp
    find_VersionEditor.hpp
)
target_link_libraries(find_VersionEditor PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
target_compile_features(find_VersionEditor PRIVATE cxx_std_20)
mocks_add_windows_icon(find_VersionEditor)
mocks_apply_common_options(find_VersionEditor)
mocks_add_deploy_target(find_VersionEditor)

# ----------------------------------------------------------------------------
# 4. Alvo Unificado de Deploy e Execução dos Mocks
# ----------------------------------------------------------------------------
add_custom_target(
    MocksDeploy
    ALL
    DEPENDS
        CompileTranslationsGUIDeploy
        CompileTranslationsTerminalDeploy
        find_VersionEditorDeploy
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/icons"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/icons"
        "${CMAKE_CURRENT_BINARY_DIR}/icons"
    COMMENT "Deploying all Concrete Mix Designer Mocks runtimes"
)

if(WIN32)
    set(MOCKS_DETACHED_GUI_LAUNCHER "${CMAKE_CURRENT_SOURCE_DIR}/launch_detached_gui.cmake")

    add_custom_target(
        MocksRun
        DEPENDS MocksDeploy
        COMMAND "${CMAKE_COMMAND}" -E env
            "QT_PLUGIN_PATH=$<TARGET_FILE_DIR:find_VersionEditor>"
            "QT_QPA_PLATFORM_PLUGIN_PATH=$<TARGET_FILE_DIR:find_VersionEditor>/platforms"
            "${CMAKE_COMMAND}"
            -DLAUNCH_FILE=$<TARGET_FILE:find_VersionEditor>
            -DLAUNCH_WORKING_DIR=$<TARGET_FILE_DIR:find_VersionEditor>
            -P "${MOCKS_DETACHED_GUI_LAUNCHER}"
        COMMAND "${CMAKE_COMMAND}" -E env
            "QT_PLUGIN_PATH=$<TARGET_FILE_DIR:CompileTranslationsGUI>"
            "QT_QPA_PLATFORM_PLUGIN_PATH=$<TARGET_FILE_DIR:CompileTranslationsGUI>/platforms"
            "${CMAKE_COMMAND}"
            -DLAUNCH_FILE=$<TARGET_FILE:CompileTranslationsGUI>
            -DLAUNCH_WORKING_DIR=$<TARGET_FILE_DIR:CompileTranslationsGUI>
            -P "${MOCKS_DETACHED_GUI_LAUNCHER}"
        COMMENT "Launching mocks runtime tools"
        VERBATIM
        USES_TERMINAL
    )

    mocks_add_gui_run_target(find_VersionEditorRun find_VersionEditor)
    mocks_add_gui_run_target(CompileTranslationsGUIRunAll CompileTranslationsGUI --all)
    mocks_add_gui_run_target(CompileTranslationsGUIRunConcreteMixDesign CompileTranslationsGUI --module concrete_mix_design)

    mocks_add_terminal_run_target(CompileTranslationsTerminalRunAll CompileTranslationsTerminal --all)
    mocks_add_terminal_run_target(CompileTranslationsTerminalRunConcreteMixDesign CompileTranslationsTerminal --module concrete_mix_design)
endif()

