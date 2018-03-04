# tutorial
SET(EXAMPLE_NAME 09Keyboard)
SET(EXAMPLE_FILES 
	testKeyboard.cpp
	../Util/KeyboardHandler.h
	../Util/KeyboardHandler.cpp
	../Util/findNodeVisitor.h
	../Util/findNodeVisitor.cpp
	)
START_EXAMPLE()
IF(MSVC)
	SET_TARGET_PROPERTIES(${EXAMPLE_NAME} PROPERTIES FOLDER "tutorial")
ENDIF()
