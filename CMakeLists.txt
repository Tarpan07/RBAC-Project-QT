cmake_minimum_required(VERSION 3.16)

project(RBAC_Library_System VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp



)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(RBAC_Library_System
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        services/AuthService.h
        services/AuthService.cpp
        windows/LoginWindow.h
        windows/LoginWindow.cpp
        windows/RegisterWindow.h
        windows/RegisterWindow.cpp
        models/User.h
        models/User.cpp
        services/AuthManager.h
        services/TokenManager.h
        services/AuthManager.cpp
        services/TokenManager.cpp
        services/UserRepository.h
        services/UserRepository.cpp
        services/PasswordUtil.h
        services/PasswordUtil.cpp
        windows/LoginWindow.ui
        windows/RegisterWindow.ui
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET RBAC_Library_System APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(RBAC_Library_System SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(RBAC_Library_System
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(RBAC_Library_System PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.RBAC_Library_System)
endif()
set_target_properties(RBAC_Library_System PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS RBAC_Library_System
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(RBAC_Library_System)
endif()
