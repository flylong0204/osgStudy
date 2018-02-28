# tutorial
SET(EXAMPLE_NAME 11bOrbit)
SET(EXAMPLE_FILES 
	TestOrbit.cpp
	../Util/KeyboardHandler.h
	../Util/KeyboardHandler.cpp
	../../Util/Axis.h
	)
START_EXAMPLE()
IF(MSVC)
	SET_TARGET_PROPERTIES(${EXAMPLE_NAME} PROPERTIES FOLDER "tutorial")
ENDIF()
