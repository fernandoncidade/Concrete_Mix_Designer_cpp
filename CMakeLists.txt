cmake_minimum_required(VERSION 3.28)

project(ConcreteMixDesign LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(WIN32)
    enable_language(RC)
endif()

# Fallback condicional apenas se CMAKE_PREFIX_PATH nao for definido via preset/CLI
if(NOT DEFINED CMAKE_PREFIX_PATH OR CMAKE_PREFIX_PATH STREQUAL "")
    if(MSVC)
        set(CMAKE_PREFIX_PATH "C:/Qt/6.12.0/msvc2022_64")
    elseif(MINGW)
        set(CMAKE_PREFIX_PATH "C:/Qt/6.12.0/mingw_64")
    endif()
endif()

find_package(Qt6 6.12 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Charts
    Sql
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/source/core
    ${CMAKE_CURRENT_SOURCE_DIR}/source/gui
    ${CMAKE_CURRENT_SOURCE_DIR}/source/language
    ${CMAKE_CURRENT_SOURCE_DIR}/source/utils
)

set(CORE_SOURCES
    source/core/ConcreteStandards.hpp
    source/core/ConcreteStandards.cpp
    source/core/AggregateOptimizer.hpp
    source/core/AggregateOptimizer.cpp
    source/core/MortarContentOptimizer.hpp
    source/core/MortarContentOptimizer.cpp
    source/core/RegressionEngine.hpp
    source/core/RegressionEngine.cpp
    source/core/MixDesignCalculator.hpp
    source/core/MixDesignCalculator.cpp
    source/core/ProjectData.hpp
    source/core/ProjectData.cpp
    source/core/NormativeDatabase.hpp
    source/core/NormativeDatabase.cpp
)

set(LANGUAGE_SOURCES
    source/language/GerenciadorTraducao.hpp
    source/language/GerenciadorTraducao.cpp
)

set(UTILS_CORE_SOURCES
    source/utils/CaminhoPersistenteUtils.hpp
    source/utils/CaminhoPersistenteUtils.cpp
    source/utils/LogManager.hpp
    source/utils/LogManager.cpp
)

set(UTILS_SOURCES
    source/utils/ApplicationPathUtils.hpp
    source/utils/ApplicationPathUtils.cpp
    ${UTILS_CORE_SOURCES}
)

set(GUI_SOURCES
    source/gui/IptUnifiedDiagramWidget.hpp
    source/gui/IptUnifiedDiagramWidget.cpp
    source/gui/QuadChartWidget.hpp
    source/gui/QuadChartWidget.cpp
    source/gui/TabProjectParams.hpp
    source/gui/TabProjectParams.cpp
    source/gui/TabMaterials.hpp
    source/gui/TabMaterials.cpp
    source/gui/TabMortarContent.hpp
    source/gui/TabMortarContent.cpp
    source/gui/TabExperimentalMixes.hpp
    source/gui/TabExperimentalMixes.cpp
    source/gui/TabFinalMix.hpp
    source/gui/TabFinalMix.cpp
    source/gui/TabCosts.hpp
    source/gui/TabCosts.cpp
    source/gui/TabTechnicalReport.hpp
    source/gui/TabTechnicalReport.cpp
    source/gui/SobreDialog.hpp
    source/gui/SobreDialog.cpp
    source/gui/ManualDialog.hpp
    source/gui/ManualDialog.cpp
    source/gui/MainWindow.hpp
    source/gui/MainWindow.cpp
)

# Bibliotecas modulares reutilizadas por GUI, CLI e Suite de Testes
add_library(ConcreteMixCoreLib STATIC
    ${CORE_SOURCES}
    ${LANGUAGE_SOURCES}
    ${UTILS_CORE_SOURCES}
)
target_include_directories(ConcreteMixCoreLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/source/core
    ${CMAKE_CURRENT_SOURCE_DIR}/source/gui
    ${CMAKE_CURRENT_SOURCE_DIR}/source/language
    ${CMAKE_CURRENT_SOURCE_DIR}/source/utils
)
target_link_libraries(ConcreteMixCoreLib PUBLIC
    Qt6::Core
    Qt6::Sql
)
if(WIN32)
    target_link_libraries(ConcreteMixCoreLib PUBLIC dwmapi advapi32)
endif()
if(MSVC)
    target_compile_options(ConcreteMixCoreLib PUBLIC /utf-8 /EHsc)
endif()

add_library(ConcreteMixGuiLib STATIC
    ${GUI_SOURCES}
    source/utils/ApplicationPathUtils.hpp
    source/utils/ApplicationPathUtils.cpp
)
target_link_libraries(ConcreteMixGuiLib PUBLIC
    ConcreteMixCoreLib
    Qt6::Gui
    Qt6::Widgets
    Qt6::Charts
)

# Vincula o ícone diretamente no binário PE (.exe) no Windows
set(APP_RC_SRC "")
if(WIN32)
    set(APP_ICON_PATH "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/icons/concrete_mix_design.ico")
    set(APP_ICON_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/source/windows_app_icon.rc.in")
    if(EXISTS "${APP_ICON_TEMPLATE}" AND EXISTS "${APP_ICON_PATH}")
        file(TO_NATIVE_PATH "${APP_ICON_PATH}" CONCRETE_MIX_DESIGNER_APP_ICON_RC_PATH_NATIVE)
        string(REPLACE "\\" "\\\\" CONCRETE_MIX_DESIGNER_APP_ICON_RC_PATH "${CONCRETE_MIX_DESIGNER_APP_ICON_RC_PATH_NATIVE}")

        set(APP_ICON_RC "${CMAKE_CURRENT_BINARY_DIR}/app_icon.rc")
        configure_file("${APP_ICON_TEMPLATE}" "${APP_ICON_RC}" @ONLY)
        set(APP_RC_SRC "${APP_ICON_RC}")
    else()
        message(WARNING "Template de recurso de ícone não encontrado em ${APP_ICON_TEMPLATE} ou ícone ausente em ${APP_ICON_PATH}. O executável será gerado sem recurso de ícone RC.")
    endif()
endif()

# Copia os arquivos de traducao .qm compilados para a pasta binaria de saida
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/source/language/locale/concrete_mix_design_pt_BR.qm
          ${CMAKE_CURRENT_SOURCE_DIR}/source/language/locale/concrete_mix_design_en_US.qm
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/locale)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/source/assets/icons/concrete_mix_design.ico
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/icons)

