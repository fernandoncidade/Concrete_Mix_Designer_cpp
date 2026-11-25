set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

set(CONCRETE_SUITE_TEST_TARGETS
    verify_calculations
    test_RuntimePluginsAndFrameworks
    test_StandardsAndNormativeDB
    test_MathematicalCoreAndOptimizers
    test_ProjectDataSerializationAndReports
    test_I18nUtilsAndPersistence
    test_GuiWorkflowsAndCharts
    test_CliIntegration
    test_MocksAndAuditTools
)

add_executable(verify_calculations
    verify_calculations.cpp
)
target_link_libraries(verify_calculations PRIVATE
    ConcreteMixCoreLib
)

add_executable(test_RuntimePluginsAndFrameworks
    test_RuntimePluginsAndFrameworks.cpp
)
target_link_libraries(test_RuntimePluginsAndFrameworks PRIVATE
    ConcreteMixGuiLib
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    Qt6::Test
)

add_executable(test_StandardsAndNormativeDB
    test_StandardsAndNormativeDB.cpp
)
target_link_libraries(test_StandardsAndNormativeDB PRIVATE
    ConcreteMixCoreLib
)

add_executable(test_MathematicalCoreAndOptimizers
    test_MathematicalCoreAndOptimizers.cpp
)
target_link_libraries(test_MathematicalCoreAndOptimizers PRIVATE
    ConcreteMixCoreLib
)

add_executable(test_ProjectDataSerializationAndReports
    test_ProjectDataSerializationAndReports.cpp
)
target_link_libraries(test_ProjectDataSerializationAndReports PRIVATE
    ConcreteMixCoreLib
    Qt6::Gui
)

add_executable(test_I18nUtilsAndPersistence
    test_I18nUtilsAndPersistence.cpp
)
target_link_libraries(test_I18nUtilsAndPersistence PRIVATE
    ConcreteMixGuiLib
    Qt6::Test
)
target_compile_definitions(test_I18nUtilsAndPersistence PRIVATE
    CONCRETE_REPO_ROOT_DIR="${CMAKE_SOURCE_DIR}"
)

add_executable(test_GuiWorkflowsAndCharts
    test_GuiWorkflowsAndCharts.cpp
)
target_link_libraries(test_GuiWorkflowsAndCharts PRIVATE
    ConcreteMixGuiLib
    Qt6::Test
)

add_executable(test_CliIntegration
    test_CliIntegration.cpp
)
target_link_libraries(test_CliIntegration PRIVATE
    ConcreteMixCoreLib
)
add_dependencies(test_CliIntegration concrete-mix-designer-cli)
target_compile_definitions(test_CliIntegration PRIVATE
    CONCRETE_CLI_EXE_PATH="$<TARGET_FILE:concrete-mix-designer-cli>"
)

add_executable(test_MocksAndAuditTools
    test_MocksAndAuditTools.cpp
    ${CMAKE_SOURCE_DIR}/mocks/find_VersionEditor.cpp
    ${CMAKE_SOURCE_DIR}/mocks/find_VersionEditor.hpp
)
set_target_properties(test_MocksAndAuditTools PROPERTIES AUTOMOC ON)
target_link_libraries(test_MocksAndAuditTools PRIVATE
    Qt6::Core
    Qt6::Widgets
)
target_compile_definitions(test_MocksAndAuditTools PRIVATE
    CONCRETE_VERSION_EDITOR_NO_MAIN
    CONCRETE_REPO_ROOT_DIR="${CMAKE_SOURCE_DIR}"
    NOMINMAX
    WIN32_LEAN_AND_MEAN
)
if(WIN32)
    target_link_libraries(test_MocksAndAuditTools PRIVATE advapi32 user32)
endif()

foreach(_suite_target IN LISTS CONCRETE_SUITE_TEST_TARGETS)
    if(MSVC)
        target_compile_options(${_suite_target} PRIVATE /utf-8 /EHsc)
    endif()
endforeach()

# Deploy exclusivo para execucao da suite de testes em build/build_suite_* (sem distribuicao externa)
set(_suite_deploy_commands "")
set(_suite_qt_targets
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Charts
    Qt6::Sql
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    Qt6::Test
)
foreach(_qt_tgt IN LISTS _suite_qt_targets)
    concrete_mix_get_qt_target_location("${_qt_tgt}" _qt_dll_path)
    if(_qt_dll_path AND EXISTS "${_qt_dll_path}")
        list(APPEND _suite_deploy_commands
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${_qt_dll_path}" "${CMAKE_BINARY_DIR}/"
        )
    endif()
endforeach()

