cmake_minimum_required(VERSION 3.15)
project(FileFlock VERSION 0.1 LANGUAGES CXX)

# ─── C++ STANDARD ─────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ─── VCPKG TOOLCHAIN ──────────────────────────────────────────────────────────
# Only necessary if not already passed via CLI
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

# ─── THREAD SUPPORT (Windows) ─────────────────────────────────────────────────
find_package(Threads REQUIRED)

# ─── OPENSSL FOR HASHING ──────────────────────────────────────────────────────
find_package(OpenSSL REQUIRED)

# ─── JSONCPP SUPPORT ──────────────────────────────────────────────────────────
find_package(jsoncpp CONFIG REQUIRED)

# ─── INCLUDE DIRECTORIES ──────────────────────────────────────────────────────
include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

# ─── SOURCE FILES ─────────────────────────────────────────────────────────────
file(GLOB_RECURSE SRC_FILES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/peer_utils.cpp  # Add new source file here
)

message(STATUS "Collected source files: ${SRC_FILES}")

# ─── EXECUTABLE TARGET ────────────────────────────────────────────────────────
add_executable(${PROJECT_NAME} ${SRC_FILES})

# ─── DEFINE PLATFORM MACRO ────────────────────────────────────────────────────
if(MSVC)
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE PLATFORM_WINDOWS
    )
else()
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE PLATFORM_UNIX
    )
endif()

# ─── LINK LIBRARIES ───────────────────────────────────────────────────────────
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Threads::Threads
        OpenSSL::Crypto
        JsonCpp::JsonCpp    # jsoncpp from vcpkg
)

# ─── WINDOWS SPECIFIC LIBRARIES ───────────────────────────────────────────────
if(MSVC)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            ws2_32              # Windows socket lib
    )
endif()

# ─── INSTALL RULES (Optional) ─────────────────────────────────────────────────
# install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