add_executable(ConcreteMixDesign WIN32
    main.cpp
    ${APP_RC_SRC}
)

target_link_libraries(ConcreteMixDesign PRIVATE
    ConcreteMixGuiLib
)

# Garante o deploy completo de dependencias Qt (DLLs de Core, Gui, Widgets, Charts, Sql, plugins e runtimes)
function(concrete_mix_get_qt_target_location TARGET_NAME OUT_VAR)
    set(_resolved_loc "")
    if(TARGET "${TARGET_NAME}")
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(_props IMPORTED_LOCATION_DEBUG IMPORTED_LOCATION_RELWITHDEBINFO IMPORTED_LOCATION_RELEASE IMPORTED_LOCATION)
        else()
            set(_props IMPORTED_LOCATION_RELEASE IMPORTED_LOCATION_RELWITHDEBINFO IMPORTED_LOCATION IMPORTED_LOCATION_DEBUG)
        endif()
        foreach(_prop IN LISTS _props)
            get_target_property(_cand "${TARGET_NAME}" "${_prop}")
            if(_cand)
                set(_resolved_loc "${_cand}")
                break()
            endif()
        endforeach()
    endif()
    set(${OUT_VAR} "${_resolved_loc}" PARENT_SCOPE)
endfunction()

concrete_mix_get_qt_target_location(Qt6::Core APP_QT_CORE_LOCATION)
if(APP_QT_CORE_LOCATION)
    get_filename_component(APP_QT_BIN_DIR "${APP_QT_CORE_LOCATION}" DIRECTORY)
    get_filename_component(APP_QT_PREFIX_DIR "${APP_QT_BIN_DIR}/.." ABSOLUTE)
else()
    get_filename_component(APP_QT_PREFIX_DIR "${Qt6_DIR}/../../.." ABSOLUTE)
    set(APP_QT_BIN_DIR "${APP_QT_PREFIX_DIR}/bin")
endif()

find_program(WINDEPLOYQT_EXE
    NAMES windeployqt
    PATHS "${APP_QT_BIN_DIR}" "${APP_QT_PREFIX_DIR}/bin"
    NO_DEFAULT_PATH
)

set_property(GLOBAL APPEND PROPERTY JOB_POOLS windeployqt_pool=1)
option(CONCRETE_MIX_SERIALIZE_DEPLOY "Serializa os alvos de deploy windeployqt no mesmo diretorio" ON)
set(CONCRETE_MIX_LAST_DEPLOY_TARGET "")

