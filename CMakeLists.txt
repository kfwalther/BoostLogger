cmake_minimum_required (VERSION 3.13)

project(BoostLogger LANGUAGES CXX)

# Give CMake a hint at where to find boost.
set(BOOST_ROOT "D:/apps/boost")
# Find Boost on the system.
find_package(Boost 1.71 EXACT REQUIRED)

if(NOT Boost_FOUND)
	message(FATAL_ERROR "Boost was not found, but this project requires Boost. Please install Boost!")
endif()

# Create the test executable.
add_executable(${PROJECT_NAME}Test1 main1.cpp)
target_include_directories(${PROJECT_NAME}Test1 PUBLIC ${Boost_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME}Test1 PUBLIC ${BOOST_LIBRARIES})
