# tutorial
SET(EXAMPLE_NAME 10ManualCamera)
SET(EXAMPLE_FILES 
	TestManualCamera.cpp
	../Util/Keyboardhandler.h
	../Util/Keyboardhandler.cpp
	)
START_EXAMPLE()
IF(MSVC)
	SET_TARGET_PROPERTIES(${EXAMPLE_NAME} PROPERTIES FOLDER "tutorial")
ENDIF()
