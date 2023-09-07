cmake_minimum_required (VERSION 3.20.0)
project (openVCB LANGUAGES CXX VERSION 1.0.0)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED TRUE)

#find_package (TBB REQUIRED)
find_package (glm REQUIRED)
find_package (ZSTD REQUIRED)

add_definitions (-DMAIN_PACKAGE_NAMESPACE=openVCB)

add_library (openVCB SHARED 
#add_executable (openVCB
    cSharpExportInterface.cpp
    openVCB.cpp
    openVCBAssembler.cpp
    openVCBBlueprint.cpp
    openVCBExpr.cpp
    openVCBMain.cpp
    openVCBPreprocessing.cpp
    openVCBReader.cpp
    openVCBSim.cpp

    openVCB.h
    openVCBExpr.h
    openVCB_Data.hh
    openVCB_Expr.hh
    openVCB_Helpers.hh
    openVCB_Utils.hh
    macros.h
)

target_include_directories (openVCB PRIVATE
    "${GLM_INCLUDE_DIRS}"
)

target_link_libraries (openVCB
    zstd::libzstd_shared
    #TBB::tbb
)

IF (WIN32)
    set_target_properties (openVCB PROPERTIES PREFIX "")
ENDIF ()
