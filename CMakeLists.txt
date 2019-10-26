cmake_minimum_required (VERSION 3.13)

project(BoostLogger LANGUAGES CXX)

# Give CMake a hint at where to find boost.
set(BOOST_ROOT "D:/apps/boost/boost_1_71_0")
set(BOOST_LIBRARYDIR ${BOOST_ROOT}/stage/lib)
set(Boost_COMPILER -vc142)
# Turn this on to debug CMake's search for Boost includes and libraries.
set(Boost_DEBUG OFF)

# Find Boost on the system.
find_package(Boost 1.71 EXACT REQUIRED
	COMPONENTS filesystem log
)

if(NOT Boost_FOUND)
	message(FATAL_ERROR "Boost was not found, but this project requires Boost. Please install Boost!")
endif()

# Create the test executable.
add_executable(${PROJECT_NAME}Test1 main1.cpp)
#[[ Add these definitions to the compilation due to issues: 
 	https://stackoverflow.com/q/41193447/3987854
	https://stackoverflow.com/q/23137637/3987854 ]]
target_compile_definitions(${PROJECT_NAME}Test1 PUBLIC 
	BOOST_ALL_NO_LIB
	BOOST_LOG_DYN_LINK
)

target_include_directories(${PROJECT_NAME}Test1 PUBLIC ${Boost_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME}Test1 PUBLIC Boost::filesystem Boost::log)
