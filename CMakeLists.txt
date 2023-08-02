cmake_minimum_required (VERSION 3.20.0)
project (openVCB LANGUAGES CXX VERSION 1.0.0)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package (tbb REQUIRED)
find_package (glm REQUIRED)
find_package (ZSTD REQUIRED)

add_definitions(-DMAIN_PACKAGE_NAMESPACE=openVCB)
include_directories(ZSTD glm tbb)

add_library(openVCB SHARED 
    cSharpExportInterface.cpp
    openVCB.cpp
    openVCBAssembler.cpp
    openVCBBlueprint.cpp
    openVCBExpr.cpp
    openVCBMain.cpp
    openVCBPreprocessing.cpp
    openVCBReader.cpp
    openVCBSim.cpp

    macros.h
    openVCB.h
    openVCBExpr.h
    openVCB_Data.hh
    openVCB_Expr.hh
    openVCB_Helpers.hh
    openVCB_Utils.hh
)

target_link_libraries(openVCB
    zstd
)

set_target_properties(openVCB PROPERTIES PREFIX "")