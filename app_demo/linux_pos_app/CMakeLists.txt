cmake_minimum_required(VERSION 3.10)

# 配置ARM框架还是MIPS框架
# 优先级：-DTARGET_ARCH=... > 环境变量 TARGET_ARCH > pkg_bat/target_arch.cfg > 默认 MIPS
# build.bat 会把选择的平台写入 pkg_bat/target_arch.cfg，并以 -DTARGET_ARCH 显式传入。
set(TARGET_ARCH "" CACHE STRING "Target architecture: ARM or MIPS")
set_property(CACHE TARGET_ARCH PROPERTY STRINGS ARM MIPS)

if(NOT TARGET_ARCH)
    if(DEFINED ENV{TARGET_ARCH} AND NOT "$ENV{TARGET_ARCH}" STREQUAL "")
        set(TARGET_ARCH "$ENV{TARGET_ARCH}")
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/pkg_bat/target_arch.cfg")
        file(READ "${CMAKE_CURRENT_SOURCE_DIR}/pkg_bat/target_arch.cfg" TARGET_ARCH)
        string(STRIP "${TARGET_ARCH}" TARGET_ARCH)
    endif()
endif()

if(NOT TARGET_ARCH)
    set(TARGET_ARCH "MIPS")
endif()

string(TOUPPER "${TARGET_ARCH}" TARGET_ARCH)

if(NOT TARGET_ARCH STREQUAL "ARM" AND NOT TARGET_ARCH STREQUAL "MIPS")
    message(FATAL_ERROR "TARGET_ARCH must be ARM or MIPS (current: ${TARGET_ARCH})")
endif()

set(TARGET_ARCH "${TARGET_ARCH}" CACHE STRING "Target architecture: ARM or MIPS" FORCE)
set_property(CACHE TARGET_ARCH PROPERTY STRINGS ARM MIPS)
message(STATUS "TARGET_ARCH=${TARGET_ARCH}")

# 在 project() 之前加载交叉编译平台配置，避免 CMake 按宿主机平台初始化链接规则
if(TARGET_ARCH STREQUAL "MIPS")
    include(mips-linux-setup.cmake)
    set(PLATFORM_MIPS 1)    # 君正平台
else()
    include(arm-linux-setup.cmake)
    set(PLATFORM_MIPS 0)    # 非君正平台
endif()

#工程创建
project(linux_pos_app)

# 添加C/C++宏定义
add_definitions(-DPLATFORM_MIPS=${PLATFORM_MIPS})

# 工程根目录（用于统一拼接输出与库路径）
set(PROJECT_TOP_DIR ${CMAKE_SOURCE_DIR})

# 递归搜索匹配源码目录和其子目录下的 .cpp/.c 文件
# CONFIGURE_DEPENDS：每次构建时检查通配结果，源文件有新增/删除/改名会自动重新配置，
# 无需手工删除 build 目录（需要 CMake >= 3.12）。
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.12)
    file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
else()
    file(GLOB_RECURSE SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
    message(WARNING "CMake ${CMAKE_VERSION} < 3.12: CONFIGURE_DEPENDS is unavailable, "
                    "new or deleted source files need a manual re-configure.")
endif()

if(NOT SRCS)
    message(FATAL_ERROR "No source files found under ${CMAKE_CURRENT_SOURCE_DIR}/src")
endif()

# 固定源文件顺序，避免通配顺序变化导致无谓的重新链接
list(SORT SRCS)
list(LENGTH SRCS SRC_COUNT)
message(STATUS "Source files: ${SRC_COUNT}")

# 头文件目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/dspread
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/openssl
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/image
    ${CMAKE_CURRENT_SOURCE_DIR}/src/custom_app
    ${CMAKE_CURRENT_SOURCE_DIR}/src/custom_app/battery
    ${CMAKE_CURRENT_SOURCE_DIR}/src/custom_app/http
    ${CMAKE_CURRENT_SOURCE_DIR}/src/custom_app/ota
    ${CMAKE_CURRENT_SOURCE_DIR}/src/custom_app/util
    ${CMAKE_CURRENT_SOURCE_DIR}/src/custom_app/setting
    ${CMAKE_CURRENT_SOURCE_DIR}/src/custom_app/trans
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common_api
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common_api/base64
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common_api/iso8583
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common_api/network
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common_api/pedtest
    ${CMAKE_CURRENT_SOURCE_DIR}/src/project
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lvgl
)


#配置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_TOP_DIR}/release/${TARGET_ARCH}/bin)


#添加可执行目标
add_executable( ${PROJECT_NAME} ${SRCS})

if(CMAKE_STRIP)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_STRIP}" "$<TARGET_FILE:${PROJECT_NAME}>"
        VERBATIM
    )
else()
    message(WARNING "strip tool not found; ${PROJECT_NAME} will not be stripped")
endif()


# 依赖库目录：lib/<平台>/<分类>，分类为 emv、mqtt、os、sdk
# os 分类为设备系统自带库（仅参与链接，不打进 APK，见 pkg_bat/package.bat）
set(POS_LIB_SUBDIRS emv mqtt os sdk)
set(POS_LIB_DIRS "")
foreach(POS_LIB_SUB ${POS_LIB_SUBDIRS})
    set(POS_LIB_ONE ${PROJECT_TOP_DIR}/lib/${TARGET_ARCH}/${POS_LIB_SUB})
    if(IS_DIRECTORY ${POS_LIB_ONE})
        list(APPEND POS_LIB_DIRS ${POS_LIB_ONE})
    else()
        message(STATUS "Library category not found, skipped: lib/${TARGET_ARCH}/${POS_LIB_SUB}")
    endif()
endforeach()
if(NOT POS_LIB_DIRS)
    message(FATAL_ERROR "No library directory found under lib/${TARGET_ARCH} (expected: ${POS_LIB_SUBDIRS})")
endif()
message(STATUS "Library directories: ${POS_LIB_DIRS}")
target_link_directories(${PROJECT_NAME} PRIVATE ${POS_LIB_DIRS})

#链接动态库
# target_link_libraries(${PROJECT_NAME} PUBLIC binder log pos_sdk pthread dl iconv crypto ssl sqlite3 pos_devapi posmui mxml rt xutilmate APHLang freetype Common qUICS UICS)
target_link_libraries(${PROJECT_NAME} PUBLIC binder log pos_sdk pthread dl iconv crypto ssl sqlite3 pos_devapi posmui mxml rt xutilmate freetype APHLang MQTT get_xml z kernel emvl2 entrypoint amex bancomat dpas jcb paypass paywave pure quics rupay mir interac SDK m)