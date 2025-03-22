cmake_minimum_required(VERSION 3.21)
message("Using toolchain file ${CMAKE_TOOLCHAIN_FILE}.")

########################################################################################################################
## Define project
########################################################################################################################
project(
        ValhallaCombat
        VERSION 1.0.0
        DESCRIPTION ""
        LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF)

include(GNUInstallDirs)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/version.rc
        @ONLY)

# Add headers
file(
        GLOB_RECURSE
        headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/**.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/**.hpp
)
list(REMOVE_ITEM headers ${CMAKE_CURRENT_SOURCE_DIR}/include/PCH.h)

# Add sources
file(
        GLOB_RECURSE
        sources
        ${CMAKE_CURRENT_SOURCE_DIR}/src/**.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/**.cc
        ${CMAKE_CURRENT_BINARY_DIR}/version.rc
)


source_group(
        TREE ${CMAKE_CURRENT_SOURCE_DIR}
        FILES
        ${sources})

########################################################################################################################
## Configure target DLL
########################################################################################################################
find_package(CommonLibSSE CONFIG REQUIRED)

add_commonlibsse_plugin(${PROJECT_NAME} SOURCES ${sources})
add_library("${PROJECT_NAME}::${PROJECT_NAME}" ALIAS "${PROJECT_NAME}")

target_include_directories(${PROJECT_NAME}
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
        $<INSTALL_INTERFACE:src>)

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

#target_link_libraries(
#		${PROJECT_NAME}
#		PRIVATE
#		ryml::ryml)

target_precompile_headers(
        ${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/PCH.h)

install(TARGETS
        ${PROJECT_NAME}
        DESTINATION
        "${CMAKE_INSTALL_LIBDIR}")

set(DISTRIBUTION_DIR "${CMAKE_CURRENT_SOURCE_DIR}/contrib/Distribution/")
set(CONTRIB_PROJECT_DIR "${DISTRIBUTION_DIR}/Plugin - ${CMAKE_BUILD_TYPE}/")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CONTRIB_PROJECT_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${PROJECT_NAME}> "${CONTRIB_PROJECT_DIR}")

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PDB_FILE:${PROJECT_NAME}> "${CONTRIB_PROJECT_DIR}")
endif ()

foreach (DEPLOY_TARGET $ENV{SkyrimPluginTargets})
    set(DEPLOY_TARGET_COMMON_DIR "${DEPLOY_TARGET}/${PROJECT_NAME} - ${CMAKE_BUILD_TYPE}/SKSE/Plugins/")
    message(STATUS "Adding deployment target ${DEPLOY_TARGET}.")

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${DEPLOY_TARGET_COMMON_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:${PROJECT_NAME}>" "${DEPLOY_TARGET_COMMON_DIR}")

    if (${CMAKE_BUILD_TYPE} STREQUAL Debug OR ${CMAKE_BUILD_TYPE} STREQUAL RelWithDebInfo)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_PDB_FILE:${PROJECT_NAME}>" "${DEPLOY_TARGET_COMMON_DIR}")
    endif ()
endforeach ()