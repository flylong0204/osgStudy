SET(LIB_NAME osgdb_osgmodeling)

SET(SOURCES
    IO_Curve.cpp
    IO_Model.cpp
    IO_Extrude.cpp
    IO_Lathe.cpp
    IO_Loft.cpp
    IO_Helix.cpp
    IO_Bezier.cpp
    IO_Nurbs.cpp
    IO_BspTree.cpp
    IO_PolyMesh.cpp
)

ADD_LIBRARY(${LIB_NAME} SHARED ${SOURCES})

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include/)
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/AppModeling/modeling)

SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
set_target_properties(${LIB_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}/bin/Debug")    

TARGET_LINK_LIBRARIES(${LIB_NAME}
    debug osg${CMAKE_DEBUG_POSTFIX}         optimized osg
    debug osgUtil${CMAKE_DEBUG_POSTFIX}     optimized osgUtil
    debug osgViewer${CMAKE_DEBUG_POSTFIX}   optimized osgViewer
    debug osgText${CMAKE_DEBUG_POSTFIX}     optimized osgText
    debug osgDB${CMAKE_DEBUG_POSTFIX}       optimized osgDB
    debug osgGA${CMAKE_DEBUG_POSTFIX}       optimized osgGA
    debug OpenThreads${CMAKE_DEBUG_POSTFIX} optimized OpenThreads
	debug osgModeling${CMAKE_DEBUG_POSTFIX} optimized osgModeling
)

IF(MSVC)
	SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES FOLDER "AppModeling")
ENDIF()