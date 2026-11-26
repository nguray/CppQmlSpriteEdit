cmake_minimum_required(VERSION 3.16)

project(CppQmlSpriteEdit VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)  # <-- 1. Enable automatic .qrc compilation

find_package(Qt6 REQUIRED COMPONENTS Quick Widgets)

qt_standard_project_setup(REQUIRES 6.10)

# Content of downloaded design from Figma to Qt or similar
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/importedcontent/CMakeLists.txt")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/importedcontent)
endif()

add_custom_target(docs SOURCES README.md)

qt_add_executable(appCppQmlSpriteEdit
    main.cpp
    resources.qrc
)

qt_add_qml_module(appCppQmlSpriteEdit
    URI CppQmlSpriteEdit
    QML_FILES
        Main.qml
        QML_FILES MainForm.ui.qml
        SOURCES editarea.h editarea.cpp
        SOURCES palette.h palette.cpp
        RESOURCES resources.qrc
        SOURCES colorrect.cpp colorrect.h
        SOURCES cornerrect.cpp cornerrect.h
        SOURCES editmode.cpp editmode.h
        SOURCES ellipsemode.cpp ellipsemode.h
        SOURCES fillmode.cpp fillmode.h
        SOURCES pastemode.cpp pastemode.h pencilmode.cpp pencilmode.h
        SOURCES rectanglemode.cpp rectanglemode.h selectrectmode.cpp selectrectmode.h
        SOURCES spriteimage.cpp spriteimage.h
        SOURCES pixelrect.cpp pixelrect.h
        SOURCES selectrect.cpp selectrect.h
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appCppQmlSpriteEdit PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appCppQmlSpriteEdit
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appCppQmlSpriteEdit
    PRIVATE Qt6::Quick Qt6::Widgets
)

install(TARGETS appCppQmlSpriteEdit
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
