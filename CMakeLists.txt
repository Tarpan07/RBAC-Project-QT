cmake_minimum_required(VERSION 3.16)

project(RBAC_Library_System VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_NO_PACKAGE_VERSION_CHECK TRUE)
find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)
set(QT_VERSION_MAJOR 6)

set(PROJECT_SOURCES
        main.cpp



)

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
    services/DatabaseManager.h
    services/DatabaseManager.cpp
    services/UserRepository.h
    services/UserRepository.cpp
    services/PasswordUtil.h
    services/PasswordUtil.cpp
    windows/LoginWindow.ui
    windows/RegisterWindow.ui
    models/Book.h models/Book.cpp
    services/LibraryService.h services/LibraryService.cpp
    services/StorageManager.h services/StorageManager.cpp
    services/AuditLogger.h services/AuditLogger.cpp
    services/PermissionStub.h
    windows/Dashboardwindow.cpp
    windows/Dashboardwindow.h
    windows/dashboardwindow.ui
)

target_link_libraries(RBAC_Library_System PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql)

set(BUNDLE_ID_OPTION "")
if(APPLE)
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

qt_finalize_executable(RBAC_Library_System)
