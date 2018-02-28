# 
SET(EXAMPLE_NAME hudSingle)
SET(EXAMPLE_FILES 
	main.cpp
	../common/CHUDPickHandlerSingle.h
	../common/CHUDPickHandlerSingle.cpp
	../common/MyTrackballManipulator.h
	../common/MyTrackballManipulator.cpp
	)
START_EXAMPLE()
IF(MSVC)
	SET_TARGET_PROPERTIES(${EXAMPLE_NAME} PROPERTIES FOLDER "example")
ENDIF()