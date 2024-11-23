cmake_minimum_required(VERSION 3.14)

project(qjsonrpc
    VERSION 2.0.0
    DESCRIPTION ""
    LANGUAGES CXX
)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

configure_file(
    ${PROJECT_NAME}-config.hpp.in
    ${PROJECT_NAME}-config.hpp
)

include(GNUInstallDirs)

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(${PROJECT_NAME}-compiler-flags INTERFACE)
target_compile_features(${PROJECT_NAME}-compiler-flags INTERFACE cxx_std_17)
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
target_compile_options(${PROJECT_NAME}-compiler-flags INTERFACE
    $<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wpedantic;-Wcast-align;-Wcast-qual;-Wconversion;-Wctor-dtor-privacy;-Wenum-compare;-Wfloat-equal;-Wnon-virtual-dtor;-Wold-style-cast;-Woverloaded-virtual;-Wredundant-decls;-Wsign-conversion;-Wsign-promo>>
    $<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>
)

file(GLOB_RECURSE headers CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/*.hpp"
)
file(GLOB_RECURSE sources CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/*.cpp"
)

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME} PRIVATE ${headers} ${sources})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX} VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})
target_precompile_headers(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}/stable.hpp)
target_compile_definitions(${PROJECT_NAME} PRIVATE QJSONRPC_LIBRARY)
target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(${PROJECT_NAME}
    INTERFACE ${PROJECT_NAME}-compiler-flags
    PUBLIC Qt${QT_VERSION_MAJOR}::Core
)

install(FILES ${headers}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)
install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}-compiler-flags
    EXPORT ${PROJECT_NAME}-targets
)
install(EXPORT ${PROJECT_NAME}-targets
    FILE ${PROJECT_NAME}-targets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

include(CTest)

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set(CPACK_PACKAGE_VERSION_MAJOR ${LIBQJSONRPC_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${LIBQJSONRPC_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${LIBQJSONRPC_VERSION_PATCH})
include(CPack)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}-config-version.cmake
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

export(EXPORT ${PROJECT_NAME}-targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}-targets.cmake
)
