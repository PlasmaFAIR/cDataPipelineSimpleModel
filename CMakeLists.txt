cmake_minimum_required(VERSION 3.13)
project(cSimpleModel
    VERSION 0.1.0
    DESCRIPTION "C Simple Model for the FAIR Data Pipeline"
    LANGUAGES C CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(FetchContent)

add_executable(cSimpleModel ./include/cSimpleModel/simple_model.h ./src/simple_model.c)

find_package(fdpapi QUIET)
if(NOT fdpapi_FOUND)
    FetchContent_Declare(
        fdpapi
        GIT_REPOSITORY https://github.com/FAIRDataPipeline/cppDataPipeline.git
        GIT_TAG c_api
    )
    FetchContent_MakeAvailable(fdpapi)
endif()

target_include_directories(
    cSimpleModel
    PRIVATE
    $<BUILD_INTERFACE:${cSimpleModel_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(cSimpleModel fdpapi::fdpapi)
