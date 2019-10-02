cmake_minimum_required(VERSION 2.8)
PROJECT(PROJECT)

FIND_PACKAGE(VTK)
IF(VTK_FOUND)	
	INCLUDE(${VTK_USE_FILE})
ENDIF(VTK_FOUND)

FIND_PACKAGE(ITK)
include(${ITK_USE_FILE})
if (ITKVtkGlue_LOADED)
  find_package(VTK REQUIRED)
  include(${VTK_USE_FILE})
else()
  find_package(ItkVtkGlue REQUIRED)
  include(${ItkVtkGlue_USE_FILE})
  set(Glue ItkVtkGlue)
endif()
	
add_executable(PROJECT MACOSX_BUNDLE PROJECT.cxx)

TARGET_LINK_LIBRARIES(PROJECT ${Glue} ${ITK_LIBRARIES} ${VTK_LIBRARIES})
