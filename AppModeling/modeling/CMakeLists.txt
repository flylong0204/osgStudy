SET(LIB_NAME osgModeling)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include/)
SET(HEADER_PATH ${PROJECT_SOURCE_DIR}/include/${LIB_NAME})

SET(HEADERS
    ${HEADER_PATH}/Export
    ${HEADER_PATH}/Algorithm
    ${HEADER_PATH}/Curve
    ${HEADER_PATH}/Model
    ${HEADER_PATH}/ModelVisitor
    ${HEADER_PATH}/NormalVisitor
    ${HEADER_PATH}/TexCoordVisitor
    ${HEADER_PATH}/Utilities
    ${HEADER_PATH}/Extrude
    ${HEADER_PATH}/Lathe
    ${HEADER_PATH}/Loft
    ${HEADER_PATH}/Helix
    ${HEADER_PATH}/Bezier
    ${HEADER_PATH}/Nurbs
    ${HEADER_PATH}/Subdivision
    ${HEADER_PATH}/BspTree
    ${HEADER_PATH}/BoolOperator
    ${HEADER_PATH}/PolyMesh
)

SET(SOURCES
    Curve.cpp
    ModelVisitor.cpp
    NormalVisitor.cpp
    TexCoordVisitor.cpp
    Utilities.cpp
    Extrude.cpp
    Lathe.cpp
    Loft.cpp
    Helix.cpp
    BezierCurve.cpp
    BezierSurface.cpp
    NurbsCurve.cpp
    NurbsSurface.cpp
    Subdivision.cpp
    BspTree.cpp
    BoolOperator.cpp
    PolyMesh.cpp
)

ADD_DEFINITIONS(-DOSGMODELING_LIBRARY)
ADD_LIBRARY(${LIB_NAME} SHARED ${HEADERS} ${SOURCES})
INCLUDE_DIRECTORIES(${EXTERNAL_INCLUDE_DIR})
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
)
IF(MSVC)
	SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES FOLDER "AppModeling")
ENDIF()