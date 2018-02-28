# 
SET(EXAMPLE_NAME hudMulti)
SET(EXAMPLE_FILES 
	main.cpp
	../common/CHUDPickHandler.h
	../common/CHUDPickHandler.cpp
	../common/MyTrackballManipulator.h
	../common/MyTrackballManipulator.cpp
	)
START_EXAMPLE()
IF(MSVC)
	SET_TARGET_PROPERTIES(${EXAMPLE_NAME} PROPERTIES FOLDER "example")
ENDIF()