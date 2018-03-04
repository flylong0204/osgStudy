# tutorial
SET(EXAMPLE_NAME 11cCamera)
SET(EXAMPLE_FILES 
	main.cpp
	../Util/KeyboardHandler.h
	../Util/KeyboardHandler.cpp
	)
START_EXAMPLE()
IF(MSVC)
	SET_TARGET_PROPERTIES(${EXAMPLE_NAME} PROPERTIES FOLDER "tutorial")
ENDIF()
