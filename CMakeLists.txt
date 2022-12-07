cmake_minimum_required(VERSION 3.13)
project(cSimpleModel
    VERSION 0.1.0
    DESCRIPTION "C Simple Model for the FAIR Data Pipeline"
    LANGUAGES C
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(FetchContent)

add_executable(cSimpleModel ./include/cSimpleModel/simple_model.h ./src/simple_model.c)

find_package(fdpapi QUIET)
if(NOT TARGET fdpapi::fdpapic)
    FetchContent_Declare(
        cDataPipeline
        GIT_REPOSITORY https://github.com/PlasmaFAIR/cDataPipeline.git
        GIT_TAG main
    )
    FetchContent_MakeAvailable(cDataPipeline)
endif()

target_include_directories(
    cSimpleModel
    PRIVATE
    $<BUILD_INTERFACE:${cSimpleModel_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(cSimpleModel fdpapi::fdpapic)
