cmake_minimum_required(VERSION 3.10)
#Import cmake configuration file
include(arm-linux-setup.cmake)
#create project
project(linux_pos_app)

#Configure output directory
set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/release)

# Recursive search to match. cpp files in the source code directory and its subdirectories
FILE(GLOB_RECURSE SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

message(EXP_SRCS = ${SRCS})

# .h
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/dspread
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/business
    ${CMAKE_CURRENT_SOURCE_DIR}/src/posapp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/posapp/iso8583
    ${CMAKE_CURRENT_SOURCE_DIR}/src/posapp/network
    ${CMAKE_CURRENT_SOURCE_DIR}/src/posapp/encryption
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
)


# lib
link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/lib_pub
)
#Place the generated lib executable files in their respective directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}/lib)

#Add executable targets
add_executable( ${PROJECT_NAME} ${SRCS})

#Link dynamic library
target_link_libraries(${PROJECT_NAME} PUBLIC binder log pos_sdk pthread dl iconv crypto ssl sqlite3 pos_devapi posmui mxml rt xutilmate freetype APHLang kernel emvl2 entrypoint amex bancomat dpas jcb paypass paywave pure quics rupay mir interac kernelins MQTT)