# tutorial
SET(EXAMPLE_NAME 07Switch)
SET(EXAMPLE_FILES 
	TestSwitchDOF.cpp
	../Util/findNodeVisitor.h
	../Util/findNodeVisitor.cpp
	)
START_EXAMPLE()
IF(MSVC)
	SET_TARGET_PROPERTIES(${EXAMPLE_NAME} PROPERTIES FOLDER "tutorial")
ENDIF()
