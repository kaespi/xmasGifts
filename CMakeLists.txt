cmake_minimum_required(VERSION 3.16)

set(APP_NAME "xmasGifts")

# Define a CMake project
project(
  ${APP_NAME}
  VERSION 1.0
  DESCRIPTION "Commandline tool to organize the assignments donors-giftee in your family (like for Xmas)"
  HOMEPAGE_URL "https://github.com/kaespi/xmasGifts"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(ENABLE_DEVELOPER_MODE
    TRUE
    CACHE BOOL "Enable 'developer mode'")
set(OPT_WARNINGS_AS_ERRORS_DEVELOPER_DEFAULT TRUE)

add_executable(${APP_NAME})
target_sources(${APP_NAME}
    PRIVATE
        src/email.cpp
        src/config.cpp
        src/guid.cpp
        src/output.cpp
        src/parser.cpp
        src/shuffle.cpp
        src/xmasGifts.cpp
)

# check if the quickmail and uuid libraries are installed
find_library(QUICKMAIL_LIBRARY NAMES quickmail)
if(NOT QUICKMAIL_LIBRARY)
    message(FATAL_ERROR "quickmail library not found.")
endif()

find_library(UUID_LIBRARY NAMES uuid)
if(NOT UUID_LIBRARY)
message(FATAL_ERROR "uuid library not found.")
endif()

target_link_libraries(${APP_NAME}
    quickmail
    uuid
)
