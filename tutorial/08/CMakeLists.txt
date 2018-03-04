# tutorial
SET(EXAMPLE_NAME 08UpdateCallback)
SET(EXAMPLE_FILES 
	TestUpdateCallback.cpp
	../Util/findNodeVisitor.h
	../Util/findNodeVisitor.cpp
	)
START_EXAMPLE()
IF(MSVC)
	SET_TARGET_PROPERTIES(${EXAMPLE_NAME} PROPERTIES FOLDER "tutorial")
ENDIF()
