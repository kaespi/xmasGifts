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

# check if the quickmail and uuid libraries are installed
find_library(QUICKMAIL_LIBRARY NAMES quickmail)
find_library(UUID_LIBRARY NAMES uuid)
if(QUICKMAIL_LIBRARY AND UUID_LIBRARY)
    set(EMAIL_SRC src/email.cpp src/guid.cpp)
    set(EMAIL_LIBS ${QUICKMAIL_LIBRARY} ${UUID_LIBRARY})
    target_compile_definitions(${APP_NAME} PRIVATE WITH_EMAIL)
else()
    set(EMAIL_LIBS)
    set(EMAIL_SRC)
    if(NOT QUICKMAIL_LIBRARY)
        message("quickmail library not found, email feature not compiled in.")
    endif()
    if(NOT UUID_LIBRARY)
        message("uuid library not found, email feature not compiled in.")
    endif()
endif()

target_sources(${APP_NAME}
    PRIVATE
        src/config.cpp
        src/output.cpp
        src/parser.cpp
        src/shuffle.cpp
        src/xmasGifts.cpp
        ${EMAIL_SRC}
)

target_link_libraries(${APP_NAME}
    ${EMAIL_LIBS}
)