function(concrete_mix_add_deploy_target TARGET_NAME)
    if(NOT WIN32)
        return()
    endif()

    cmake_parse_arguments(DEPLOY "INCLUDE_GUI_EXTRA;INCLUDE_APP_ASSETS" "" "" ${ARGN})

    set(_qt_dll_suffix "")
    if(MSVC AND CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(_qt_dll_suffix "d")
    endif()

    set(_deploy_commands "")
    if(WINDEPLOYQT_EXE)
        list(APPEND _deploy_commands
            COMMAND "${CMAKE_COMMAND}" -E echo "Implantando dependencias enxutas de ${TARGET_NAME} com windeployqt..."
            COMMAND "${WINDEPLOYQT_EXE}"
                --verbose 0
                --no-translations
                --no-compiler-runtime
                --no-system-d3d-compiler
                --no-system-dxc-compiler
                --no-opengl-sw
                --no-quick-import
                --skip-plugin-types generic,iconengines,networkinformation,tls
                --exclude-plugins qgif,qicns,qjpeg,qpdf,qsvg,qtga,qtiff,qwbmp,qwebp,qsqlibase,qsqlmimer,qsqloci,qsqlodbc,qsqlpsql
                "$<TARGET_FILE:${TARGET_NAME}>"
        )
    endif()

    set(_sql_dll "Qt6Sql${_qt_dll_suffix}.dll")
    if(EXISTS "${APP_QT_BIN_DIR}/${_sql_dll}")
        list(APPEND _deploy_commands
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${APP_QT_BIN_DIR}/${_sql_dll}"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/"
        )
    endif()

    set(_sqlite_plugin "${APP_QT_PREFIX_DIR}/plugins/sqldrivers/qsqlite${_qt_dll_suffix}.dll")
    if(EXISTS "${_sqlite_plugin}")
        list(APPEND _deploy_commands
            COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:${TARGET_NAME}>/sqldrivers"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${_sqlite_plugin}"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/sqldrivers/"
        )
    endif()

    if(DEPLOY_INCLUDE_GUI_EXTRA)
        foreach(_gui_base IN ITEMS "Qt6Charts" "Qt6OpenGL" "Qt6OpenGLWidgets")
            set(_gui_dll "${_gui_base}${_qt_dll_suffix}.dll")
            if(EXISTS "${APP_QT_BIN_DIR}/${_gui_dll}")
                list(APPEND _deploy_commands
                    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                        "${APP_QT_BIN_DIR}/${_gui_dll}"
                        "$<TARGET_FILE_DIR:${TARGET_NAME}>/"
                )
            endif()
        endforeach()

        set(_ico_plugin "${APP_QT_PREFIX_DIR}/plugins/imageformats/qico${_qt_dll_suffix}.dll")
        if(EXISTS "${_ico_plugin}")
            list(APPEND _deploy_commands
                COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:${TARGET_NAME}>/imageformats"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                    "${_ico_plugin}"
                    "$<TARGET_FILE_DIR:${TARGET_NAME}>/imageformats/"
            )
        endif()
    endif()

    if(MSVC AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(_msvc_crt_dir "")
        if(DEFINED ENV{VCToolsRedistDir} AND NOT "$ENV{VCToolsRedistDir}" STREQUAL "")
            file(TO_CMAKE_PATH "$ENV{VCToolsRedistDir}/x64/Microsoft.VC145.CRT" _env_crt_dir)
            if(EXISTS "${_env_crt_dir}/msvcp140.dll")
                set(_msvc_crt_dir "${_env_crt_dir}")
            endif()
        endif()
        if(NOT _msvc_crt_dir)
            foreach(_cand_crt IN ITEMS
                "C:/Program Files/Microsoft Visual Studio/18/Community/VC/Redist/MSVC/14.52.37025/x64/Microsoft.VC145.CRT"
                "C:/Program Files (x86)/Microsoft Visual Studio/18/BuildTools/VC/Redist/MSVC/14.52.37025/x64/Microsoft.VC145.CRT"
            )
                if(EXISTS "${_cand_crt}/msvcp140.dll")
                    set(_msvc_crt_dir "${_cand_crt}")
                    break()
                endif()
            endforeach()
        endif()
        if(_msvc_crt_dir)
            foreach(_crt_dll IN ITEMS "msvcp140.dll" "msvcp140_1.dll" "msvcp140_2.dll" "vcruntime140.dll" "vcruntime140_1.dll")
                if(EXISTS "${_msvc_crt_dir}/${_crt_dll}")
                    list(APPEND _deploy_commands
                        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                            "${_msvc_crt_dir}/${_crt_dll}"
                            "$<TARGET_FILE_DIR:${TARGET_NAME}>/"
                    )
                endif()
            endforeach()
        endif()
    elseif(MINGW)
        get_filename_component(_mingw_bin_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
        if(NOT EXISTS "${_mingw_bin_dir}/libstdc++-6.dll")
            if(EXISTS "C:/Qt/Tools/mingw1510_64/bin/libstdc++-6.dll")
                set(_mingw_bin_dir "C:/Qt/Tools/mingw1510_64/bin")
            else()
                set(_mingw_bin_dir "${APP_QT_BIN_DIR}")
            endif()
        endif()
        foreach(_mingw_dll IN ITEMS "libgcc_s_seh-1.dll" "libstdc++-6.dll" "libwinpthread-1.dll")
            if(EXISTS "${_mingw_bin_dir}/${_mingw_dll}")
                list(APPEND _deploy_commands
                    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                        "${_mingw_bin_dir}/${_mingw_dll}"
                        "$<TARGET_FILE_DIR:${TARGET_NAME}>/"
                )
            endif()
        endforeach()
    endif()

    if(DEPLOY_INCLUDE_APP_ASSETS)
        # Copia exclusivamente os .qm compilados, o icone .ico e os documentos utilizados em runtime
        list(APPEND _deploy_commands
            COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:${TARGET_NAME}>/locale"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/source/language/locale/concrete_mix_design_pt_BR.qm"
                "${CMAKE_CURRENT_SOURCE_DIR}/source/language/locale/concrete_mix_design_en_US.qm"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/locale/"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets/icons"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/icons/concrete_mix_design.ico"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets/icons/"
            COMMAND "${CMAKE_COMMAND}" -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/ABOUT"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets/ABOUT"
            COMMAND "${CMAKE_COMMAND}" -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/CLC"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets/CLC"
            COMMAND "${CMAKE_COMMAND}" -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/COPYRIGHT"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets/COPYRIGHT"
            COMMAND "${CMAKE_COMMAND}" -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/EULA"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets/EULA"
            COMMAND "${CMAKE_COMMAND}" -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/LICENSES"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets/LICENSES"
            COMMAND "${CMAKE_COMMAND}" -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/NOTICES"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets/NOTICES"
            COMMAND "${CMAKE_COMMAND}" -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/PRIVACY_POLICY"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets/PRIVACY_POLICY"
            COMMAND "${CMAKE_COMMAND}" -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/RELEASE"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets/RELEASE"
        )
    endif()

    if(_deploy_commands)
        add_custom_target(
            "${TARGET_NAME}Deploy"
            ALL
            DEPENDS "${TARGET_NAME}"
            ${_deploy_commands}
            COMMENT "Implantando runtime de ${TARGET_NAME}"
            JOB_POOL windeployqt_pool
            VERBATIM
        )
        if(CONCRETE_MIX_SERIALIZE_DEPLOY AND CONCRETE_MIX_LAST_DEPLOY_TARGET)
            add_dependencies("${TARGET_NAME}Deploy" "${CONCRETE_MIX_LAST_DEPLOY_TARGET}")
        endif()
        set(CONCRETE_MIX_LAST_DEPLOY_TARGET "${TARGET_NAME}Deploy" PARENT_SCOPE)
    endif()
endfunction()

concrete_mix_add_deploy_target(ConcreteMixDesign INCLUDE_GUI_EXTRA INCLUDE_APP_ASSETS)

if(WIN32)
    set_target_properties(ConcreteMixDesign PROPERTIES
        WIN32_EXECUTABLE TRUE # Executa como aplicativo GUI nativo sem abrir janela de terminal/console
    )
endif()

# ============================================================================
# Executável de Linha de Comando (CLI Standalone)
# ============================================================================
add_executable(concrete-mix-designer-cli
    source/cli/main_cli.cpp
    ${APP_RC_SRC}
)

target_link_libraries(concrete-mix-designer-cli PRIVATE
    ConcreteMixCoreLib
)

concrete_mix_add_deploy_target(concrete-mix-designer-cli)

option(CONCRETE_BUILD_TEST_SUITE "Compila a suite completa de testes modulares em tests/" OFF)
if(CONCRETE_BUILD_TEST_SUITE)
    find_package(Qt6 6.12 REQUIRED COMPONENTS
        Test
        OpenGL
        OpenGLWidgets
    )
    enable_testing()
    add_subdirectory(tests)
endif()
