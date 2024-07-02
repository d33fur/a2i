cmake_minimum_required(VERSION 3.14)

project(a2i LANGUAGES CXX VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_DIR "source")
set(INCLUDE_DIR "include")

file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS "${SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE LIB_HEADERS CONFIGURE_DEPENDS "${INCLUDE_DIR}/*.hpp")
include_directories(${INCLUDE_DIR})

include(GNUInstallDirs)

add_library(a2i STATIC ${LIB_SOURCES})

find_package(PkgConfig REQUIRED)
pkg_search_module(FFTW REQUIRED fftw3 IMPORTED_TARGET)
if(FFTW_FOUND)
    message("FFTW found")
    target_include_directories(a2i PRIVATE ${FFTW_INCLUDE_DIRS})
    target_link_libraries(a2i PRIVATE ${FFTW_LIBRARIES})
else()
    message(FATAL_ERROR "FFTW not found")
endif()

find_package(OpenCV CONFIG REQUIRED)
if(OpenCV_FOUND)
    message("OpenCV found")
    target_include_directories(a2i PRIVATE ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(a2i PRIVATE ${OpenCV_LIBS})
else()
    message(FATAL_ERROR "OpenCV not found")
endif()

target_include_directories(a2i PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

install(TARGETS a2i
    EXPORT a2iTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(DIRECTORY ${INCLUDE_DIR}/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/a2i)

install(EXPORT a2iTargets
    FILE a2iTargets.cmake
    NAMESPACE a2i::
    DESTINATION ${CMAKE_INSTALL_DATADIR}/a2i/cmake)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/a2iConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_file(a2iConfig.cmake.in "${CMAKE_CURRENT_BINARY_DIR}/a2iConfig.cmake" @ONLY)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/a2iConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/a2iConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/a2i/cmake)

export(EXPORT a2iTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/a2iTargets.cmake"
    NAMESPACE a2i::)
export(PACKAGE a2i)

find_package(Doxygen REQUIRED)

if(DOXYGEN_FOUND)
    set(DOXYGEN_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
    add_custom_target(
            documentation
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating documentation with Doxygen"
            VERBATIM
    )
endif()