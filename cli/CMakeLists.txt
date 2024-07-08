cmake_minimum_required(VERSION 3.5.1)

project(a2i LANGUAGES CXX VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_DIR "source")
set(INCLUDE_DIR "include")
set(JSON_DIR "json/single_include")

file(GLOB_RECURSE CLI_SOURCES CONFIGURE_DEPENDS "${SOURCE_DIR}/*.cpp")

include_directories(${INCLUDE_DIR} ${JSON_DIR})

add_executable(${PROJECT_NAME} ${CLI_SOURCES})

find_package(raylib QUIET)
if(NOT raylib_FOUND)
    # message("raylib not found")
    # message("Trying to download raylib...")
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.0
        GIT_SHALLOW 1
    )
    FetchContent_MakeAvailable(raylib)
    # message("raylib downloaded")
    target_link_libraries(${PROJECT_NAME} PUBLIC raylib)
else()
  # message("raylib found")
  target_link_libraries(${PROJECT_NAME} PUBLIC raylib)
endif()

find_package(a2i REQUIRED)
# message("a2i found")
find_package(OpenCV REQUIRED)
# message("OpenCV found")

include_directories(${OpenCV_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PUBLIC 
  raylib
  ${OpenCV_LIBS}
  a2i::a2i)
