cmake_minimum_required(VERSION 3.10)
project(opencv-tcp)
add_definitions(-Wall)

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES 14)

add_executable(${PROJECT_NAME} src/main.cpp)
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# [Poco(system)]
find_package(Poco REQUIRED COMPONENTS Foundation Net Util)
message(STATUS "Poco_FOUND: ${Poco_FOUND}")
target_include_directories(${PROJECT_NAME} PUBLIC ${Poco_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${Poco_LIBRARIES})

# [openvr(local)]
target_include_directories(${PROJECT_NAME} PUBLIC
  ${PROJECT_SOURCE_DIR}/third_party/openvr/headers
)
target_link_directories(${PROJECT_NAME} PUBLIC
  ${PROJECT_SOURCE_DIR}/third_party/openvr/lib/win64
)
target_link_libraries(${PROJECT_NAME} openvr_api)
# install(DIRECTORY ${PROJECT_SOURCE_DIR}/third_party/openvr/lib/win64
#   DESTINATION lib
# )

# Don't use install(DIRECTORIES ..), this copy the win64 folder and our exe has no way to find that dll on windows!
# Can't use install(FILES win64/* ) and I don't know why
install(DIRECTORY ${PROJECT_SOURCE_DIR}/third_party/openvr/bin/win64/
  DESTINATION bin
)
