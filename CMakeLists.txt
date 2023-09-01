cmake_minimum_required(VERSION 3.10)
project(xrock_io_library VERSION 1.0.0 DESCRIPTION "io library for xrock_gui")
find_package(PkgConfig REQUIRED)
find_package(lib_manager)

lib_defaults()
define_module_info()

if(APPLE)
add_definitions(-std=c++17)
add_definitions(-DGL_SILENCE_DEPRECATION)
endif(APPLE)
add_definitions(-DXROCK_DEFAULT_RESOURCES_PATH=\"${CMAKE_INSTALL_PREFIX}/share\")

set(FILE_DB 1)

pkg_check_modules(lib_manager REQUIRED IMPORTED_TARGET lib_manager)
pkg_check_modules(configmaps REQUIRED IMPORTED_TARGET configmaps)
pkg_check_modules(mars_utils REQUIRED IMPORTED_TARGET mars_utils)
pkg_check_modules(xrock_gui_model REQUIRED IMPORTED_TARGET xrock_gui_model)

find_package(xdbi REQUIRED)
find_package(xtypes REQUIRED)
find_package( Boost COMPONENTS system thread REQUIRED)

set(SOURCES 
  src/BaseDB.cpp
  src/RestDB.cpp
  src/ServerlessDB.cpp
  src/MultiDB.cpp
  src/XIOLibrary.cpp
)

set(HEADERS
  src/BaseDB.hpp
  src/RestDB.hpp
  src/ServerlessDB.hpp
  src/MultiDB.hpp
  src/XIOLibrary.hpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

target_compile_options(${PROJECT_NAME} PRIVATE -Wno-inconsistent-missing-override)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

target_link_libraries(${PROJECT_NAME}
        PkgConfig::lib_manager
        PkgConfig::configmaps
        PkgConfig::mars_utils
        PkgConfig::xrock_gui_model
        xtypes::xtypes_cpp
        xtypes_generator::xtypes_generator_cpp
        xdbi::xdbi_cpp
        ${Boost_SYSTEM_LIBRARY} 	
)

add_executable(test_restdb test/test_restdb.cpp)
target_link_libraries(test_restdb ${PROJECT_NAME}
          PkgConfig::configmaps
)


if(WIN32)
  set(LIB_INSTALL_DIR bin) # .dll are in PATH, like executables
else(WIN32)
  set(LIB_INSTALL_DIR lib)
endif(WIN32)


set(_INSTALL_DESTINATIONS
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${LIB_INSTALL_DIR}
  ARCHIVE DESTINATION lib
)

# Install the library into the lib folder
install(TARGETS ${PROJECT_NAME} ${_INSTALL_DESTINATIONS})

# Install headers into mars include directory
install(FILES ${HEADERS} DESTINATION include/${PROJECT_NAME})

# Prepare and install necessary files to support finding of the library 
# using pkg-config
configure_file(${PROJECT_NAME}.pc.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION lib/pkgconfig)
