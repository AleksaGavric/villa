cmake_minimum_required(VERSION 3.28)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(VC3DUiTests LANGUAGES CXX)
    enable_testing()
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets Test)
set(vc3d_ui_source "${CMAKE_CURRENT_LIST_DIR}/../..")

add_executable(test_copilot_ui
    test_copilot_ui.cpp
    "${vc3d_ui_source}/CommandPalette.cpp"
    "${vc3d_ui_source}/Keybinds.cpp"
    "${vc3d_ui_source}/AppTheme.cpp"
    "${vc3d_ui_source}/resources.qrc")
set_target_properties(test_copilot_ui PROPERTIES
    AUTOMOC ON AUTORCC ON AUTOUIC OFF CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
target_include_directories(test_copilot_ui PRIVATE "${vc3d_ui_source}")
target_link_libraries(test_copilot_ui PRIVATE Qt6::Widgets Qt6::Test)
add_test(NAME copilot_ui COMMAND test_copilot_ui)
set_tests_properties(copilot_ui PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen" TIMEOUT 30)

if(COMMAND vc_register_test_target)
    vc_register_test_target(test_copilot_ui)
    vc_register_ctest(copilot_ui test_copilot_ui)
endif()
