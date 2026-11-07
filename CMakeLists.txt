cmake_minimum_required(VERSION 3.20)

project(ConcreteMixDesign LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(WIN32)
    enable_language(RC)
endif()

# Se CMAKE_PREFIX_PATH não estiver definido, sugere o caminho padrão do Qt 6 no ambiente MinGW
if(NOT DEFINED CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "C:/Qt/6.11.1/mingw_64")
endif()

find_package(Qt6 REQUIRED COMPONENTS
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

set(UTILS_SOURCES
    source/utils/ApplicationPathUtils.hpp
    source/utils/ApplicationPathUtils.cpp
    source/utils/CaminhoPersistenteUtils.hpp
    source/utils/CaminhoPersistenteUtils.cpp
    source/utils/LogManager.hpp
    source/utils/LogManager.cpp
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

# Vincula o ícone diretamente no binário PE (.exe) no Windows
set(APP_RC_SRC "")
if(WIN32)
    set(APP_ICON_PATH "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/icons/concrete_mix_design.ico")
    set(APP_ICON_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/windows_app_icon.rc.in")
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

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/source/assets/icons
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)

add_executable(ConcreteMixDesign WIN32
    main.cpp
    ${CORE_SOURCES}
    ${LANGUAGE_SOURCES}
    ${UTILS_SOURCES}
    ${GUI_SOURCES}
    ${APP_RC_SRC}
)

target_link_libraries(ConcreteMixDesign PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Charts
    Qt6::Sql
)

if(WIN32)
    target_link_libraries(ConcreteMixDesign PRIVATE dwmapi advapi32)
endif()

# Garante o deploy completo de dependencias Qt (DLLs de Core, Gui, Widgets, Charts, plugins e runtimes)
get_filename_component(APP_QT_PREFIX_DIR "${Qt6_DIR}/../../.." ABSOLUTE)
find_program(WINDEPLOYQT_EXE NAMES windeployqt HINTS "${CMAKE_PREFIX_PATH}/bin" "${APP_QT_PREFIX_DIR}/bin")
if(WINDEPLOYQT_EXE)
    add_custom_command(TARGET ConcreteMixDesign POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E echo "Implantando dependencias com windeployqt..."
        COMMAND "${WINDEPLOYQT_EXE}" "$<TARGET_FILE:ConcreteMixDesign>" --no-translations --compiler-runtime
        COMMENT "Copiando automaticamente todas as DLLs e plugins necessarios para execucao direta sem erros de DLL ausente"
    )
endif()

if(WIN32)
    set(APP_EXTRA_DLLS
        "Qt6Charts.dll"
        "Qt6OpenGL.dll"
        "Qt6OpenGLWidgets.dll"
        "Qt6Sql.dll"
    )
    if(MINGW)
        list(APPEND APP_EXTRA_DLLS
            "libgcc_s_seh-1.dll"
            "libstdc++-6.dll"
            "libwinpthread-1.dll"
        )
    endif()
    foreach(EXTRA_DLL IN LISTS APP_EXTRA_DLLS)
        if(EXISTS "${APP_QT_PREFIX_DIR}/bin/${EXTRA_DLL}")
            add_custom_command(TARGET ConcreteMixDesign POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                    "${APP_QT_PREFIX_DIR}/bin/${EXTRA_DLL}"
                    "$<TARGET_FILE_DIR:ConcreteMixDesign>/"
            )
        endif()
    endforeach()
endif()

# Copia os arquivos de traducao compilados .qm e os assets institucionais/icones para junto do executavel na distribuicao
add_custom_command(TARGET ConcreteMixDesign POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/source/language/locale"
        "$<TARGET_FILE_DIR:ConcreteMixDesign>/locale"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/source/assets"
        "$<TARGET_FILE_DIR:ConcreteMixDesign>/assets"
)


if(WIN32)
    set_target_properties(ConcreteMixDesign PROPERTIES
        WIN32_EXECUTABLE TRUE # Executa como aplicativo GUI nativo sem abrir janela de terminal/console
    )
endif()

add_executable(verify_calculations
    tests/verify_calculations.cpp
    ${CORE_SOURCES}
    source/utils/CaminhoPersistenteUtils.cpp
    source/utils/LogManager.cpp
)

target_link_libraries(verify_calculations PRIVATE
    Qt6::Core
    Qt6::Sql
)

if(WIN32)
    target_link_libraries(verify_calculations PRIVATE dwmapi advapi32)
endif()

# ============================================================================
# Executável de Linha de Comando (CLI Standalone)
# ============================================================================
add_executable(concrete-mix-designer-cli
    source/cli/main_cli.cpp
    ${CORE_SOURCES}
    ${LANGUAGE_SOURCES}
    ${UTILS_SOURCES}
    ${APP_RC_SRC}
)

target_link_libraries(concrete-mix-designer-cli PRIVATE
    Qt6::Core
    Qt6::Sql
)

if(WIN32)
    target_link_libraries(concrete-mix-designer-cli PRIVATE dwmapi advapi32)
endif()
