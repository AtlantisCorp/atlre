# CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

set(ATL_VERSION "1.0")

# All of our tests required C++17 or later
set(CMAKE_CXX_STANDARD 17)

# Sets policy CMP0057 to "NEW".
cmake_policy(SET CMP0057 NEW)

# Adds the cmake directory to our module path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(atl VERSION ${ATL_VERSION})

# Finds the std::filesystem library.
find_package(Filesystem REQUIRED COMPONENTS Final Experimental)

# Configure ./Configure.h.
configure_file(Configure.h.in "${CMAKE_CURRENT_SOURCE_DIR}/src/ATL/Configure.h" @ONLY)

# To build shared libraries in Windows, we set CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS to TRUE.
# See https://cmake.org/cmake/help/v3.4/variable/CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS.html
# See https://blog.kitware.com/create-dlls-on-windows-without-declspec-using-new-cmake-export-all-feature/
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Create our library target.
add_library(atl SHARED)

file(GLOB ATL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/ATL/*.cpp")
file(GLOB ATL_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/ATL/*.h")
target_sources(atl PRIVATE ${ATL_SOURCES} ${ATL_HEADERS})

# This will name our output to 'atl.1.0'.
# For now we have to set the NO_SONAME property because i saved everything inside a Google Drive
# folder, which doesn't allow any symlinks...
set_target_properties(atl
PROPERTIES
	VERSION ${ATL_VERSION}
	SOVERSION ${ATL_VERSION}
    CXX_STANDARD 17
    NO_SONAME TRUE
)

# Let's set compiler-specific flags
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    # G++
    target_compile_options(atl PRIVATE -Wall -Wextra)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    # MSVC
    target_compile_options(atl PRIVATE /EHsc /MTd /W2 /c)
    # Set the DLLEXPORT variable to export symbols
    target_compile_definitions(atl PRIVATE WIN_EXPORT)
endif()

target_include_directories(atl PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/libs/glm")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs/glm")
target_link_libraries(atl PRIVATE glm)

# Adds the std::filesystem library.
target_link_libraries(atl PRIVATE std::filesystem)
