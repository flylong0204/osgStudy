# tutorial
SET(EXAMPLE_NAME 16Intersect)
SET(EXAMPLE_FILES 
	TestIntersect.cpp
	)
START_EXAMPLE()
IF(MSVC)
	SET_TARGET_PROPERTIES(${EXAMPLE_NAME} PROPERTIES FOLDER "tutorial")
ENDIF()