concrete_mix_get_qt_target_location(Qt6::Core _suite_core_loc)
if(_suite_core_loc)
    get_filename_component(_suite_qt_bin "${_suite_core_loc}" DIRECTORY)
    get_filename_component(_suite_qt_root "${_suite_qt_bin}" DIRECTORY)
    set(_suite_qt_plugins "${_suite_qt_root}/plugins")

    set(_suite_plugin_specs
        "platforms/qoffscreen.dll|platforms"
        "platforms/qwindows.dll|platforms"
        "styles/qmodernwindowsstyle.dll|styles"
        "sqldrivers/qsqlite.dll|sqldrivers"
        "imageformats/qico.dll|imageformats"
    )
    foreach(_spec IN LISTS _suite_plugin_specs)
        string(REPLACE "|" ";" _parts "${_spec}")
        list(GET _parts 0 _rel_src)
        list(GET _parts 1 _rel_dst)
        set(_src_plugin "${_suite_qt_plugins}/${_rel_src}")
        if(EXISTS "${_src_plugin}")
            list(APPEND _suite_deploy_commands
                COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/${_rel_dst}"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${_src_plugin}" "${CMAKE_BINARY_DIR}/${_rel_dst}/"
            )
        endif()
    endforeach()
endif()

if(MINGW)
    set(_suite_mingw_bin "C:/Qt/Tools/mingw1510_64/bin")
    if(NOT EXISTS "${_suite_mingw_bin}/libstdc++-6.dll")
        get_filename_component(_suite_mingw_bin "${CMAKE_CXX_COMPILER}" DIRECTORY)
    endif()
    foreach(_mingw_dll IN ITEMS "libgcc_s_seh-1.dll" "libstdc++-6.dll" "libwinpthread-1.dll")
        if(EXISTS "${_suite_mingw_bin}/${_mingw_dll}")
            list(APPEND _suite_deploy_commands
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${_suite_mingw_bin}/${_mingw_dll}" "${CMAKE_BINARY_DIR}/"
            )
        endif()
    endforeach()
elseif(MSVC AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(_suite_crt_dir "")
    if(DEFINED ENV{VCToolsRedistDir} AND NOT "$ENV{VCToolsRedistDir}" STREQUAL "")
        set(_cand "$ENV{VCToolsRedistDir}/x64/Microsoft.VC145.CRT")
        if(EXISTS "${_cand}/msvcp140.dll")
            set(_suite_crt_dir "${_cand}")
        endif()
    endif()
    if(NOT _suite_crt_dir)
        foreach(_cand_crt IN ITEMS
            "C:/Program Files/Microsoft Visual Studio/18/Community/VC/Redist/MSVC/14.52.37025/x64/Microsoft.VC145.CRT"
            "C:/Program Files (x86)/Microsoft Visual Studio/18/BuildTools/VC/Redist/MSVC/14.52.37025/x64/Microsoft.VC145.CRT")
            if(EXISTS "${_cand_crt}/msvcp140.dll")
                set(_suite_crt_dir "${_cand_crt}")
                break()
            endif()
        endforeach()
    endif()
    if(_suite_crt_dir)
        foreach(_crt_dll IN ITEMS "msvcp140.dll" "msvcp140_1.dll" "msvcp140_2.dll" "vcruntime140.dll" "vcruntime140_1.dll")
            if(EXISTS "${_suite_crt_dir}/${_crt_dll}")
                list(APPEND _suite_deploy_commands
                    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${_suite_crt_dir}/${_crt_dll}" "${CMAKE_BINARY_DIR}/"
                )
            endif()
        endforeach()
    endif()
endif()

list(APPEND _suite_deploy_commands
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/assets"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/source/assets" "${CMAKE_BINARY_DIR}/assets"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/locale"
)
foreach(_qm_file IN ITEMS
    "${CMAKE_SOURCE_DIR}/source/language/locale/concrete_mix_design_pt_BR.qm"
    "${CMAKE_SOURCE_DIR}/source/language/locale/concrete_mix_design_en_US.qm")
    if(EXISTS "${_qm_file}")
        list(APPEND _suite_deploy_commands
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${_qm_file}" "${CMAKE_BINARY_DIR}/locale/"
        )
    endif()
endforeach()

add_custom_target(ConcreteMixTestSuiteDeploy ALL
    ${_suite_deploy_commands}
    COMMENT "Preparando runtime Qt, plugins, assets e traducoes locais para a suite de testes..."
    VERBATIM
)

add_custom_target(ConcreteMixTestSuite
    DEPENDS
        ${CONCRETE_SUITE_TEST_TARGETS}
        concrete-mix-designer-cli
        ConcreteMixTestSuiteDeploy
)

foreach(_suite_target IN LISTS CONCRETE_SUITE_TEST_TARGETS)
    add_dependencies(${_suite_target} ConcreteMixTestSuiteDeploy)
    add_test(NAME ${_suite_target} COMMAND ${_suite_target} WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
    set_tests_properties(${_suite_target} PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    )
endforeach()
