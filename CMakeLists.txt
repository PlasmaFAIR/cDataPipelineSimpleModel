cmake_minimum_required(VERSION 3.13)
project(cSimpleModel
    VERSION 0.1.0
    DESCRIPTION "C Simple Model for the FAIR Data Pipeline"
    LANGUAGES C CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(FetchContent)

if(FDPAPI_NO_FETCHCONTENT)
  find_package(fdpapi REQUIRED)
else()
  find_package(fdpapi QUIET)
  if(NOT fdpapi_FOUND)
      if(NOT DEFINED CPPDATAPIPELINEREF)
          set(CPPDATAPIPELINEREF "heads/main")
      endif()
      set(FDPAPI_URL "https://github.com/FAIRDataPipeline/cppDataPipeline/archive/refs/${CPPDATAPIPELINEREF}.zip")
      FetchContent_Declare(fdpapi URL ${FDPAPI_URL})
      FetchContent_MakeAvailable(fdpapi)
  endif()
endif()


add_executable(cSimpleModel ./include/cSimpleModel/simple_model.h ./src/simple_model.c)

target_include_directories(
    cSimpleModel
    PRIVATE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(cSimpleModel fdpapi::fdpapi)
set_target_properties(
    cSimpleModel
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin/"
)
