cmake_minimum_required(VERSION 3.15)
project(TorrentLite VERSION 0.1 LANGUAGES CXX)

# ─── C++ STANDARD ─────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ─── VCPKG TOOLCHAIN (optional hint) ──────────────────────────────────────────
# Only necessary if not already passed via command line
# set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
#     CACHE STRING "Vcpkg toolchain file")

# ─── THREAD SUPPORT (Windows) ─────────────────────────────────────────────────
find_package(Threads REQUIRED)

# ─── OPENSSL FOR HASHING ──────────────────────────────────────────────────────
# Make sure OpenSSL is installed via vcpkg before configuring
find_package(OpenSSL REQUIRED)

# ─── INCLUDE DIRECTORIES ──────────────────────────────────────────────────────
include_directories(
  ${CMAKE_SOURCE_DIR}/include
)

# ─── SOURCE FILES ─────────────────────────────────────────────────────────────
file(GLOB_RECURSE SRC_FILES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
) 
message(STATUS "Collected source files: ${SRC_FILES}")

# ─── EXECUTABLE TARGET ────────────────────────────────────────────────────────
add_executable(${PROJECT_NAME} ${SRC_FILES})

# ─── DEFINE PLATFORM MACRO ────────────────────────────────────────────────────
target_compile_definitions(${PROJECT_NAME}
    PRIVATE PLATFORM_WINDOWS
)

# ─── LINK LIBRARIES ───────────────────────────────────────────────────────────
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Threads::Threads
        OpenSSL::Crypto
        ws2_32
)

# ─── INSTALL RULES (Optional) ─────────────────────────────────────────────────
# install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
