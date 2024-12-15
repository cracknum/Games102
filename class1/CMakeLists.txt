cmake_minimum_required(VERSION 3.30)
project(class1)

set(CMAKE_CXX_STANDARD 17)

find_package(imgui CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

file(GLOB SOURCE_FILES src/*.cpp)
file(GLOB HEADER_FILES includes/*.h)
add_executable(class1 ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(class1 PRIVATE imgui::imgui glfw OpenGL::GL)
target_include_directories(class1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/includes)