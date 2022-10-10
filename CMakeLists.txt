# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(IDF_USER_NAME $ENV{USERNAME})

if (NOT IDF_USER_NAME)
    set(IDF_USER_NAME $ENV{USER})
endif()

message("Got user name `${IDF_USER_NAME}`")

set(CMAKE_USER_PROFILE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/CMakeProfile_${IDF_USER_NAME}.cmake)
message("Select `${CMAKE_USER_PROFILE_FILE}` as user-specific profile path.")

if (EXISTS ${CMAKE_USER_PROFILE_FILE})
    include(${CMAKE_USER_PROFILE_FILE})
    message("User-specific profile was included.")
else()
    file(WRITE ${CMAKE_USER_PROFILE_FILE} "# Add your configuration for cmake")
    message("User specific profile does not exist. It has been automatically created at ${CMAKE_USER_PROFILE_FILE}")
endif()

set(IDF_TARGET "esp32c3")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(YuChunGui-ColorStrip2)
