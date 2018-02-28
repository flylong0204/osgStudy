# tutorial
SET(EXAMPLE_NAME 03TextureGeometry)
SET(EXAMPLE_FILES 
	texturedGeometry.cpp
	../../Util/Axis.h
	)
START_EXAMPLE()
IF(MSVC)
	SET_TARGET_PROPERTIES(${EXAMPLE_NAME} PROPERTIES FOLDER "tutorial")
ENDIF()
